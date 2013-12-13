#include "graph.h"
#include <list>
#include <iostream>
#include <cmath>
#include <fstream>
#include <ctime>
#include <chrono>
#include "testgraph.h"

#include "../benchmark_algos/boost/astar.h"

int main()
{
	// timing stuff
	std::ofstream timeout("time.out", std::ios::out);
	timeout << "graph" << "\t" << "threshold" << "n" << "\t" << "n*n" << "\t" << "#proc" << "\t" << "run" << "\t"
			<< "time [s]" << "\t" << "distance" << "time_lazy [s]" << "\t" << "distance_lazy" << std::endl;
	std::chrono::high_resolution_clock::time_point t_start, t_end;
	std::chrono::high_resolution_clock::time_point t_startl, t_endl;

	// initial graph stuff
	Testgraph tg;
	EuklidDistance distance;
	std::list<Node*> *path = new std::list<Node*>;
	double dist, dist2;

	int runcount = 5;
	int number_of_processor = 4;

	// benchmarking
	for (unsigned int n = 128; n <= 2048; n*=2)
	{
		Graph g;
	 	HolyGraph hg = HolyGraph(n);
// 		CircleGraph cg = CircleGraph(n / 2, n / 4); 
// 		SmileyGraph sg = SmileyGraph(n/2);

		tg.getGenericGraphFast(n, distance, hg, g);
		g.randomDisplaceAllNodes(0.3, distance);
// 		tg.removeRandomNodes(g, 2000, 10000);

		double exact_dist = boost_shortestPath_ek(g, 0, n*n-1);
		
		for (double t = 0.25; t <= 4; t*=2)
		{
			g.threshold = t;

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
					timeout << "1 " << "\t" << t << "\t"
							<< n << "\t" << n*n << "\t" << i << "\t" << j << "\t"
							<< time_span.count() << "\t" << dist <<  "\t"
							<< time_spanl.count() << "\t" << dist2 <<  std::endl;
				}
			}

			for (int j = 0; j < runcount; j++)
			{
				g.cleanup();

				t_start = std::chrono::high_resolution_clock::now();
				dist = g.getShortestPathSeq(0, n*n-1, path, distance);	// holy values / circle values
				t_end = std::chrono::high_resolution_clock::now();
				std::chrono::duration<double> time_span =
					std::chrono::duration_cast<std::chrono::duration<double>>(t_end - t_start);

				// end timing and write to file
				timeout << "1 " << "\t" << t << "\t"
						<< n << "\t" << n*n << "\t" << "-1" << "\t" << j << "\t"
						<< time_span.count() << "\t" << dist  << "\t"
						<< "\t" << exact_dist << std::endl;
			}
		}
	}
	// benchmarking
	for (unsigned int n = 128; n <= 2048; n*=2)
	{
		Graph g;
// 	 	HolyGraph hg = HolyGraph(n);
		CircleGraph cg = CircleGraph(n / 2, n / 4); 
// 		SmileyGraph sg = SmileyGraph(n/2);

		tg.getGenericGraphFast(n, distance, cg, g);
		g.randomDisplaceAllNodes(0.3, distance);
// 		tg.removeRandomNodes(g, 2000, 10000);

		double exact_dist = boost_shortestPath_ek(g, 0, n*n-1);
		
		for (double t = 0.25; t <= 4; t*=2)
		{
			for (int i = 1; i <= number_of_processor; i++)
			{
				g.threshold = t;
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
					timeout << "2 " << "\t" << t << "\t"
							<< n << "\t" << n*n << "\t" << i << "\t" << j << "\t"
							<< time_span.count() << "\t" << dist <<  "\t"
							<< time_spanl.count() << "\t" << dist2 <<  std::endl;
				}
			}
			for (int j = 0; j < runcount; j++)
			{
				g.cleanup();

				t_start = std::chrono::high_resolution_clock::now();
				dist = g.getShortestPathSeq(0, n*n-1, path, distance);	// holy values / circle values
				t_end = std::chrono::high_resolution_clock::now();
				std::chrono::duration<double> time_span =
					std::chrono::duration_cast<std::chrono::duration<double>>(t_end - t_start);

				// end timing and write to file
				timeout << "2 " << "\t" << t << "\t"
						<< n << "\t" << n*n << "\t" << "-1" << "\t" << j << "\t"
						<< time_span.count() << "\t" << dist  << "\t"
						<< "\t" << exact_dist << std::endl;
				
			}
		}
	}

	for (unsigned int r = 128; r <= 1024; r*=2)
	{
		for (int i = 1; i <= number_of_processor; i++)
		{
			int n = 1. * r * std::sqrt(i);

			Graph g;
	 	 	HolyGraph hg = HolyGraph(n);
// 			CircleGraph cg = CircleGraph(n / 2, n / 4); 
	// 		SmileyGraph sg = SmileyGraph(n/2);

			tg.getGenericGraphFast(n, distance, hg, g);
			g.randomDisplaceAllNodes(0.3, distance);
	// 		tg.removeRandomNodes(g, 2000, 10000);
		
			boost_shortestPath_ek(g, 0, n*n-1);

			for (double t = 0.25; t <= 4; t*=2)
			{
				g.threshold = t;
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
					timeout << "3 " << "\t" << t << "\t"
							<< n << "\t" << n*n << "\t" << i << "\t" << j << "\t"
							<< time_span.count() << "\t" << dist <<  "\t"
							<< time_spanl.count() << "\t" << dist2 <<  std::endl;
				}
			}
		}
	}

	for (unsigned int r = 128; r <= 1024; r*=2)
	{
		for (int i = 1; i <= number_of_processor; i++)
		{
			int n = r * std::sqrt(i);

			Graph g;
	// 	 	HolyGraph hg = HolyGraph(n);
			CircleGraph cg = CircleGraph(n / 2, n / 4); 
	// 		SmileyGraph sg = SmileyGraph(n/2);

			tg.getGenericGraphFast(n, distance, cg, g);
			g.randomDisplaceAllNodes(0.3, distance);
	// 		tg.removeRandomNodes(g, 2000, 10000);
		
			boost_shortestPath_ek(g, 0, n*n-1);

			for (double t = 0.25; t <= 4; t*=2)
			{
				g.threshold = t;
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
					timeout << "4 " << "\t" << t << "\t"
							<< n << "\t" << n*n << "\t" << i << "\t" << j << "\t"
							<< time_span.count() << "\t" << dist <<  "\t"
							<< time_spanl.count() << "\t" << dist2 <<  std::endl;
				}
			}
		}
	}

	delete path;

	return 0;
}

