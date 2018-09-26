CC=gcc
CFLAGS=-I.
DEPS = myshell.h
OBJ = myshell.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

myshell.out: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

debug: CFLAGS += -g
debug: myshell.out

.PHONY: clean

clean:
	rm *.o *.out
