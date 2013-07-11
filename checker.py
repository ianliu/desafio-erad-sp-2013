#!/usr/bin/python2.7

import sys
from struct import pack

def usage():
    sys.stderr.write("Usage: %s -w WIDTH -h HEIGHT [-s SIZE]\n" % \
            (sys.argv[0], ))
    sys.stderr.write("      -w  WIDTH  Image width\n")
    sys.stderr.write("      -h  HEIGHT Image height\n")
    sys.stderr.write("      -s  SIZE   Size of the checkers (default 100)\n")
    sys.exit(1)

height, width = 0, 0
size = 100

i = 1
while i < len(sys.argv):
    if sys.argv[i] == "-w":
        width = int(sys.argv[i+1])
        i += 1
    elif sys.argv[i] == "-h":
        height = int(sys.argv[i+1])
        i += 1
    elif sys.argv[i] == "-s":
        size = int(sys.argv[i+1])
        i += 1
    i += 1

if height == 0 or width == 0:
    usage()

f = sys.stdout
f.write(pack('hh', width, height))

color = [(chr(255), chr(  0), chr(  0), chr(255)),
         (chr(  0), chr(255), chr(  0), chr(255)),
         (chr(  0), chr(  0), chr(255), chr(255))]

for row in xrange(height):
    for col in xrange(width):
        c = (row/size + col/size) % 3
        f.write(pack('cccc', *color[c]))
