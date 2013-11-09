# Makefile for fringe search
CC = g++
CFLAGS = -Wall -pedantic -ansi -c
LFLAGS = -Wall

# make
all: fringe
# make optimized
optimized: CC += -O3
optimized: fringe
# make profile
profile: CC += -pg
profile: optimized

fringe: main.o graph.o testgraph.o edge.o node.o
	$(CC) $(LFLAGS) -o $@ $^
main.o: main.cpp graph.h testgraph.h node.h edge.h
	$(CC) $(CFLAGS) $<
graph.o: graph.cpp graph.h testgraph.h node.h edge.h
	$(CC) $(CFLAGS) $<
testgraph.o: testgraph.cpp testgraph.h edge.h node.h
	$(CC) $(CFLAGS) $<
edge.o: edge.cpp edge.h node.h
	$(CC) $(CFLAGS) $<
node.o: node.cpp node.h
	$(CC) $(CFLAGS) $<
clean: 
	$(RM) *.o
