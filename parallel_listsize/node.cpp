#include "node.h"
#include <climits>
#include <iostream>
#include <cmath>

Node::Node(length_t xP, length_t yP, index_t i) 
									 : index(i)
									 , xPos(xP)
									 , yPos(yP)
									 , parent(0)
									 , status(inactive)
									 , f(0)
									 , g(0)
									 , h(0)
									 , next(NULL)
									 , prev(NULL)
{
}
Node::~Node()
{
}

void Node::addEdge(Node* dest, length_t dist)
{
	adjEdges.push_back(std::make_pair(dest, dist));
}
// void Node::addEdge(edge_t e)
// {
// 	adjEdges.push_back(e);
// }

void Node::print() const
{
	std::cout << index << " " << xPos << " " << yPos << " " << status << std::endl;
	return;
}

// void Node::removeEdge(Node* dest) 
// {
// 	for (edges_t::iterator it = adjEdges.begin(); it != adjEdges.end(); it++)
// 	{
// 		if ((*it).first == dest) 
// 		{
// 			adjEdges.erase(it);
// 			return;
// 		}
// 	}
// 	return;
// }

