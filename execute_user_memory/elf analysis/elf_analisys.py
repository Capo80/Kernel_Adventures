import sys
from pwn import *
from elftools.elf.elffile import ELFFile

context.os = "linux"
context.arch = "amd64"

def print_istr_range(istr_array):
    for i in istr_array:
        print(i)

def analyze_syscalls(text_data):
    text_data_lines = text_data.split("\n")
    for idx, istr in enumerate(text_data_lines):
        if ("syscall" in istr):
            print_istr_range(text_data_lines[idx-10:idx+1])
            print("---")


def process_file(filename):
    print('In file:', filename)
    with open(filename, 'rb') as f:
        elffile = ELFFile(f)

        for section in elffile.iter_sections():
            if section.name.startswith('.text'):
                analyze_syscalls(disasm(section.data()))



if __name__ == '__main__':
    if sys.argv[1] == '--files':
        for filename in sys.argv[2:]:
            process_file(filename)
    else:
        print("<usage> elf_analys.py --files <files>")
    