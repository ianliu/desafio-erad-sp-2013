PROGRAM = smooth

CC ?= clang
CFLAGS ?= -Wall -O3
CPPFLAGS ?= -DDEBUG
LIBS ?= -lpthread
LDFLAGS ?= -static

all: $(PROGRAM)

$(PROGRAM): main.c
	@$(CC) --version | head -n1 > compiler.ver
	@echo $(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) > build.cmd
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

run: $(PROGRAM) image.in
	./$(PROGRAM)

clean:
	rm -f $(PROGRAM) image.out build.cmd compiler.ver

image.in: checker.py
	./checker.py -w 7680 -h 4320 > image.in

perf: $(PROGRAM)
	@echo "host:    `hostname`"
	@echo "cc:      `cat compiler.ver`"
	@echo "build:   `cat build.cmd`"
	@echo "version: `git rev-parse HEAD 2>/dev/null`"
	@echo "-------------------------------------------------"
	perf stat -r5 -e instructions,cache-references,cache-misses,L1-dcache-loads,L1-dcache-load-misses ./$(PROGRAM)

dist:
	rm -rf serial-killers serial-killers.tar.gz \
	  && mkdir serial-killers \
	  && cp Makefile main.c AUTHORS COPYING serial-killers \
	  && tar czf serial-killers.tar.gz serial-killers
