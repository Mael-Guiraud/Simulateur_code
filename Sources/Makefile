
OS := $(shell uname)
ifeq ($(OS),Darwin)
  CC = gcc-7
else
  CC = gcc
endif


CFLAGS = -c -g -std=c99 -Wall -O3

DIRbin = bin/
DIRdata = ../datas/

all: create_folder run

run : prog 
	./prog

debug : prog 
	valgrind ./prog

prog: $(DIRbin)main.o $(DIRbin)message_generation.o $(DIRbin)simulator.o
	$(CC) -o prog $(DIRbin)main.o $(DIRbin)message_generation.o $(DIRbin)simulator.o

$(DIRbin)main.o: main.c struct.h
	$(CC) $(CFLAGS) main.c -o $(DIRbin)main.o 

$(DIRbin)message_generation.o: message_generation.c struct.h
	$(CC) $(CFLAGS) message_generation.c -o $(DIRbin)message_generation.o 

$(DIRbin)simulator.o: simulator.c struct.h
	$(CC) $(CFLAGS) simulator.c -o $(DIRbin)simulator.o 

create_folder:
	mkdir -p $(DIRbin)
	mkdir -p $(DIRdata)

clean:
	rm -f $(DIRbin)*