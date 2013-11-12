#include "edge.h"

Edge::Edge(Node* pFrom, Node* pTo) : from(pFrom)
								   , to(pTo)
{
	pFrom->addEdge(this);
	dist = pFrom->EuklidDistanceTo(pTo);
}

Edge::Edge(Node* pFrom, Node* pTo, length_t d) : from(pFrom)
											   , to(pTo)
											   , dist(d)
{
	pFrom->addEdge(this);
}

