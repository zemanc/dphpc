# Makefile for fringe search
CC = g++
CFLAGS = -Wall -pedantic -ansi -c -std=c++0x
LFLAGS = -Wall

# make
all: fringe
# make optimized
optimized: CC += -O3
optimized: fringe
# make profile
profile: CC += -pg
profile: fringe

fringe: main.o graph.o testgraph.o edge.o node.o astar-test2.o
	$(CC) $(LFLAGS) -o $@ $^
main.o: main.cpp graph.h testgraph.h node.h edge.h testgraph.h graph.h testgraph.genericGraphs.h testgraph.getGenericGraphFast.h ../benchmark_algos/boost/astar.h
	$(CC) $(CFLAGS) $<
testgraph.o: testgraph.cpp testgraph.h graph.h edge.h node.h testgraph.genericGraphs.h testgraph.getGenericGraphFast.h
	$(CC) $(CFLAGS) $<
graph.o: graph.cpp graph.h node.h edge.h graph.distances.h graph.getShortestPath.h
	$(CC) $(CFLAGS) $<
edge.o: edge.cpp edge.h node.h
	$(CC) $(CFLAGS) $<
astar-test2.o: ../benchmark_algos/boost/astar-test2.cpp ../benchmark_algos/boost/astar.h testgraph.h
	$(CC) $(CFLAGS) $<
node.o: node.cpp node.h
	$(CC) $(CFLAGS) $<
clean: 
	$(RM) *.o
