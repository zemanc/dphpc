//
//=======================================================================
// Copyright (c) 2004 Kristopher Beevers
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
//

#include "astar.h"

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
#include <cmath>
#include <ctime>
#include <string>
#include <chrono>

using namespace boost;
using namespace std;

// auxiliary types
struct location
{
	length_t x;
	length_t y;
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
			length_t _minx, 
			length_t _maxx,
			length_t _miny, 
			length_t _maxy,
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
			length_t px = 1 - (loc[v].x - minx) / (maxx - minx);
			length_t py = (loc[v].y - miny) / (maxy - miny);
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
		length_t minx, maxx, miny, maxy;
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


// manhattan distance heuristic
template <class Graph, class CostType, class LocMap>
class manhattan_distance : public astar_heuristic<Graph, CostType>
{
	public:
		typedef typename graph_traits<Graph>::vertex_descriptor Vertex;

		manhattan_distance(LocMap l, Vertex goal)
			: m_location(l)
			, m_goal(goal) 
		  {}

		CostType operator()(Vertex u)
		{
			CostType dx = abs(m_location[m_goal].x - m_location[u].x);
			CostType dy = abs(m_location[m_goal].y - m_location[u].y);
			return dx + dy;
		}

	private:
		LocMap m_location;
		Vertex m_goal;
};

// euclidean distance heuristic
template <class Graph, class CostType, class LocMap>
class euklid_distance : public astar_heuristic<Graph, CostType>
{
	public:
		typedef typename graph_traits<Graph>::vertex_descriptor Vertex;

		euklid_distance(LocMap l, Vertex goal)
			: m_location(l)
			, m_goal(goal) 
		  {}

		CostType operator()(Vertex u)
		{
			CostType dx = m_location[m_goal].x - m_location[u].x;
			CostType dy = m_location[m_goal].y - m_location[u].y;
			return std::sqrt(dx * dx + dy * dy);
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


void boost_shortestPath_ek(Graph& my_g, unsigned int start_n, unsigned int end_n)
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

	// edges
	for (pEdg_v_it it = my_g.pEdges_v.begin(); it != my_g.pEdges_v.end(); it++)
		edge_array_v.push_back(edge((*it)->getFrom()->getIndex(), (*it)->getTo()->getIndex()));

	// locations
	for (pNode_v_it it = my_g.pNodes_v.begin(); it != my_g.pNodes_v.end(); it++)
		locations_v.push_back({(*it)->getX(), (*it)->getY()});
	
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
	vertex start = start_n;
	vertex goal = end_n;
  	std::ofstream timeout("time_boost.out", std::ios::out);


	timeout << "Start vertex: " << start << endl;
	timeout << "Goal vertex: " << goal << endl;
  
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
			euklid_distance<mygraph_t, cost_t, location*>
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

		timeout << "Shortest path from " << start << " to " << goal << ": ";

		list<vertex>::iterator spi = shortest_path.begin();
		timeout << start;

		for(++spi; spi != shortest_path.end(); ++spi)
			timeout << " -> " << *spi;

		timeout << endl << "Total travel time: " << d[goal] << endl;
		timeout << endl << "CPU time: " << time_span.count() << " seconds" << endl;

		ofstream dotfile;
		dotfile.open("output_boost.dot");
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

		return;

	}

	// clean up
	delete [] locations;
	delete [] weights;
	delete [] edge_array;
  
	timeout << "Didn't find a path from " << start << "to" << goal << "!" << endl;

}
