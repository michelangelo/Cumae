"""
MIT License

Copyright (c) 2016 Michelangelo De Simone <michel@ngelo.eu>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
"""
import argparse
import datetime
import os
import re
import sys

"""
The concrete representation of a PBM file.
"""
class PBMFile:
    """
    Initializes a new PBMFile instance with the supplied array
    of text lines (eg: from a semi-parsed textfile).
    """
    def __init__(self, pbmtextlines, name, outfname):
        self.inner_pbmlines = pbmtextlines
        self.name = name
        self.outfname = outfname

    def genframefile(self):
        n = self.name.upper()
        with open(self.outfname, mode='w') as f:
            f.write('/*\n')
            f.write(' * THIS IS A CUMAE-GENERATED FILE, DO NOT MODIFY!\n')
            f.write(' *\n')
            f.write(' * Generated on: %s\n' % datetime.datetime.now())
            f.write(' */\n\n')
            f.write('#ifndef %s_h\n' % n)
            f.write('#define %s_h\n\n' % n)
            f.write('#include <stdint.h>\n')
            f.write('#include <avr/pgmspace.h>\n\n')
            f.write('const uint8_t %s_frame_data[] PROGMEM = {\n' % self.name)
            f.write(self.outbytes)
            f.write('\n};\n')

            f.write('#endif /* %s_h */\n' % n)

    def genframedata(self):
        assert(self.rows % 4 == 0)
        assert(self.columns % 4 == 0)

        self.outbytes = ''

        # Generate odd bytes on ONE line
        for pixeline in self.pixeldata:
            pl = len(pixeline) - 1 # Odd bytes have even indexes
            for i in range(0, int(self.columns / 8), 1):
                oddbyte = 0
                if pixeline[pl - 1] == 1:
                    oddbyte = oddbyte | (0x3 << 6)
                if pixeline[pl - 1 - 2] == 1:
                    oddbyte = oddbyte | (0x3 << 4)
                if pixeline[pl - 1 - 4] == 1:
                    oddbyte = oddbyte | (0x3 << 2)
                if pixeline[pl - 1 - 6] == 1:
                    oddbyte = oddbyte | (0x3 << 0)

                pl -= 8
                self.outbytes += '0x' + format(oddbyte, '02x') + ', '

            self.outbytes += '\n'

            # Generate even bytes on ONE line
            pl = 1
            for i in range(0, int(self.columns / 8), 1):
                evenbyte = 0
                if pixeline[pl] == 1:
                    evenbyte = evenbyte | (0x3 << 6)
                if pixeline[pl + 2] == 1:
                    evenbyte = evenbyte | (0x3 << 4)
                if pixeline[pl + 4] == 1:
                    evenbyte = evenbyte | (0x3 << 2)
                if pixeline[pl + 6] == 1:
                    evenbyte = evenbyte | (0x3 << 0)

                pl += 8
                self.outbytes += '0x' + format(evenbyte, '02x') + ', '

            self.outbytes += '\n'

    def parse(self):
        if not self.inner_pbmlines[0].upper() == 'P1':
            return False

        m  = re.match(r'(?P<columns>\d+) (?P<rows>\d+)', self.inner_pbmlines[1])
        if m is None:
            return False

        self.columns = int(m.group('columns'))
        self.rows = int(m.group('rows'))
        self.pixeldata = [[0 for x in range(self.columns)] for y in range(self.rows)]

        iter_rows = 0

        filtered_text = [l.replace('\n', '').replace(' ', '').replace('\r', '') for l in self.inner_pbmlines[2:]]
        filtered_text = ''.join(filtered_text)

        for row_iter in range(0, self.rows):
            col_offset = row_iter * self.columns
            self.pixeldata[row_iter] = [int(i) for i in filtered_text[col_offset:col_offset + self.columns]]

        if not len(self.pixeldata) == self.rows:
            return False

        for r in self.pixeldata:
            if not len(r) == self.columns:
                return False

        return self.columns == 128 and self.rows == 96

    def __str__(self):
        s = ''
        for rw in self.pixeldata:
            for cl in rw:
                s += str(cl)
            s += '\n'

        return s

def filter_file_and_go(pbmfile_input_file, outid, outname):
    with open(pbmfile_input_file) as pbmfile:
        uncommented_lines = []

        for line in pbmfile:
            l = line.strip()
            if l.startswith('#'): continue
            uncommented_lines.append(l)

        return PBMFile(uncommented_lines, outid, outname)

def intro():
    print('Cumae Frame Generator\n')

if __name__ == '__main__':
    intro()

    arp = argparse.ArgumentParser(description='Generates frame data files for SBuCa starting from PBM files.')
    arp.add_argument('-in', nargs=1, help='file to parse', required=True, dest='infile')
    arp.add_argument('-out', nargs=1, help='file to generate')
    arp_args = arp.parse_args()

    outfilename = outid = "unknown"

    if arp_args.out:
        outid = os.path.splitext(os.path.basename(arp_args.out[0]))[0]
        outfilename = arp_args.out[0]
    else:
        outid = os.path.splitext(os.path.basename(arp_args.infile[0]))[0]
        outfilename = outid + '.h'

    bf = filter_file_and_go(arp_args.infile[0], outid, outfilename)
    if bf.parse() is False:
        sys.exit('%s does not seem to be a valid file' % sys.argv[1])

    bf.genframedata()
    bf.genframefile()

    print('Done.')
