
OS := $(shell uname)
ifeq ($(OS),Darwin)
  CC = gcc-7
  PROFILING = qcachegrind
else
  CC = gcc
  PROFILING = kcachegrind
endif


CFLAGS = -c -lm -g -std=c99 -Wall -O3 

DIRbin = bin/
DIRdata = ../datas/

all: create_folder run short_distrib gnuplot

run : prog 
	./prog

gnuplot:
	gnuplot ../gnuplot/distribs_short.gplt

debug : prog 
	valgrind ./prog

short_distrib: compil_short_distrib
	./short_distrib

compil_short_distrib: distribution.c
	gcc distribution.c -o short_distrib

profile: prog
	valgrind --tool=callgrind --dump-instr=yes --simulate-cache=yes --collect-jumps=yes ./prog 
	$(PROFILING) 

prog: $(DIRbin)main.o $(DIRbin)message_generation.o $(DIRbin)simulator.o
	$(CC) -lm -o prog $(DIRbin)main.o $(DIRbin)message_generation.o $(DIRbin)simulator.o

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