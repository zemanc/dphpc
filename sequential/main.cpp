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

	// size
	unsigned int n = 1000;

	Testgraph tg;
// 	Graph g = tg.getSimpleGraph(100);
// 	Graph g = tg.getLukasOriginalGraph(100);
// 	Graph g = tg.getSmileyGraph(100, Testgraph::dir8);
	Graph g = tg.getCircleGraphFast(n);
// 	tg.removeRandomNodes(g, 2000, 10000);
	ManhattanDistance ek_distance;

// 	g.randomDisplaceAllNodes(0.1, ek_distance);
// 	g.randomDisplaceAllNodes(0.4, ek_distance);

	std::list<Node*> *path = new std::list<Node*>;
//  	ManhattanDistance ek_distance;

	// start timing
	std::ofstream timeout("time.out", std::ios::out);
	std::chrono::high_resolution_clock::time_point t_start, t_end;
	t_start = std::chrono::high_resolution_clock::now();

// 	g.getShortestPath(1, 5549, path);
// 	g.getShortestPath(2686, 2481, path, ek_distance);	// smiley values
// 	g.getShortestPath(1, 5000, path, ek_distance);	// smiley values
// 	g.getShortestPath(1446, 20, path, ek_distance);	// smiley values
	g.getShortestPath(0, n*n-1, path, ek_distance);	// circle values

	// end timing
	t_end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> time_span =
		std::chrono::duration_cast<std::chrono::duration<double>>(t_end - t_start);
	timeout << time_span.count() << " seconds" << std::endl;

  	g.printGml();

	g.save("out.graph");

	delete path;

	return 0;
}


