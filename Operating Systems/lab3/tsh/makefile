SYS_LINK=-L/usr/lib
INCS = -I../include
OBJS = ../obj 
FLAGS = -g
LIBS = -lpthread -lsng
cc = gcc

all : tsh tshtest 

tsh : tsh.c 
	$(cc) $(EXTRA) $(INCS) $(FLAGS) -o tsh.out tsh.c myshell.c -L$(OBJS) $(LIBS)

tshtest : tshtest.c 
	$(cc) $(EXTRA) $(INCS) $(FLAGS) -o tshtest.out tshtest.c myshell.c -L$(OBJS) $(LIBS)

