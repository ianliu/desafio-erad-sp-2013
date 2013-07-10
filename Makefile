PROGRAM = smooth

all: $(PROGRAM)

$(PROGRAM): main.c
	$(CC) -Wall -O3 -o $@ $^

run: $(PROGRAM)
	./$(PROGRAM)

clean:
	@rm $(PROGRAM)
