import subprocess
import os

EXAMPLES_DIR = '/home/leon3428/dev/Lexis/test/test2/examples'
PROJECT_ROOT = '/home/leon3428/dev/Lexis'

def main():

    for file in os.listdir(EXAMPLES_DIR):
        name, extension = file.split('.')

        if extension == 'lan':
            # run Lexis
            with open(os.path.join(EXAMPLES_DIR, file)) as configFile:
                configFileContent = configFile.read()
                generateP = subprocess.run([os.path.join(PROJECT_ROOT, 'build/src/Lexis')], input=configFileContent, capture_output=True, text=True)

            # compile Lexer output
            err = subprocess.run(['cmake', '--build', os.path.join(PROJECT_ROOT ,'build'), '--target', 'Test2'], capture_output=True, text=True)


            # run Lexer and compare output

            with open(os.path.join(EXAMPLES_DIR, name + '.in')) as inputFile, open(os.path.join(EXAMPLES_DIR, name + '.out')) as outputFile:
                inputFileContent = inputFile.read()
                outputFileContent = outputFile.read()

                LexerProcess = subprocess.run([os.path.join(PROJECT_ROOT, 'build/test/test2/Test2')], input=inputFileContent, capture_output=True, text=True)

                if LexerProcess.stdout == outputFileContent:
                    print('Test passed for', name)
                else:
                    print('Test failed for', name)
                    print(LexerProcess.stdout, '\n\n', outputFileContent)
                    break


if __name__ == '__main__':
    main()