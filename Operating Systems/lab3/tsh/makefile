SYS_LINK=-L/usr/lib
INCS = -I../include
OBJS = ../obj 
FLAGS = -g 
cc = gcc

all : tsh tshtest 

tsh : tsh.c 
	$(cc) $(EXTRA) $(INCS) $(FLAGS) -o tsh.out tsh.c myshell.c -L$(OBJS) -lsng -lreadline

tshtest : tshtest.c 
	$(cc) $(EXTRA) $(INCS) $(FLAGS) -o tshtest.out tshtest.c -L$(OBJS) -lsng -lreadline
