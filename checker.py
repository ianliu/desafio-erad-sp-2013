#!/usr/bin/python2.7

import sys
from struct import pack

def usage():
    sys.stderr.write("Usage: %s -w WIDTH -h HEIGHT\n" % (sys.argv[0], ))
    sys.exit(1)

height, width = 0, 0

i = 1
while i < len(sys.argv):
    if sys.argv[i] == "-w":
        width = int(sys.argv[i+1])
        i += 1
    elif sys.argv[i] == "-h":
        height = int(sys.argv[i+1])
        i += 1
    i += 1

if height == 0 or width == 0:
    usage()

f = sys.stdout
f.write(pack('hh', width, height))

size = 100
color = [(chr(255), chr(  0), chr(  0), chr(255)),
         (chr(  0), chr(255), chr(  0), chr(255)),
         (chr(  0), chr(  0), chr(255), chr(255))]

for row in xrange(height):
    for col in xrange(width):
        c = (row/size + col/size) % 3
        f.write(pack('cccc', *color[c]))
