#include "graph.h"
#include <list>
#include <iostream>
#include <fstream>
#include <ctime>
#include <chrono>
// #include <ratio>
#include "testgraph.h"

int main()
{

	Testgraph tg;
// 	Graph g = tg.getSimpleGraph(100);
// 	Graph g = tg.getLukasOriginalGraph(100);
	Graph g = tg.getSmileyGraph(100, Testgraph::dir8);
	tg.removeRandomNodes(g, 2000, 10000);

	std::list<Node*> *path = new std::list<Node*>;
// 	EuklidDistance ek_distance;
 	ManhattanDistance ek_distance;

	// start timing
	std::ofstream timeout("time.out", std::ios::out);
	std::chrono::high_resolution_clock::time_point t_start, t_end;
	t_start = std::chrono::high_resolution_clock::now();

// 	g.getShortestPath(1, 5549, path);
	g.getShortestPath(2686, 2481, path, ek_distance);	// smiley values

	// end timing
	t_end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> time_span =
		std::chrono::duration_cast<std::chrono::duration<double>>(t_end - t_start);
	timeout << time_span.count() << " seconds" << std::endl;

  	g.printGml();

	delete path;

	return 0;
}


