
OS := $(shell uname)
ifeq ($(OS),Darwin)
  CC = gcc-7
  PROFILING = qcachegrind
else
  CC = gcc
  PROFILING = kcachegrind
endif


CFLAGS = -c -lm -g -std=c99 -Wall -O2

DIRbin = bin/
DIRdata = ../datas/

all: create_folder run gnuplot

run : prog 
	./prog

gnuplot:
	gnuplot ../gnuplot/distribs.gplt
	gnuplot ../gnuplot/train.gplt

debug : prog 
	valgrind ./prog

profile: prog
	valgrind --tool=callgrind --dump-instr=yes --simulate-cache=yes --collect-jumps=yes ./prog 
	$(PROFILING) 

prog: $(DIRbin)main.o $(DIRbin)message_generation.o $(DIRbin)simulator.o $(DIRbin)data_treatment.o $(DIRbin)bemodel.o
	$(CC) -lm -o prog $(DIRbin)main.o $(DIRbin)message_generation.o $(DIRbin)simulator.o $(DIRbin)data_treatment.o $(DIRbin)bemodel.o

$(DIRbin)main.o: main.c struct.h
	$(CC) $(CFLAGS) main.c -o $(DIRbin)main.o 

$(DIRbin)message_generation.o: message_generation.c struct.h
	$(CC) $(CFLAGS) message_generation.c -o $(DIRbin)message_generation.o 

$(DIRbin)simulator.o: simulator.c struct.h
	$(CC) $(CFLAGS) simulator.c -o $(DIRbin)simulator.o 

$(DIRbin)data_treatment.o: data_treatment.c
	$(CC) $(CFLAGS) data_treatment.c -o $(DIRbin)data_treatment.o 

$(DIRbin)bemodel.o: bemodel.c
	$(CC) $(CFLAGS) bemodel.c -o $(DIRbin)bemodel.o 

create_folder:
	mkdir -p $(DIRbin)
	mkdir -p $(DIRdata)

clean:
	rm -f $(DIRbin)*