//
//=======================================================================
// Copyright (c) 2004 Kristopher Beevers
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
//


#include <boost/graph/astar_search.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/random.hpp>
#include <boost/random.hpp>
#include <boost/graph/graphviz.hpp>
#include <sys/time.h>
#include <vector>
#include <list>
#include <iostream>
#include <fstream>
#include <math.h>    // for sqrte
#include <ctime>
#include <chrono>

using namespace boost;
using namespace std;


// auxiliary types
struct location
{
  float y, x; // lat, long
};
typedef float cost;

template <class LocMap>
class city_writer {
public:
  city_writer(LocMap l, list<vertex> sp, float _minx, float _maxx,
              float _miny, float _maxy,
              unsigned int _ptx, unsigned int _pty)
    : loc(l), shortest_path(sp), minx(_minx), maxx(_maxx), miny(_miny),
      maxy(_maxy), ptx(_ptx), pty(_pty) {}
  template <class Vertex>
  void operator()(ostream& out, const Vertex& v) const {
    float px = 1 - (loc[v].x - minx) / (maxx - minx);
    float py = (loc[v].y - miny) / (maxy - miny);
    out << "[label=\" \", pos=\""
        << static_cast<unsigned int>(ptx * px) << ","
        << static_cast<unsigned int>(pty * py)
        << "\", fontsize=\"11\", fillcolor=\"" << getColor(v) << "\"]";
  }
private:
  LocMap loc;
  list<vertex> sp;
  float minx, maxx, miny, maxy;
  unsigned int ptx, pty;

};

template <class WeightMap>
class time_writer {
public:
  time_writer(WeightMap w) : wm(w) {}
  template <class Edge>
  void operator()(ostream &out, const Edge& e) const {
    out << "[label=\"" << wm[e] << "\", fontsize=\"11\"]";
  }
private:
  WeightMap wm;
};


// euclidean distance heuristic
template <class Graph, class CostType, class LocMap>
class distance_heuristic : public astar_heuristic<Graph, CostType>
{
public:
  typedef typename graph_traits<Graph>::vertex_descriptor Vertex;
  distance_heuristic(LocMap l, Vertex goal)
    : m_location(l), m_goal(goal) {}
  CostType operator()(Vertex u)
  {
    CostType dx = m_location[m_goal].x - m_location[u].x;
    CostType dy = m_location[m_goal].y - m_location[u].y;
    return ::sqrt(dx * dx + dy * dy);
  }
private:
  LocMap m_location;
  Vertex m_goal;
};


struct found_goal {}; // exception for termination

// visitor that terminates when we find the goal
template <class Vertex>
class astar_goal_visitor : public boost::default_astar_visitor
{
public:
  astar_goal_visitor(Vertex goal) : m_goal(goal) {}
  template <class Graph>
  void examine_vertex(Vertex u, Graph& g) {
    if(u == m_goal)
      throw found_goal();
  }
private:
  Vertex m_goal;
};


int main(int argc, char **argv)
{
  
  // specify some types
  typedef adjacency_list<listS, vecS, directedS, no_property,
    property<edge_weight_t, cost> > mygraph_t;
  typedef property_map<mygraph_t, edge_weight_t>::type WeightMap;
  typedef mygraph_t::vertex_descriptor vertex;
  typedef mygraph_t::edge_descriptor edge_descriptor;
  typedef mygraph_t::vertex_iterator vertex_iterator;
  typedef std::pair<int, int> edge;

/*	==============================================================
	START ÄNDERUNGEN CHRISTIAN
	(erstellt einfach einen n*n Graphen ohne Hindernisse)
	============================================================== */

	// vectors
	std::vector<location> locations_v;
	std::vector<edge> edge_array_v;

	const int n = 100;
	// no nodes in the middle (circle graph)
	const int cp = n/2;
	const int r = n/4;
	const int rr = r*r;
 
	// edges
	int e_no = 0;
	int it1 = 0;
	int it2 = 1;
	int it3 = n;
	for (int i = 1; i <= n; i++)
		for (int j = 1; j <= n; j++)
		{
			if (j < n)
			{
				if ( pow(i-1-cp, 2) + pow(j-1-cp, 2) >= rr
					&& pow(i-1-cp, 2) + pow(j-cp, 2) >= rr )
				{
// 					edge_array[e_no++] = edge(it1, it2);
// 					edge_array[e_no++] = edge(it2, it1);
					edge_array_v.push_back(edge(it1, it2));
					edge_array_v.push_back(edge(it2, it1));
				}
			}
			if (i < n)
			{
				if ( pow(i-1-cp, 2) + pow(j-1-cp, 2) >= rr
					&& pow(i-cp, 2) + pow(j-cp, 2) >= rr )
				{
// 					edge_array[e_no++] = edge(it1, it3);
// 					edge_array[e_no++] = edge(it3, it1);
					edge_array_v.push_back(edge(it1, it3));
					edge_array_v.push_back(edge(it3, it1));
				}
			}
			it1++;
			it2++;
			it3++;
		}

	// locations
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
		{	// only outside of circle
// 			if ( pow(i-cp, 2) + pow(j-cp, 2) >= rr )
				locations_v.push_back({(float)i, (float)j});
		}
	
	// define some constants
	const int num_edges = edge_array_v.size();
	const int num_nodes = locations_v.size();

	// copy values of vectors into arrays
	cost *weights = new cost[num_edges];
	location *locations = new location[num_nodes];
	edge *edge_array = new edge[num_edges];
	for (int i = 0; i < num_edges; i++)
	{
		weights[i] = 1;
		edge_array[i] = edge_array_v[i];
	}
	for (int i = 0; i < num_nodes; i++)
		locations[i] = locations_v[i];
 
  // specify data
//   enum nodes {
//     Troy, LakePlacid, Plattsburgh, Massena, Watertown, Utica,
//     Syracuse, Rochester, Buffalo, Ithaca, Binghamton, Woodstock,
//     NewYork, N
//   };
//   const char *name[] = {
//     "Troy", "Lake Placid", "Plattsburgh", "Massena",
//     "Watertown", "Utica", "Syracuse", "Rochester", "Buffalo",
//     "Ithaca", "Binghamton", "Woodstock", "New York"
//   };
//   location locations[] = { // lat/long
//     {42.73, 73.68}, {44.28, 73.99}, {44.70, 73.46},
//     {44.93, 74.89}, {43.97, 75.91}, {43.10, 75.23},
//     {43.04, 76.14}, {43.17, 77.61}, {42.89, 78.86},
//     {42.44, 76.50}, {42.10, 75.91}, {42.04, 74.11},
//     {40.67, 73.94}
//   };
//   edge edge_array[] = {
//     edge(Troy,Utica), edge(Troy,LakePlacid),
//     edge(Troy,Plattsburgh), edge(LakePlacid,Plattsburgh),
//     edge(Plattsburgh,Massena), edge(LakePlacid,Massena),
//     edge(Massena,Watertown), edge(Watertown,Utica),
//     edge(Watertown,Syracuse), edge(Utica,Syracuse),
//     edge(Syracuse,Rochester), edge(Rochester,Buffalo),
//     edge(Syracuse,Ithaca), edge(Ithaca,Binghamton),
//     edge(Ithaca,Rochester), edge(Binghamton,Troy),
//     edge(Binghamton,Woodstock), edge(Binghamton,NewYork),
//     edge(Syracuse,Binghamton), edge(Woodstock,Troy),
//     edge(Woodstock,NewYork)
//   };
//   unsigned int num_edges = sizeof(edge_array) / sizeof(edge);
//   cost weights[] = { // estimated travel time (mins)
//     96, 134, 143, 65, 115, 133, 117, 116, 74, 56,
//     84, 73, 69, 70, 116, 147, 173, 183, 74, 71, 124
//   };
  
/*	==============================================================
	ENDE ÄNDERUNGEN CHRISTIAN
	============================================================== */
  
  // create graph
  mygraph_t g(num_nodes);
  WeightMap weightmap = get(edge_weight, g);
  for(int j = 0; j < num_edges; ++j) {
    edge_descriptor e; bool inserted;
    tie(e, inserted) = add_edge(edge_array[j].first,
                                edge_array[j].second, g);
    weightmap[e] = weights[j];
  }
  
/*	==============================================================
	START ÄNDERUNGEN CHRISTIAN
	(erstellt einfach einen n*n Graphen ohne Hindernisse)
	============================================================== */
  
  // pick random start/goal
//   mt19937 gen(time(0));
//   vertex start = random_vertex(g, gen);
//   vertex goal = random_vertex(g, gen);


	// set own start an goal
	vertex start = 1;
	vertex goal = 2000;
  
	cout << "Start vertex: " << start << endl;
	cout << "Goal vertex: " << goal << endl;
  
//   ofstream dotfile;
//   dotfile.open("test-astar-cities.dot");
//   write_graphviz(dotfile, g,
//                  city_writer<const char **, location*>
//                   (name, locations, 73.46, 78.86, 40.67, 44.93,
//                    480, 400),
//                  time_writer<WeightMap>(weightmap));
	
	// timing
	std::chrono::high_resolution_clock::time_point t_start, t_end;  
  
  vector<mygraph_t::vertex_descriptor> p(num_vertices(g));
  vector<cost> d(num_vertices(g));
  try {
	// start timing
	t_start = std::chrono::high_resolution_clock::now();
    // call astar named parameter interface
    astar_search
      (g, start,
       distance_heuristic<mygraph_t, cost, location*>
        (locations, goal),
       predecessor_map(&p[0]).distance_map(&d[0]).
       visitor(astar_goal_visitor<vertex>(goal)));
  
  
  } catch(found_goal fg) { // found a path to the goal
	// end timing
	t_end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> time_span =
		std::chrono::duration_cast<std::chrono::duration<double>>(t_end - t_start);	
    list<vertex> shortest_path;
    for(vertex v = goal;; v = p[v]) {
      shortest_path.push_front(v);
      if(p[v] == v)
        break;
    }
    cout << "Shortest path from " << start << " to "
         << goal << ": ";
    list<vertex>::iterator spi = shortest_path.begin();
    cout << start;
    for(++spi; spi != shortest_path.end(); ++spi)
      cout << " -> " << *spi;
    cout << endl << "Total travel time: " << d[goal] << endl;
	cout << endl << "CPU time: " << time_span.count() << " seconds" << endl;

  ofstream dotfile;
  dotfile.open("test_graph.dot");
  write_graphviz(dotfile, g, city_writer<location*>(locations, shortest_path,0, 100, 0, 100, 400, 400));




    return 0;
  }

	// clean up
	delete [] locations;
	delete [] weights;
	delete [] edge_array;

/*	==============================================================
	ENDE ÄNDERUNGEN CHRISTIAN
	============================================================== */

  
  cout << "Didn't find a path from " << start << "to"
       << goal << "!" << endl;


  return 0;
  
}
