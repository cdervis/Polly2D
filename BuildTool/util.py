import struct


class Util:
    @staticmethod
    def load_file_contents(filename: str):
        with open(filename, 'rb') as fs:
            return bytearray(fs.read())

    @staticmethod
    def get_clean_path(path: str, withEndingSlash=None):
        path = path.replace('\\', '/')

        if len(path) > 0:
            if withEndingSlash is True:
                if not path.endswith('/'):
                    path += '/'
            elif path.endswith('/'):
                path = path.rstrip('/')

        return path

    @staticmethod
    def file_size_display_str(size: int):
        return f'{(size / 1000000):.1f} MB' if size > 1000000 else f'{(size / 1000):.1f} KB'

    @staticmethod
    def xor_crypt_str(var: str, key: str):
        result = ''

        for i in range(len(var)):
            result += chr(ord(var[i]) ^ ord(key[i % len(key)]))

        return result


class BinaryWriter:
    def __init__(self, out_stream, encryption_key: str = None):
        self.out_stream = out_stream
        self.encryption_key = encryption_key

    def write_i32(self, value: int):
        self.out_stream.write(struct.pack('i', value))

    def write_u32(self, value: int):
        self.out_stream.write(struct.pack('I', value))

    def write_u8(self, value: int):
        self.out_stream.write(struct.pack('B', value))

    def write_bytes_no_length(self, value: bytes):
        self.out_stream.write(value)

    def write_bytes(self, value: bytes):
        self.write_u32(len(value))
        self.write_bytes_no_length(value)

    def write_str(self, value: str):
        encoded = bytes(value.encode('utf-8'))
        self.write_u32(len(encoded))
        self.out_stream.write(encoded)

    def write_str_encrypted(self, value: str):
        self.write_str(Util.xor_crypt_str(value, self.encryption_key))

    def tell(self):
        return self.out_stream.tell()

    def seek(self, offset: int):
        self.out_stream.seek(offset)


class BinaryReader:
    def __init__(self, in_stream, encryption_key: str = None):
        self.in_stream = in_stream
        self.encryption_key = encryption_key

    def read_i32(self) -> int:
        return struct.unpack('i', self.in_stream.read(4))[0]

    def read_u32(self) -> int:
        return struct.unpack('I', self.in_stream.read(4))[0]

    def read_u8(self) -> int:
        return struct.unpack('B', self.in_stream.read(1))[0]

    def read_str(self) -> str:
        str_len = self.read_u32()
        return self.in_stream.read(str_len).decode('utf-8') if str_len > 0 else ''

    def tell(self):
        return self.in_stream.tell()

    def seek(self, offset: int):
        self.in_stream.seek(offset)
