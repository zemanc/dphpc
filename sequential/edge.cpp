#include "edge.h"

Edge::Edge(Node* pFrom, Node* pTo) : from(pFrom)
								   , to(pTo)
{
	pFrom->addEdge(this);
	dist = pFrom->HeurDistanceTo(pTo);
}

Edge::Edge(Node* pFrom, Node* pTo, length_t d) : from(pFrom)
											   , to(pTo)
											   , dist(d)
{
	pFrom->addEdge(this);
}

Node* Edge::getFrom() const
{
	return from;
}

Node* Edge::getTo() const
{
	return to;
}

length_t Edge::getDistance() const
{
	return dist;
}
