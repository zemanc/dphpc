#include "graph.h"
#include <list>
#include <iostream>
#include <cmath>
#include <fstream>
#include <ctime>
#include <chrono>
#include "testgraph.h"

/* zweite Ausgabespalte (erste Spalte ist wie immer Typ 1 Holy 2 Circle)
   1 TAS_EXP_LOCK
   2 TAS_LOCK
   3 TATAS_LOCK
   4 OMP_LOCKS
	LOCK ANALYSE MIT THRESHOLD 1 */

int main()
{
	// timing stuff
	std::ofstream timeout("time_locks4.out", std::ios::app);
	std::chrono::high_resolution_clock::time_point t_start, t_end;
	std::chrono::high_resolution_clock::time_point t_startl, t_endl;

	// initial graph stuff
	Testgraph tg;
	EuklidDistance distance;
	std::list<Node*> *path = new std::list<Node*>;
	double dist, dist2;

	int runcount = 50;
	int number_of_processor = 8;

	// benchmarking
	for (unsigned int n = 128; n <= 4096; n*=2)
	{
		Graph g;
	 	HolyGraph hg = HolyGraph(n);
// 		CircleGraph cg = CircleGraph(n / 2, n / 4); 
// 		SmileyGraph sg = SmileyGraph(n/2);

		tg.getGenericGraphFast(n, distance, hg, g);
		g.randomDisplaceAllNodes(0.3, distance);
// 		tg.removeRandomNodes(g, 2000, 10000);

		g.threshold = 1;

		for (int i = 1; i <= number_of_processor; i++)
		{
			for (int j = 0; j < runcount; j++)
			{
				g.cleanup();

				omp_set_dynamic(0);
				omp_set_num_threads(i);

				// start timing
				t_start = std::chrono::high_resolution_clock::now();
				dist = g.getShortestPath(0, n*n-1, path, distance);	// holy values / circle values
				t_end = std::chrono::high_resolution_clock::now();
				std::chrono::duration<double> time_span =
					std::chrono::duration_cast<std::chrono::duration<double>>(t_end - t_start);

				g.cleanup();

				t_startl = std::chrono::high_resolution_clock::now();
				dist2 = g.getShortestPathLazy(0, n*n-1, path, distance);	// holy values / circle values
				t_endl = std::chrono::high_resolution_clock::now();
				std::chrono::duration<double> time_spanl =
					std::chrono::duration_cast<std::chrono::duration<double>>(t_endl - t_startl);

				// end timing and write to file
				timeout << "1 1" << "\t" << 1 << "\t"
						<< n << "\t" << n*n << "\t" << i << "\t" << j << "\t"
						<< time_span.count() << "\t" << dist <<  "\t"
						<< time_spanl.count() << "\t" << dist2 <<  std::endl;
			}
		}
	}

	for (unsigned int n = 128; n <= 4096; n*=2)
	{
		Graph g;
// 	 	HolyGraph hg = HolyGraph(n);
		CircleGraph cg = CircleGraph(n / 2, n / 4); 
// 		SmileyGraph sg = SmileyGraph(n/2);

		tg.getGenericGraphFast(n, distance, cg, g);
		g.randomDisplaceAllNodes(0.3, distance);
// 		tg.removeRandomNodes(g, 2000, 10000);
		
		g.threshold = 1;

		for (int i = 1; i <= number_of_processor; i++)
		{
			for (int j = 0; j < runcount; j++)
			{
				g.cleanup();

				omp_set_dynamic(0);
				omp_set_num_threads(i);

				// start timing
				t_start = std::chrono::high_resolution_clock::now();
				dist = g.getShortestPath(0, n*n-1, path, distance);	// holy values / circle values
				t_end = std::chrono::high_resolution_clock::now();
				std::chrono::duration<double> time_span =
					std::chrono::duration_cast<std::chrono::duration<double>>(t_end - t_start);

				g.cleanup();

				t_startl = std::chrono::high_resolution_clock::now();
				dist2 = g.getShortestPathLazy(0, n*n-1, path, distance);	// holy values / circle values
				t_endl = std::chrono::high_resolution_clock::now();
				std::chrono::duration<double> time_spanl =
					std::chrono::duration_cast<std::chrono::duration<double>>(t_endl - t_startl);

				// end timing and write to file
				timeout << "2 1 " << "\t" << 1 << "\t"
						<< n << "\t" << n*n << "\t" << i << "\t" << j << "\t"
						<< time_span.count() << "\t" << dist <<  "\t"
						<< time_spanl.count() << "\t" << dist2 <<  std::endl;
			}
		}
	}

	delete path;

	return 0;
}

