#include "edge.h"
#include <climits>
#include <iostream>

unsigned int Node::max_index = 0;

Node::Node(length_t xP, length_t yP) : xPos(xP)
									 , yPos(yP)
									 , parent(0)
									 , status(inactive)
									 , f(0)
									 , g(0)
									 , h(0)
{
	index = max_index++;
}
Node::~Node()
{
//	max_index--; das wäre ja gar nicht gut! :-) der Index muss einmalig sein!
}

length_t Node::HeurDistanceTo(Node* pTo) const
{
	return ((pTo->xPos - xPos) * (pTo->xPos - xPos) 
		   +(pTo->yPos - yPos) * (pTo->yPos - yPos));
}

length_t Node::getX() const
{
	return xPos;
}
length_t Node::getY() const
{
	return yPos;
}

unsigned int Node::getIndex() const
{
	return index;
}
void Node::addEdge(Edge* pnewEdge)
{
	adjEdges.push_back(pnewEdge);
	return;
}

void Node::removeEdge(Edge* e)
{
	for (pEdg_v::iterator it = adjEdges.begin(); it != adjEdges.end(); it++)
	{
		if (*it == e) 
		{
			adjEdges.erase(it);
			return;
		}
	}
	return;
}

void Node::print() const
{
	std::cout << index << " " << xPos << " " << yPos << " " << status << std::endl;
	return;
}
