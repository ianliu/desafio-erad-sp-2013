#!/usr/bin/python2.7

from PIL import Image
from struct import unpack
import sys


f = sys.stdin
width, height = unpack('hh', f.read(4))
img = Image.new('RGBA', (width, height))

print width, height

for row in xrange(height):
    length = width * 4
    line = unpack(str(length) + 'c', f.read(length))
    for col in xrange(width):
        color = tuple(map(ord, line[col*4:(col+1)*4]))
        img.putpixel((col, row), color)

img.save("image.png")
# img.show()
