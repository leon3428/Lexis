import subprocess
import os

EXAMPLES_DIR = '/mnt/d/lexis/test/test3/examples'
PROJECT_ROOT = '/mnt/d/lexis'

def main():

    for file in os.listdir(EXAMPLES_DIR):
        #name, extension = file.split('.')


    
        # run Lexis
        with open(os.path.join(EXAMPLES_DIR, file, 'test.lan')) as configFile:
            configFileContent = configFile.read()
            #print(configFileContent)
            generateP = subprocess.run([os.path.join(PROJECT_ROOT, 'build/src/Lexis')], input=configFileContent, capture_output=True, text=True)
            if(file.startswith('09')):
                print(generateP.stdout)
        # compile Lexer output
        err = subprocess.run(['cmake', '--build', os.path.join(PROJECT_ROOT ,'build'), '--target', 'Test3'], capture_output=True, text=True)


        # run Lexer and compare output

        with open(os.path.join(EXAMPLES_DIR, file, 'test.in')) as inputFile, open(os.path.join(EXAMPLES_DIR, file, 'test.out')) as outputFile:
            inputFileContent = inputFile.read()
            outputFileContent = outputFile.read()

            LexerProcess = subprocess.run([os.path.join(PROJECT_ROOT, 'build/test/test3/Test3')], input=inputFileContent, capture_output=True, text=True)

            if LexerProcess.stdout == outputFileContent:
                print('Test passed for', file)
            else:
                print('Test failed for', file)
                print(LexerProcess.stdout, '\n\n', outputFileContent)
                break
        

if __name__ == '__main__':
    main()