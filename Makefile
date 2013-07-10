PROGRAM = smooth

all: $(PROGRAM)

$(PROGRAM): main.c
	$(CC) $(CFLAGS) -Wall -O3 -o $@ $^ -lrt

run: $(PROGRAM) image.in
	./$(PROGRAM)

clean:
	@rm $(PROGRAM)

data: checker.py
	./checker.py -w 7680 -h 4320 > image.in
