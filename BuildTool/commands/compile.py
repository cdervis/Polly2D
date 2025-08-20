import base64
import io
import os
import pathlib
import json
import zlib

from util import Util, BinaryWriter
from version import build_tool_version_nums

asset_suffix = '.asset'
image_file_extensions = ['png', 'bmp', 'jpg',
                         'jpeg', 'hdr', 'psd', 'tga', 'gif']


class CompileAssetCommand:
    def __init__(self, encryption_key: str, base: str, asset: str, dst: str, optimize: bool):
        self.encryption_key = encryption_key
        self.base = base
        self.asset_filename = os.path.join(base, asset)
        self.dst_filename = dst
        self.optimize = optimize
        self.asset_name = Util.get_clean_path(asset)

    def execute(self):
        with self.__process_asset() as processed_data:
            self.__postprocess_asset(processed_data)

    def __process_asset(self):
        ext = pathlib.Path(self.asset_name).suffix.lower().lstrip('.')

        fs = io.BytesIO()
        writer = BinaryWriter(fs, self.encryption_key)

        if ext in image_file_extensions:
            self.__process_image(writer)
        elif ext == 'shd':
            self.__process_shader(writer)
        elif ext == 'ttf':
            self.__process_font(writer)
        elif ext in ['mp3', 'wav', 'ogg']:
            self.__process_sound(writer)
        elif self.__is_spine_skeleton(ext):
            is_json = ext == 'json'
            self.__process_spine_skeleton(writer, is_json)
        elif ext == 'atlas':
            self.__process_spine_atlas(writer)
        else:
            self.__process_misc(writer)

        fs.seek(0)

        return fs

    def __postprocess_asset(self, processed_data: io.BytesIO):
        compressed_data = zlib.compress(
            processed_data.read(), 9 if self.optimize else 0)

        with open(self.dst_filename, 'wb') as fs:
            # Tag the file
            writer = BinaryWriter(fs)

            # Version
            major, minor, revision = build_tool_version_nums
            writer.write_u8(major)
            writer.write_u8(minor)
            writer.write_u8(revision)

            # Name
            writer.write_str(self.asset_name)

            # Data
            writer.write_bytes_no_length(compressed_data)

    def __load_asset_contents(self):
        return Util.load_file_contents(self.asset_filename)

    def __process_image(self, writer: BinaryWriter):
        writer.write_u8(ord('i'))
        writer.write_bytes_no_length(self.__load_asset_contents())

    def __process_shader(self, writer: BinaryWriter):
        writer.write_u8(ord('s'))
        shader_source_bytes = self.__load_asset_contents()
        shader_source_code = shader_source_bytes.decode('utf-8')
        writer.write_str_encrypted(shader_source_code)

    def __process_font(self, writer: BinaryWriter):
        writer.write_u8(ord('f'))
        writer.write_bytes_no_length(self.__load_asset_contents())

    def __process_sound(self, writer: BinaryWriter):
        writer.write_u8(ord('a'))
        writer.write_bytes_no_length(self.__load_asset_contents())

    def __process_spine_skeleton(self, writer: BinaryWriter, is_json: bool):
        writer.write_u8(ord('x'))
        writer.write_u8(1 if is_json else 0)
        writer.write_bytes_no_length(self.__load_asset_contents())

    def __process_spine_atlas(self, writer: BinaryWriter):
        writer.write_u8(ord('y'))
        writer.write_bytes_no_length(self.__load_asset_contents())

    def __process_misc(self, writer: BinaryWriter):
        writer.write_u8(ord('m'))
        writer.write_bytes_no_length(self.__load_asset_contents())

    def __is_spine_skeleton(self, ext: str):
        if ext == 'skel':
            return True

        if ext == 'json':
            with open(self.asset_filename) as f:
                d = json.load(f)
                return d['skeleton'] is not None and d['bones'] is not None

        return False
