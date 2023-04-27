
from elftools.elf.elffile import ELFFile
import argparse
import re

class elf_blob:

    def __init__(self, text=b"", rodata=b"", sys_patches=[], ro_patches=[], rodata_pref=0):
        self.text = text
        self.rodata = rodata
        self.sys_patches = sys_patches
        self.ro_patches = ro_patches
        self.rodata_pref = rodata_pref

    def set_text(self, text):
        self.text = text

    def set_rodata(self, rodata):
        self.rodata = rodata

    def set_rodata_pref(self, rodata_pref):
        self.rodata_pref = rodata_pref

    def add_sys_patch(self, patch):
        self.sys_patches.append(patch)

    def add_ro_patch(self, patch):
        self.ro_patches.append(patch)

    def __find_syscalls(self):
        return re.finditer(b"\x48\xc7.{5}\x0f\x05", self.text)

    def process_text(self, text_section):
        self.text = text_section
        syscalls_iter = self.__find_syscalls()
        for m in syscalls_iter:
            print(m.span())
            self.add_sys_patch(m.span()[0])
        return 0

    def process_rodata(self, rodata_section):
        print("not implemented yet!")
        return -1

    def to_header(self):
        header = ""
        # print(self.text)
        if (len(self.text) != 0):
            header += "char text[] = {" + [i for i in self.text].__str__()[1:-1] + "};\n"
        if (len(self.rodata) != 0):    
            header += "char *rodata[] = " + [i for i in self.rodata].__str__()[1:-1] + ";\n"
        if (len(self.sys_patches) != 0):
            header += "unsigned long syscalls_addr[] = {" + self.sys_patches.__str__()[1:-1] + "};\n"
        if (len(self.ro_patches) != 0):    
            header += "unsigned long rodatas_addr[] = {" + self.ro_patches.__str__()[1:-1] + "};\n"
        header += "unsigned long rodata_pref = " + str(self.rodata_pref) + ";\n"
        return header


def process_file(filename, sections, format):
    print('Working on..', filename)

    with open(filename, 'rb') as f:
        elffile = ELFFile(f)
        elf_blob_out = elf_blob()

        for section in elffile.iter_sections():
            if section.name.startswith('.text') and "t" in sections:
                elf_blob_out.process_text(section.data())
            if section.name.startswith('.rodata') and "r" in sections:
                elf_blob_out.process_rodata(section.data())

        if format == "header":
            # print(elf_blob_out.to_header())
            fd = open(filename + "_blob_header.h", "w")
            fd.write(elf_blob_out.to_header())
            fd.close()
            print("Result saved in: ", filename + "_blob_header.h")
# sections arg
# t = .text
# r = .rodata
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--file', help='File to decompose', required=True)
    parser.add_argument('--sections', default="rt", help='Section to inlude in the blob')
    parser.add_argument('--format', default="header", help='format of the blob', choices=["header", "bytes"])
    args = parser.parse_args()
    process_file(args.file, args.sections, args.format)
