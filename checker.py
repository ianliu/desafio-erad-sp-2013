#!/usr/bin/python

import sys
from struct import pack

def usage():
    sys.stderr.write("Usage: %s -w WIDTH -h HEIGHT\n" % (sys.argv[0], ))
    sys.exit(1)

rows, cols = 0, 0

i = 1
while i < len(sys.argv):
    if sys.argv[i] == "-w":
        cols = int(sys.argv[i+1])
        i += 1
    elif sys.argv[i] == "-h":
        rows = int(sys.argv[i+1])
        i += 1
    i += 1

if rows == 0 or cols == 0:
    usage()

f = sys.stdout
f.write(pack('hh', cols, rows))

size = 100
color = [(chr(255), chr(  0), chr(  0), chr(255)),
         (chr(  0), chr(255), chr(  0), chr(255)),
         (chr(  0), chr(  0), chr(255), chr(255))]

for row in xrange(rows):
    for col in xrange(cols):
        c = (row/size + col/size) % 3
        f.write(pack('cccc', *color[c]))
