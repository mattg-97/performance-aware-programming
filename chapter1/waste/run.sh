#run python
python3 main.py

#generate ASM
rm -rf ./main.s
clang -target x86_64-apple-macos -O0 -S main.c

#run C code
clang -O0 main.c
./a.out
rm -rf a.out