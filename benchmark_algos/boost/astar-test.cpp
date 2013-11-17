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
#include <string>
#include <chrono>

using namespace boost;
using namespace std;

// auxiliary types
struct location
{
	float x;
	float y;
};

typedef float cost_t;

template <class LocMap, class ColorMap>
class node_writer {
	public:
		typedef adjacency_list<
			listS, 
			vecS, 
			directedS, 
			no_property,
			property<edge_weight_t, cost_t> 
		  >::vertex_descriptor vertex;

		node_writer(
			LocMap l, 
			list<vertex> shortest_path, 
			ColorMap cm,
			float _minx, 
			float _maxx,
			float _miny, 
			float _maxy,
			unsigned int _ptx, 
			unsigned int _pty
		  )
				: loc(l)
				, sp(shortest_path)
				, cmap(cm)
				, minx(_minx)
				, maxx(_maxx)
				, miny(_miny)
				, maxy(_maxy)
				, ptx(_ptx)
				, pty(_pty)
			{}

		template <class Vertex>
		void operator()(ostream& out, const Vertex& v) const 
		{
			float px = 1 - (loc[v].x - minx) / (maxx - minx);
			float py = (loc[v].y - miny) / (maxy - miny);
			out << "["
					<< "label=\" \", "
					<< "pos=\""
						<< static_cast<unsigned int>(ptx * px) << ","
						<< static_cast<unsigned int>(pty * py) << "\" ,"
					<< "fontsize=\"11\", "
					<< "fillcolor=\"" << getColor(v) << "\""
				<< "]";
		}

	private:
		LocMap loc;
		list<vertex> sp;
		ColorMap cmap;
		float minx, maxx, miny, maxy;
		unsigned int ptx, pty;

		template <class Vertex>
		string getColor(const Vertex& v) const
		{
			for (list<vertex>::const_iterator it = sp.begin(); it != sp.end(); it++)
				if ((loc[*it].x == loc[v].x) && (loc[*it].y == loc[v].y))
					return "#0000FF";

			if (cmap[v] == default_color_type::white_color )
				return "#EEEEEE";
			else if (cmap[v] == default_color_type::gray_color )
				return "#FF0000";
			else if (cmap[v] == default_color_type::black_color )
				return "#BBEEBB";
			else
				return "#00FFFF";
		}

};

template <class WeightMap>
class time_writer
{
	public:
		time_writer(WeightMap w) : wm(w) {}

		template <class Edge>
		void operator()(ostream &out, const Edge& e) const 
		{
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
			: m_location(l)
			, m_goal(goal) 
		  {}

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
	typedef adjacency_list<
		listS, 
		vecS, 
		directedS, 
		property<vertex_color_t, default_color_type>, 
		property<edge_weight_t, cost_t>
	  > mygraph_t;
	typedef property_map<mygraph_t, edge_weight_t>::type WeightMap;
	typedef property_map<mygraph_t, vertex_color_t>::type ColorMap;
	typedef mygraph_t::vertex_descriptor vertex;
	typedef mygraph_t::edge_descriptor edge_descriptor;
	typedef mygraph_t::vertex_iterator vertex_iterator;
	typedef std::pair<int, int> edge;

	// vectors
	std::vector<location> locations_v;
	std::vector<edge> edge_array_v;

	const int n = 100;
	// no nodes in the middle (circle graph)
	const int cp = n/2;
	const int r = n/4;
	const int rr = r*r;
 
	// edges
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
					edge_array_v.push_back(edge(it1, it2));
					edge_array_v.push_back(edge(it2, it1));
				}
			}
			if (i < n)
			{
				if ( pow(i-1-cp, 2) + pow(j-1-cp, 2) >= rr
					&& pow(i-cp, 2) + pow(j-cp, 2) >= rr )
				{
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
			locations_v.push_back({(float)i, (float)j});
	
	// define some constants
	const int num_edges = edge_array_v.size();
	const int num_nodes = locations_v.size();

	// copy values of vectors into arrays
	cost_t *weights = new cost_t[num_edges];
	location *locations = new location[num_nodes];
	edge *edge_array = new edge[num_edges];
	for (int i = 0; i < num_edges; i++)
	{
		weights[i] = 1;
		edge_array[i] = edge_array_v[i];
	}
	for (int i = 0; i < num_nodes; i++)
		locations[i] = locations_v[i];
 
	// create graph
	mygraph_t g(num_nodes);
	WeightMap weightmap = get(edge_weight, g);
	ColorMap cmap = get(vertex_color, g);

	for(int j = 0; j < num_edges; ++j) 
	{
		edge_descriptor e; bool inserted;
		tie(e, inserted) = add_edge(edge_array[j].first,
									edge_array[j].second, 
									g);

		weightmap[e] = weights[j];
	}
  
// 	pick random start/goal
//	mt19937 gen(time(0));
// 	vertex start = random_vertex(g, gen);
// 	vertex goal = random_vertex(g, gen);

	// set own start an goal
	vertex start = 9899;
	vertex goal = 504;
  
	cout << "Start vertex: " << start << endl;
	cout << "Goal vertex: " << goal << endl;
  
	// timing
	std::chrono::high_resolution_clock::time_point t_start, t_end;  
  
	vector<mygraph_t::vertex_descriptor> p(num_vertices(g));
	vector<cost_t> d(num_vertices(g));

	try {
		// start timing
		t_start = std::chrono::high_resolution_clock::now();

		// call astar named parameter interface
		astar_search(
			g, 
			start,
			distance_heuristic<mygraph_t, cost_t, location*>
				(locations, 
				goal),
			predecessor_map(
				&p[0]).distance_map(&d[0])
				.visitor(astar_goal_visitor<vertex>(goal))
				.color_map(cmap)
		);


	} catch(found_goal fg) { // found a path to the goal
		// end timing
		t_end = std::chrono::high_resolution_clock::now();

		std::chrono::duration<double> time_span =
			std::chrono::duration_cast<std::chrono::duration<double>>(t_end - t_start);	

		list<vertex> shortest_path;

		for(vertex v = goal;; v = p[v]) 
		{
			shortest_path.push_front(v);

			if (p[v] == v)
				break;

		}

		cout << "Shortest path from " << start << " to " << goal << ": ";

		list<vertex>::iterator spi = shortest_path.begin();
		cout << start;

		for(++spi; spi != shortest_path.end(); ++spi)
			cout << " -> " << *spi;

		cout << endl << "Total travel time: " << d[goal] << endl;
		cout << endl << "CPU time: " << time_span.count() << " seconds" << endl;

		ofstream dotfile;
		dotfile.open("test_graph.dot");
		write_graphviz(dotfile, g, node_writer<location*, ColorMap>
					 (locations, 
					  shortest_path, 
					  cmap,
					  0,
					  100,
					  0, 
					  100,
					  400,
					  400));

		return 0;
	}

	// clean up
	delete [] locations;
	delete [] weights;
	delete [] edge_array;
  
	cout << "Didn't find a path from " << start << "to" << goal << "!" << endl;

	return 0;
  
}
