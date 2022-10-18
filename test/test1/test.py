"""
Trenutno cita regexe iz filea regex.txt red po red pa za svaki regex napravi poseban file
gdje je u prvom redu taj regex, a u svim ostalim neki string i 0 ako ne zadovoljava izraz, a 1 ako zadovoljava
"""

import re
import itertools

ALPHABET = ["0", "1", "2"]
MAX_LENGTH = 10

def main():
    with open("regex.txt") as f:
        for cnt, line in enumerate(f.readlines()):
            line = line.strip()
            regex = '\A' + line + '\Z'

            with open(str(cnt) + '.txt', 'w+') as test:
                test.write(line + '\n')
                for i in range(1, MAX_LENGTH + 1):
                    for subset in itertools.product(ALPHABET, repeat=i):
                        string = ''.join(subset)

                        tmp = re.match(regex, string)
                        match = 0
                        try:
                            start, end = tmp.span()
                            if end - start == len(string):
                                match = 1
                        except:
                            pass
                        test.write(' '.join([string, str(match)]) + '\n')
                

if __name__ == "__main__":
    main()