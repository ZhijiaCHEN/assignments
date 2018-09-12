CC=gcc
CFLAGS=-I.
DEPS = matrixTemplate.h main.h
OBJ = main.o matrix0.o matrix1.o matrix2.o matrix3.o matrix4.o matrix5.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

os_lab1_part1.out: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

debug: CFLAGS += -g
debug: os_lab1_part1.out

.PHONY: clean

clean:
	rm *.o *.out
