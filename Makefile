PROGRAM = smooth

all: $(PROGRAM)

$(PROGRAM): main.c
	$(CC) $(CFLAGS) -Wall -O3 -o $@ $^ -lrt

run: $(PROGRAM)
	./$(PROGRAM)

clean:
	@rm $(PROGRAM)
