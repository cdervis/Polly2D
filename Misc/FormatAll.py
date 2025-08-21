# Convenience script that formats all known source files in Polly using clang-format.
# Simply call this from the Polly root folder, e.g.:
#
# Polly > python Misc/FormatAll.py

import os


def formatSourcesIn(dir: str):
    for root, dirs, files in os.walk(dir):
        if 'Bindings' in root:
            continue

        print('-', root)
        for filename in files:
            if 'magic_enum' in filename:
                continue
            
            if filename.endswith('.hpp') or filename.endswith('.inl'):
                print('  -', filename)
                os.system(f'clang-format -i {os.path.join(root, filename)}')


print('Formatting all known source files...')

formatSourcesIn('Include')
formatSourcesIn('Src')
formatSourcesIn('Apps')

print('Formatting finished')
