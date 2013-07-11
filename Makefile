PROGRAM = smooth

CFLAGS ?= -Wall -O3
CPPFLAGS ?=
LIBS ?= -lrt

all: $(PROGRAM)

$(PROGRAM): main.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^ $(LIBS)

run: $(PROGRAM) image.in
	./$(PROGRAM)

clean:
	rm -f $(PROGRAM) image.out

data: checker.py
	./checker.py -w 7680 -h 4320 > image.in
