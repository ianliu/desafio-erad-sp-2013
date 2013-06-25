#!/usr/bin/python

from PIL import Image
from struct import unpack
import sys

f = sys.stdin
cols, rows = unpack('hh', f.read(4))

for row in xrange(rows):
    length = cols * 4
    line = unpack(str(length) + 'c', f.read(length))
