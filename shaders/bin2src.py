#!/usr/bin/python3

import sys

with open(sys.argv[1], 'rb') as inFile:
  data = list(inFile.read())

with open('out.txt', 'w') as outFile:
  outFile.write('SQEE_API extern const uint8_t VARNAME[%d];\n' % len(data))
  outFile.write('const uint8_t VARNAME[%d] = {%s};\n' % (len(data), ','.join(map(str,data))))
