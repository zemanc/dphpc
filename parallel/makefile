# Makefile for fringe search
CC = g++
CFLAGS = -Wall -pedantic -ansi -c -std=c++0x -fopenmp
LFLAGS = -Wall -fopenmp

# make
all: fringe
# make optimized
optimized: CC += -O1

optimized: fringe
# make profile
profile: CC += -O1 -pg
profile: fringe

graph_headers = graph.h graph.addEdges.h graph.getShortestPath.h graph.distances.h graph.randomDisplaceAllNodes.h 
testgraph_headers = testgraph.h testgraph.getGenericGraphFast.h testgraph.genericGraphs.h

fringe: main.o graph.o node.o testgraph.o testgraph.o 
	$(CC) $(LFLAGS) -o $@ $^
main.o: main.cpp $(graph_headers) $(testgraph_headers) node.h 
	$(CC) $(CFLAGS) $<
testgraph.o: testgraph.cpp $(testgraph_headers) $(graph_headers) node.h 
	$(CC) $(CFLAGS) $<
graph.o: graph.cpp $(graph_headers) node.h 
	$(CC) $(CFLAGS) $<
#astar-test.o: ../benchmark_algos/boost/astar-test.cpp ../benchmark_algos/boost/astar.h $(testgraph_headers) $(graph_headers) node.h
#	$(CC) $(CFLAGS) $<
node.o: node.cpp node.h 
	$(CC) $(CFLAGS) $<
#lock.o: lock.cpp lock.h
#	g++ $(CFLAGS) $<
clean: 
	$(RM) *.o
