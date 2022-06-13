import os
import subprocess

if __name__ == '__main__':
    os.chdir(os.path.dirname(__file__) + "/..")
    files = subprocess.run(['git', 'ls-files', '--', '*.c', '*.h'], check=True, capture_output=True).stdout.decode().strip('\n').split('\n')

    argv = ['clang-format']
    argv += files
    argv.append('-i')
    print(argv)
    try:
        subprocess.run(argv)
    except FileNotFoundError:
        print('Error: Could not find and execute clang-format.')
        print('Make sure it\'s intalled and aliased to versionless \'clang-format\'')


