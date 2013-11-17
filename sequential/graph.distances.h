#ifndef __graph_distances_h__

#define __graph_distances_h__

#include <cmath>

struct EuklidDistance {
	EuklidDistance() {};
	length_t get(Node* const pFrom, Node* const pTo) const
	{
		return std::sqrt((pTo->getX() - pFrom->getX()) * (pTo->getX() - pFrom->getX()) 
			            +(pTo->getY() - pFrom->getY()) * (pTo->getY() - pFrom->getY()));
	};
};

struct ManhattanDistance 
{
	ManhattanDistance() {};
	length_t get(Node* const pFrom, Node* const pTo) const
	{
		return std::abs(pTo->getX() - pFrom->getX()) + std::abs(pTo->getY() - pFrom->getY());
	};
};

#endif
