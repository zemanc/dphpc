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
	// timing stuff
	std::ofstream timeout("time.out", std::ios::out);
	timeout << "n" << "\t" << "n*n" << "\t" << "i" << "\t"
			<< "time [s]" << "\t" << "distance" << std::endl;
	std::chrono::high_resolution_clock::time_point t_start, t_end;

	// initial graph stuff
	EuklidDistance distance;
	Testgraph tg;
	std::list<Node*> *path = new std::list<Node*>;
	double dist;

	// benchmarking
	for (unsigned int n = 10; n <= 100; n+=10)
	{
		for (int i = 0; i < 10; i++)
		{
			Graph g;

// 			CircleGraph cg = CircleGraph(n / 2, n / 4); 
// 			SmileyGraph sg = SmileyGraph(n/2);
	 		HolyGraph hg = HolyGraph(n);
		
			tg.getGenericGraphFast(n, distance, hg, g);
			g.randomDisplaceAllNodes(0.3, distance);


			// start timing
			t_start = std::chrono::high_resolution_clock::now();

			dist = g.getShortestPath(0, n*n-1, path, distance);	// holy values
// 			double dist = g.getShortestPath(n/4*n-6*n/7, 3*n/4*n-n/7, path, distance);	// smiley values
// 			double dist = g.getShortestPath(0, n*n-1, path, distance);	// circle values

			// end timing and write to file
			t_end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> time_span =
				std::chrono::duration_cast<std::chrono::duration<double>>(t_end - t_start);
			timeout << n << "\t" << n*n << "\t" << i << "\t"
					<< time_span.count() << "\t" << dist << std::endl;

//		  	g.printGml();

// 			boost_shortestPath_ek(g, 0, n*n-1);
		}
	}

	delete path;

	return 0;
}


