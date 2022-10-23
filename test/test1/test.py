import re
import itertools
import subprocess

ALPHABET = ["0", "1", "2"]
MAX_LENGTH = 7

def main():
    with open("regex.txt") as f:
        for cnt, line in enumerate(f.readlines()):
            print('[info] Testing regex', cnt)
            failCnt = 0
            testCnt = 0

            regex = line.strip()

            for i in range(1, MAX_LENGTH + 1):
                for subset in itertools.product(ALPHABET, repeat=i):
                    testCnt+=1

                    string = ''.join(subset)

                    tmp = re.fullmatch(regex, string)
                    match = 0
                    try:
                        start, end = tmp.span()
                        if end - start == len(string):
                            match = 1
                    except:
                        pass

                    p = subprocess.run(['../../build/test/test1/Test1'], input=line + ' ' + string + '\n', capture_output=True, text=True)
                    
                    if len(p.stdout) == 0 or int(p.stdout[0]) != match:
                        print('[Fail] Regex: ' + line + ' Sequence: ' + string)
                        print(int(p.stdout[0]), match)
                        failCnt+=1
                
            print('[info]', failCnt, '/', testCnt, 'failed')

if __name__ == "__main__":
    main()