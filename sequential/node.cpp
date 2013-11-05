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
	max_index--;
}

bool Node::hasAdjEdge() const
{
	return (!adjEdges.empty());
}

Node* Node::getNearestNode() const
{

	if (!hasAdjEdge()) return 0;

	length_t minDist = INT_MAX;
	Node* pNearestNode = 0;

	for (pEdg_v::const_iterator it = adjEdges.begin(); it != adjEdges.end(); it++)
		if (minDist > (*it)->getDistance()) 
		{
			minDist = (*it)->getDistance();
			pNearestNode = (*it)->getTo();
		}
		
	return pNearestNode;
}

Node* Node::getNearestOpenNode() const
{
	if (!hasAdjEdge()) return 0;

	//not the best way .... ;)
	length_t minDist = INT_MAX;
	Node* pNearestNode = 0;

	for (pEdg_v::const_iterator it = adjEdges.begin(); it != adjEdges.end(); it++)
		if (minDist > (*it)->getDistance() && (!(*it)->getTo()->status == open))
		{
			minDist = (*it)->getDistance();
			pNearestNode = (*it)->getTo();
		}
		
	return pNearestNode;

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
}

void Node::removeEdge(Edge* e)
{
	for (unsigned int i = 0; i < adjEdges.size(); i++)
	{
		if (adjEdges[i] == e) 
			adjEdges.erase(adjEdges.begin() + i);	
	}
}

void Node::print() const
{
	std::cout << index << " " << xPos << " " << yPos << " " << status << std::endl;
}
