import argparse
import sys

from version import build_tool_version
from arg_options import add_arg_options
from commands.compile import CompileAssetCommand
from commands.pack import PackAssetsCommand
from commands.embed import EmbedCommand

if len(sys.argv) <= 1:
    sys.exit('Not enough arguments specified.')

parser = argparse.ArgumentParser(
    prog='BuildTool',
    description=f'Polly BuildTool {build_tool_version}',
    epilog='https://polly2d.org')

# Add options based on the command given.
command_name = sys.argv[1]
add_arg_options(command_name, parser)
sys.argv.remove(command_name)

args, unrecognized_args = parser.parse_known_args()

if command_name == 'compile':
    command = CompileAssetCommand(
        args.encryptionkey,
        args.base,
        args.asset,
        args.dst,
        args.optimize)
elif command_name == 'pack':
    command = PackAssetsCommand(
        args.encryptionkey,
        args.dst,
        args.optimize,
        files=unrecognized_args)
elif command_name == 'embed':
    command = EmbedCommand(
        args.filename,
        args.dst_filename_hpp,
        args.dst_filename_cpp,
        args.c_name)

assert (command is not None)

command.execute()
