#include "edge.h"
#include <climits>
#include <iostream>
#include <cmath>

unsigned int Node::max_index = 0;

Node::Node(length_t xP, length_t yP) : xPos(xP)
									 , yPos(yP)
									 , parent(0)
									 , status(inactive)
									 , f(0)
									 , g(0)
									 , h(0)
									 , next(NULL)
									 , prev(NULL)
{
	index = max_index++;
}
Node::~Node()
{
//	max_index--; das wäre ja gar nicht gut! :-) der Index muss einmalig sein!
}

void Node::setX(length_t x) 
{
	xPos = x;
}
void Node::setY(length_t y)
{
	yPos = y;
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
}

void Node::print() const
{
	std::cout << index << " " << xPos << " " << yPos << " " << status << std::endl;
	return;
}
