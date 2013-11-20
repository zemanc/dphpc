#include "graph.h"
#include <iostream>
#include <fstream>

Graph::~Graph()
{
	for  (pEdg_v_it it = pEdges_v.begin(); it != pEdges_v.end(); it++)
		delete *it;

	for  (pNode_v_it it = pNodes_v.begin(); it != pNodes_v.end(); it++)
		delete *it;
}

void Graph::addNode(length_t xPos, length_t yPos)
{
	pNodes_v.push_back(new Node(xPos, yPos));
	return;
}

void Graph::addAllEdges8Directions(unsigned int graphsize)
{

	EuklidDistance ek_distance;	

	pNode_v_it it1 = pNodes_v.begin();
	pNode_v_it it2 = pNodes_v.begin() + 1;
	pNode_v_it it3 = pNodes_v.begin() + graphsize;
	pNode_v_it it4 = pNodes_v.begin() + graphsize + 1;

	for (unsigned int i = 1; i <= graphsize; i++)
		for (unsigned int j = 1; j <= graphsize; j++)
		{
			if (j < graphsize)
			{
				pEdges_v.push_back(new Edge(*it1, *it2, ek_distance));
				pEdges_v.push_back(new Edge(*it2, *it1, ek_distance));
			}
			if (i < graphsize)
			{
				pEdges_v.push_back(new Edge(*it1, *it3, ek_distance));
				pEdges_v.push_back(new Edge(*it3, *it1, ek_distance));
			}
			if (i < graphsize && j < graphsize)
			{
				pEdges_v.push_back(new Edge(*it1, *it4, ek_distance));
				pEdges_v.push_back(new Edge(*it4, *it1, ek_distance));
				pEdges_v.push_back(new Edge(*it2, *it3, ek_distance));
				pEdges_v.push_back(new Edge(*it3, *it2, ek_distance));
			}
			it1++;
			it2++;
			it3++;
			it4++;
		}
}

void Graph::addAllEdges4Directions(unsigned int graphsize)
{
	ManhattanDistance mh_distance;

	pNode_v_it it1 = pNodes_v.begin();
	pNode_v_it it2 = pNodes_v.begin() + 1;
	pNode_v_it it3 = pNodes_v.begin() + graphsize;

	for (unsigned int i = 1; i <= graphsize; i++)
		for (unsigned int j = 1; j <= graphsize; j++)
		{
			if (j < graphsize)
			{
				pEdges_v.push_back(new Edge(*it1, *it2, mh_distance));
				pEdges_v.push_back(new Edge(*it2, *it1, mh_distance));
			}
			if (i < graphsize)
			{
				pEdges_v.push_back(new Edge(*it1, *it3, mh_distance));
				pEdges_v.push_back(new Edge(*it3, *it1, mh_distance));
			}
			it1++;
			it2++;
			it3++;
		}
}

bool Graph::addEdge(unsigned int from, unsigned int to)
{
	EuklidDistance ek_distance;
	if ((from < pNodes_v.size()) && (to < pNodes_v.size()))
	{
		pEdges_v.push_back(new Edge(getNode(from), getNode(to), 
					ek_distance));
		return true;
	} else {
		return false;
	}
}

Node* Graph::getNode(unsigned int index)
{
	for  (pNode_v_it it = pNodes_v.begin(); it != pNodes_v.end(); it++)
		if ((*it)->getIndex() == index)
			return *it;

	return 0;
}

void Graph::cleanup()
{
	for (pNode_v_it it = pNodes_v.begin(); it != pNodes_v.end(); it++)
	{
		(*it)->status = Node::inactive;
		(*it)->next = NULL;
		(*it)->prev = NULL;
		(*it)->parent = 0;
		(*it)->f = 0;
		(*it)->g = 0;
		(*it)->h = 0;
	}
}

std::string Graph::getColor(Node::state_t s) const
{
	switch (s)
	{
		case Node::open:
			return "#FF0000";
		case Node::closed:
			return "#BBEEBB";
		case Node::inactive:
			return "#EEEEEE";
		case Node::onPath:
			return "#0000FF";
		default:
			return "#000000";
	}
}

void Graph::printGml() const
{
	std::cout << "graph [" << std::endl;
	std::cout << '\t' << "directed 1" << std::endl;

	for (unsigned int i = 0; i < pNodes_v.size(); i++)
		if (pNodes_v[i]->adjEdges.size() > 0)
			std::cout << '\t' << "node [" << std::endl
				<< "\t\tid " << pNodes_v[i]->getIndex() << std::endl
				<< "\t\tgraphics [ " << std::endl
				<< "\t\t\tfill \"" << 
				getColor(pNodes_v[i]->status) << "\"" << std::endl
				<< "\t\t\tx " << pNodes_v[i]->getX()*100 << std::endl
				<< "\t\t\ty " << pNodes_v[i]->getY()*100 << std::endl
				<< "\t\t\tw 60" << std::endl
				<< "\t\t\th 60" << std::endl
				<< "\t\t]" << std::endl
				<< "\t]" << std::endl << std::endl;

	std::cout << std::endl;

	for (unsigned int i = 0; i < pEdges_v.size(); i++)
		std::cout << "\tedge [" << std::endl
			<< "\t\tsource " << pEdges_v[i]->getFrom()->getIndex() << std::endl
			<< "\t\ttarget " << pEdges_v[i]->getTo()->getIndex() << std::endl
			<< "\t]" << std::endl << std::endl;

	std::cout << "]" << std::endl;

	return;
}
void Graph::printTgf() const
{
	for (unsigned int i = 0; i < pNodes_v.size(); i++)
		std::cout << pNodes_v[i]->getIndex() << 
			" " << pNodes_v[i]->getIndex() << std::endl;

	std::cout << "#" << std::endl;

	for (unsigned int i = 0; i < pEdges_v.size(); i++)
		std::cout << pEdges_v[i]->getFrom()->getIndex() << " " 
			<< pEdges_v[i]->getTo()->getIndex() << std::endl;

	return;
}

length_t Graph::newThreshold(length_t oldThreshold)
{
	return oldThreshold + 3;
}

unsigned int Graph::getNodeIndexByInternalIndex(unsigned int i)
{
	return pNodes_v[i]->getIndex();
}

