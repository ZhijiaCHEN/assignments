CC=gcc

OBJ = main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

os_lab1_part1.out: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

debug: CFLAGS += -g
debug: os_lab1_part1.out

.PHONY: clean

clean:
	rm *.o *.out
