#!/usr/bin/python

from PIL import Image
from struct import unpack
import sys


f = sys.stdin
cols, rows = unpack('hh', f.read(4))
img = Image.new('RGBA', (cols, rows))

print cols, rows

for row in xrange(rows):
    length = cols * 4
    line = unpack(str(length) + 'c', f.read(length))
    for col in xrange(cols):
        color = tuple(map(ord, line[col*4:(col+1)*4]))
        img.putpixel((col, row), color)

img.save("image.png")
# img.show()
