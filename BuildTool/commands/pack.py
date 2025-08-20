import io
import struct
import sys

from util import BinaryWriter, BinaryReader, Util
from version import build_tool_version_nums


class PackAssetsCommand:
    def __init__(self, encryption_key: str, dst: str, optimize: bool, files: list[str]):
        self.encryption_key = encryption_key
        self.dst_filename = dst
        self.optimize = optimize
        self.files = files

    def execute(self):
        with open(self.dst_filename, 'wb') as fs:
            writer = BinaryWriter(fs, self.encryption_key)
            PackAssetsCommand.__write_magic(writer)
            PackAssetsCommand.__write_version(writer)
            self.__write_assets(writer)

    @staticmethod
    def __write_magic(writer: BinaryWriter):
        writer.write_u8(ord('p'))
        writer.write_u8(ord('l'))
        writer.write_u8(ord('a'))

    @staticmethod
    def __write_version(writer: BinaryWriter):
        major, minor, revision = build_tool_version_nums
        writer.write_u8(major)
        writer.write_u8(minor)
        writer.write_u8(revision)

    def __write_assets(self, writer: BinaryWriter):
        writer.write_u32(len(self.files))

        names = set([])
        major, minor, revision = build_tool_version_nums

        for file in self.files:
            file_contents = Util.load_file_contents(file)

            with io.BytesIO(file_contents) as fs:
                reader = BinaryReader(fs)

                asset_version_major = reader.read_u8()
                asset_version_minor = reader.read_u8()
                asset_version_revision = reader.read_u8()

                if major != asset_version_major or minor != asset_version_minor or revision != asset_version_revision:
                    sys.exit(
                        f'Version mismatch between current version of Polly ({major}.{minor}.{revision}) and the'
                        f'compiled asset ({asset_version_major}.{asset_version_minor}.{asset_version_revision}).')

                asset_name = reader.read_str()
                pos = fs.tell()

            if asset_name in names:
                sys.exit(f'Attempting to pack an asset named "{asset_name}" multiple times,'
                         f'which would produce ambiguous names.')

            names.add(asset_name)

            # Write the asset entry into the archive.
            writer.write_u8(major)
            writer.write_u8(minor)
            writer.write_u8(revision)
            writer.write_str_encrypted(asset_name)

            # Don't slice into file_contents, because that's a deep copy.
            # Instead, just remove the first N bytes.
            # for i in range(0,pos):

            writer.write_bytes(file_contents[pos:])

        files_desc = '1 asset' if len(
            self.files) == 1 else f'{len(self.files)} assets'

        print(
            f'Packed {files_desc} to archive (approx. {Util.file_size_display_str(writer.tell())})')
