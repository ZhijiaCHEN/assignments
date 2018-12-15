CFLAGS = -O3
OBJS =-L../../mysql-connector/lib -lmysqlclient
INC =-I../../mysql-connector/include 

all: touch bestOrder
bestOrder : bestOrder.c
	cc $(CFLAGS) $(INC) -DN=$(SIZE) -o bestOrder bestOrder.c $(OBJS)
touch : 
	touch *.c
