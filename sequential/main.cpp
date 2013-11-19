#include "graph.h"
#include <list>
#include <iostream>
#include <fstream>
#include <ctime>
#include <chrono>
// #include <ratio>
#include "testgraph.h"

#include "../benchmark_algos/boost/astar.h"

int main()
{

	// size
	unsigned int n = 100;

	EuklidDistance distance;
// 	CircleGraph cg = CircleGraph(n / 2, n / 4); 
	SmileyGraph sg = SmileyGraph(n/2);

	Testgraph tg;
// 	Graph g = tg.getSimpleGraph(100);
// 	Graph g = tg.getLukasOriginalGraph(100);
// 	Graph g = tg.getSmileyGraph(100, Testgraph::dir8);
	Graph g = tg.getGenericGraphFast(n, distance, sg);
// 	tg.removeRandomNodes(g, 2000, 10000);

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
	g.getShortestPath(n/4*n-6*n/7, 3*n/4*n-n/7, path, distance);	// smiley values
// 	g.getShortestPath(0, n*n-1, path, distance);	// circle values

	// end timing
	t_end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> time_span =
		std::chrono::duration_cast<std::chrono::duration<double>>(t_end - t_start);
	timeout << time_span.count() << " seconds" << std::endl;

  	g.printGml();

	boost_shortestPath_ek(g, 0, n*n-1);

	delete path;

	return 0;
}


