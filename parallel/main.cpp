#include "graph.h"
#include <list>
#include <iostream>
#include <fstream>
#include <ctime>
#include <chrono>
// #include <ratio>
#include "testgraph.h"

// #include "../benchmark_algos/boost/astar.h"

int main()
{

	// initial graph stuff
	Testgraph tg;
	EuklidDistance distance;
	std::list<Node*> *path = new std::list<Node*>;

	// benchmarking
	for (unsigned int n = 100; n <= 100; n+=10)
	{
		Graph g;
// 	 	HolyGraph hg = HolyGraph(n);
// 		CircleGraph cg = CircleGraph(n / 2, n / 4); 
		SmileyGraph sg = SmileyGraph(n/2);

		tg.getGenericGraphFast(n, distance, sg, g);
		g.randomDisplaceAllNodes(0.3, distance);
// 		tg.removeRandomNodes(g, 2000, 10000);
		
		for (int i = 4; i <= 4; i++)
		{
			omp_set_dynamic(0);
			omp_set_num_threads(i);

			g.cleanup();
			g.threshold = 10;
//  			g.getShortestPath(n/4*n-6*n/7, 3*n/4*n-n/7, path, distance, 3);	// smiley values
			g.cleanup();
			g.threshold = 1;
//  			g.getShortestPath(n/4*n-6*n/7, 3*n/4*n-n/7, path, distance, 2);	// smiley values
			g.cleanup();
			g.threshold = 0.1;
//  			g.getShortestPath(n/4*n-6*n/7, 3*n/4*n-n/7, path, distance, 1);	// smiley values
   			g.printGml();

		}
	}


	delete path;

	return 0;
}


