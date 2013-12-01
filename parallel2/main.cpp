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
	// timing stuff
	std::ofstream timeout("time.out", std::ios::out);
	std::ofstream tbout("time_boost.out", std::ios::out);
	timeout << "n" << "\t" << "n*n" << "\t" << "i" << "\t"
			<< "time [s]" << "\t" << "distance" << std::endl;
	tbout << "time [s]" << "\t" << "distance" << std::endl;
	std::chrono::high_resolution_clock::time_point t_start, t_end;

	// initial graph stuff
	Testgraph tg;
	EuklidDistance distance;
	std::list<Node*> *path = new std::list<Node*>;
	double dist;

	// benchmarking
	for (unsigned int n = 3000; n <= 3000; n+=10)
	{
		Graph g;
// 	 	HolyGraph hg = HolyGraph(n);
		CircleGraph cg = CircleGraph(n / 2, n / 4); 
// 		SmileyGraph sg = SmileyGraph(n/2);

		tg.getGenericGraphFast(n, distance, cg, g);
// 		g.randomDisplaceAllNodes(0.3, distance);
// 		tg.removeRandomNodes(g, 2000, 10000);
		
		for (int i = 0; i < 8; i++)
		{
			g.cleanup();

			omp_set_dynamic(0);
			omp_set_num_threads(i);

			// start timing
			t_start = std::chrono::high_resolution_clock::now();

			dist = g.getShortestPath(0, n*n-1, path, distance);	// holy values / circle values
// 			dist = g.getShortestPath(n/4*n-6*n/7, 3*n/4*n-n/7, path, distance);	// smiley values

			// end timing and write to file
			t_end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> time_span =
				std::chrono::duration_cast<std::chrono::duration<double>>(t_end - t_start);
			timeout << n << "\t" << n*n << "\t" << i << "\t"
					<< time_span.count() << "\t" << dist << std::endl;

// 			boost_shortestPath_ek(g, 0, n*n-1);
//  			g.printGml();

		}
	}


	delete path;

	return 0;
}


