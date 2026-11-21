import glob, os, shutil, subprocess, sys, argparse

def main(args):
    retval:int = 0

    rootpath = os.getcwd()
    files = [f for f in glob.glob(os.path.join(os.getcwd(), '**', '*.cmake'), recursive=True) if '__build__' not in f]
    print(files, flush=True)

    for file in files:
        basep = os.path.dirname(file)
        print('Test path:', basep, flush=True)

        # Reset directory, delete previous runs
        os.chdir(basep)
        try: shutil.rmtree('__build__/', ignore_errors=True)
        except: pass

        # Run and test
        os.mkdir('__build__')
        os.chdir('__build__')
        print('Configure the CMake', flush=True)
        retval |= subprocess.run(['cmake', '--fresh', '-DCMAKE_C_COMPILER=gcc', '-DCMAKE_CXX_COMPILER=g++', '-S../..', '-G', 'Ninja', '-DTEST_CMAKE_FILE_NAME={}'.format(file)]).returncode

        print('Compile', flush=True)
        retval |= subprocess.run(['cmake', '--build', '.']).returncode
        print('Run test', flush=True)
        retval |= subprocess.run(['ctest', '.', '--output-on-failure', '-C', 'Debug']).returncode

    return retval

# Get parser
def get_parser():
    parser = argparse.ArgumentParser()
    parser.add_argument("--github", required=False, action='store_true', help="Flag if test runs on Github workflow")
    return parser

# Run the script
if __name__ == '__main__':
    print('Main script running', flush=True)

    parser = get_parser()
    sys.exit(1 if main(parser.parse_args()) != 0 else 0)
