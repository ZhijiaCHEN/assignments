vpath %.hpp rapidxml

all: xmltest.exe

xmltest.exe: main.cc
	g++ -Wall -g -o xmltest.exe -Irapidxml main.cc

clean:
	rm xmltest.exe

CFLAGS=-m64 
CXXFLAGS=-m64 
LDFLAGS=-m64

