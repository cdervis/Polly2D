import argparse
import sys


def add_arg_options(command_name: str, parser: argparse.ArgumentParser):
    if command_name == 'compile':
        parser.add_argument('--base',
                            help='The absolute directory of assets.',
                            required=True)

        parser.add_argument('--asset',
                            help='Path to the asset to compile.',
                            required=True)

        parser.add_argument('--dst',
                            help='Path to the destination asset file.',
                            required=True)

        parser.add_argument('--optimize',
                            help='Optimize the asset\'s contents', action='store_true')

        parser.add_argument('--encryptionkey',
                            help='The key to use for the encryption of asset data.',
                            required=True)
    elif command_name == 'pack':
        parser.add_argument('--dst',
                            help='Path to the destination archive.',
                            required=True)

        parser.add_argument('--optimize',
                            help='Use maximum compression when generating the archive.',
                            action='store_true')

        parser.add_argument('--encryptionkey',
                            help='The key to use for the encryption of asset data.',
                            required=True)
    elif command_name == 'embed':
        parser.add_argument('--filename',
                            help='The filename of the asset to embed.',
                            required=True)

        parser.add_argument('--dst_filename_hpp',
                            help='Path to the destination .hpp file.',
                            required=True)

        parser.add_argument('--dst_filename_cpp',
                            help='Path to the destination .cpp file.',
                            required=True)

        parser.add_argument('--c_name',
                            help='The name of the C++ variable to generate.',
                            required=True)
    else:
        sys.exit(f'Unknown command "{command_name}" specified.')
