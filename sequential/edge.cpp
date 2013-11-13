#include "edge.h"



Edge::Edge(Node* pFrom, Node* pTo, length_t d) : from(pFrom)
											   , to(pTo)
											   , dist(d)
{
	pFrom->addEdge(this);
}

