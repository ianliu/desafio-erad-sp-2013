PROGRAM = smooth

CFLAGS ?= -Wall -O3
CPPFLAGS ?=
LIBS ?= -lrt
LDFLAGS ?= -static

all: $(PROGRAM)

$(PROGRAM): main.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

run: $(PROGRAM) image.in
	./$(PROGRAM)

clean:
	rm -f $(PROGRAM) image.out

data: checker.py
	./checker.py -w 7680 -h 4320 > image.in

perf: $(PROGRAM)
	perf stat -r10 -e instructions,cache-references,cache-misses,L1-dcache-loads,L1-dcache-load-misses ./smooth
