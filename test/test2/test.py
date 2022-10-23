import subprocess

def main():
    with open("examples/minusLang.in") as f:
        content = f.read()

        p = subprocess.run(['../../build/test/test2/Test2'], input=content, capture_output=True, text=True)
        print(p)
        print(p.stdout)

if __name__ == '__main__':
    main()