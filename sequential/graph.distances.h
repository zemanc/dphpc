#ifndef __graph_distances_h__

#define __graph_distances_h__

#include <cmath>
#include "testgraph.h"

struct EuklidDistance {
	EuklidDistance() {};
	length_t operator()(Node* const pFrom, Node* const pTo) const
	{
		return std::sqrt((pTo->getX() - pFrom->getX()) * (pTo->getX() - pFrom->getX()) 
			            +(pTo->getY() - pFrom->getY()) * (pTo->getY() - pFrom->getY()));
	};

	static const Graph::edge_type edge_t = Graph::edge_type::dir8;
};

struct ManhattanDistance 
{
	ManhattanDistance() {};
	length_t operator()(Node* const pFrom, Node* const pTo) const
	{
		return std::abs(pTo->getX() - pFrom->getX()) + std::abs(pTo->getY() - pFrom->getY());
	};

	static const Graph::edge_type edge_t = Graph::edge_type::dir4;
};

#endif
