# Fast Average Filter

This project is part of a high performance computing contest held in São
Carlos, Brazil. The purpose of the program is to apply an average filter in a
4K resolution image as fast as possible.

## Compiling

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

## Testing the program

    $ cd build
    $ make run
    $ display image.ppm \# Use your favorite image viewer here!

## The image format

The image is written in little endian binary form. The first 2 bytes
represents the width of the image and the next 2 bytes represents the height.
After that pixels are written with 4 bytes, each one representing the red,
green, blue and alpha channel respectively. The pixels are written as rows
first, from top-left to right-bottom.

## Borders

We assume a border of zeroes.

## Window size

The window size is hard coded as a 5x5 matrix.

## Visualizing images

This software comes with a program called _convert_ that converts the image in
the raw format to a PPM image. The alpha channel is lost in this transition,
but suffices for opaque images.
