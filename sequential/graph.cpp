#include "graph.h"
#include <iostream>
#include <fstream>

void Graph::addNode(length_t xPos, length_t yPos)
{
	pNodes_v.push_back(new Node(xPos, yPos));
	return;
}

void Graph::removeNodesEdges(pNtr_v nodesToRemove)
{
	pNode_v_it it_node = pNodes_v.begin();
	pEdg_v_it it_edg = pEdges_v.begin();
	pNtr_v_it it_ntrFrom;
	pNtr_v_it it_ntrTo;

	// remove edges
	while (it_edg != pEdges_v.end())
	{
		it_ntrFrom = nodesToRemove.find((*it_edg)->getFrom()->getIndex());
		it_ntrTo = nodesToRemove.find((*it_edg)->getTo()->getIndex());
		if ( it_ntrFrom != nodesToRemove.end() || it_ntrTo != nodesToRemove.end() )
		{
			(*it_edg)->getFrom()->removeEdge(*it_edg);
			(*it_edg)->getTo()->removeEdge(*it_edg);
			delete *it_edg;
			it_edg = pEdges_v.erase(it_edg);
		}
		else 
		{
			it_edg++;
		}
	}

	// remove nodes
	it_node = pNodes_v.begin();
	while (it_node != pNodes_v.end() )
	{
		if (nodesToRemove.find((*it_node)->getIndex()) != nodesToRemove.end())
		{
			delete *it_node;
			it_node = pNodes_v.erase(it_node);
		}
		else
		{
			it_node++;
		}

	}
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

void Graph::removeNode(unsigned int index)
{

	pEdg_v_it it_edg = pEdges_v.begin();
	while (it_edg != pEdges_v.end() )
	{
		if ( ( (*it_edg)->getFrom()->getIndex() == index)
				|| ( (*it_edg)->getTo()->getIndex() == index))
			removeEdge(it_edg);
		else
			it_edg++;
	}

	for  (pNode_v_it it = pNodes_v.begin(); it != pNodes_v.end(); it++)
		if ((*it)->getIndex() == index)
		{
			delete *it;
			pNodes_v.erase(it);
			return;
		}

	return;

}
Node* Graph::getNode(unsigned int index)
{
	for  (pNode_v_it it = pNodes_v.begin(); it != pNodes_v.end(); it++)
		if ((*it)->getIndex() == index)
			return *it;

	return 0;
}

void Graph::removeEdge(pEdg_v_it& it)
{
	(*it)->getFrom()->removeEdge(*it);
	delete *it;
	it = pEdges_v.erase(it);
}
void Graph::removeEdge(unsigned int index)
{
	pEdges_v[index]->getFrom()->removeEdge(pEdges_v[index]);
	delete pEdges_v[index];
	pEdges_v.erase(pEdges_v.begin() + index);
}
bool Graph::removeEdge(unsigned int from, unsigned int to)
{
	for  (pEdg_v_it it = pEdges_v.begin(); it != pEdges_v.end(); it++)
	{
		if (((*it)->getFrom()->getIndex() == from)
				&& (*it)->getTo()->getIndex() == to)
		{	
			removeEdge(it);
			return true;
		}
	}

	return false;

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

Graph::~Graph()
{
	for  (pEdg_v_it it = pEdges_v.begin(); it != pEdges_v.end(); it++)
		delete *it;

	for  (pNode_v_it it = pNodes_v.begin(); it != pNodes_v.end(); it++)
		delete *it;
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

double Graph::newThreshold(double oldThreshold)
{
	return oldThreshold + 1;
}

void Graph::reconstructPath(std::list<Node*>* vals, Node* start, Node* end)
{

	Node* it = end;
	while (it != start)
	{
		vals->push_front(it);
		it->status = Node::onPath;
		it = it->parent;
	}

	vals->push_front(start);
	start->status = Node::onPath;

	return;
}

unsigned int Graph::getNodeIndexByInternalIndex(unsigned int i)
{
	return pNodes_v[i]->getIndex();
}

void Graph::save(std::string fn) const
{
	std::ofstream outfile(fn);
	for (pNode_v_cit it = pNodes_v.begin(); it != pNodes_v.end(); it++)
		outfile << (*it)->getIndex() << " " 
				<< (*it)->getX() << " " 
			    << (*it)->getY() << std::endl;
	
	outfile << std::endl;

	for (pEdg_v_cit it = pEdges_v.begin(); it != pEdges_v.end(); it++)
		outfile << (*it)->getFrom()->getIndex() << " " 
				<< (*it)->getTo()->getIndex() << " "
				<< (*it)->getDistance() << std::endl;

}

Graph::Graph(std::string fn)
{

}
