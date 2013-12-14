#include "graph.h"
#include <iostream>
#include <fstream>

Graph::~Graph()
{
	clear();
}

void Graph::clear()
{
	for  (pNode_v_it it = pNodes.begin(); it != pNodes.end(); it++)
		delete *it;

	pNodes.clear();
}

void Graph::addNode(length_t xPos, length_t yPos)
{
	pNodes.push_back(new Node(xPos, yPos, pNodes.size()));
	return;
}

void Graph::removeNodesEdges(pNtr_v nodesToRemove)
{
	typedef pNtr_v::iterator pNtr_v_it;

	// Nodes mal leeren
	for (pNtr_v_it it = nodesToRemove.begin(); it != nodesToRemove.end(); it++)
		pNodes[(*it)]->adjEdges.clear();

	for (pNode_v_it nit = pNodes.begin(); nit != pNodes.end(); nit++)
	{
		Node::edges_it_t eit = (*nit)->adjEdges.begin();

		while (eit != (*nit)->adjEdges.end())
		{
			pNtr_v_it it_ntrTo = nodesToRemove.find
							((*eit).first->getIndex());
			if ( it_ntrTo != nodesToRemove.end() )
				eit = (*nit)->adjEdges.erase(eit);
			else 
				eit++;
		}
	}
}

void Graph::cleanup()
{
	for (pNode_v_it it = pNodes.begin(); it != pNodes.end(); it++)
	{
		(*it)->deleted = false;
		(*it)->status = Node::inactive;
		(*it)->next = NULL;
		(*it)->prev = NULL;
		(*it)->parent = NULL;
		(*it)->f = 0;
		(*it)->g = 0;
		(*it)->h = 0;
	}
}
std::string Graph::getColor(Node::state_t s) const
{
	switch (s)
	{
		case Node::open1:
		case Node::open2:
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
	std::cout << '\t' << "directed 0" << std::endl;

	for (pNode_v_cit it = pNodes.begin(); it != pNodes.end(); it++)
		if ((*it)->adjEdges.size() > 0)
			std::cout << '\t' << "node [" << std::endl
				<< "\t\tid " << (*it)->getIndex() << std::endl
				<< "\t\tgraphics [ " << std::endl
				<< "\t\t\tfill \"" << 
				getColor((*it)->status) << "\"" << std::endl
				<< "\t\t\tx " << (*it)->getX()*100 << std::endl
				<< "\t\t\ty " << (*it)->getY()*100 << std::endl
				<< "\t\t\tw 40" << std::endl
				<< "\t\t\th 40" << std::endl
				<< "\t\t]" << std::endl
				<< "\t]" << std::endl << std::endl;

	std::cout << std::endl;

	for (pNode_v_cit nit = pNodes.begin(); nit != pNodes.end(); nit++)
		for (Node::edges_t::const_iterator eit = (*nit)->adjEdges.begin();
				eit != (*nit)->adjEdges.end(); eit++)
			std::cout << "\tedge [" << std::endl
				<< "\t\tsource " << (*nit)->getIndex() << std::endl
				<< "\t\ttarget " << (*eit).first->getIndex() << std::endl
				<< "\t]" << std::endl << std::endl;

	std::cout << "]" << std::endl;
}
void Graph::printTgf() const
{
	for (pNode_v_cit it = pNodes.begin(); it != pNodes.end(); it++)
		std::cout << (*it)->getX() << 
			" " << (*it)->getY() << std::endl;

	std::cout << "#" << std::endl;

	for (pNode_v_cit nit = pNodes.begin(); nit != pNodes.end(); nit++)
		for (Node::edges_t::const_iterator eit = (*nit)->adjEdges.begin();
				eit != (*nit)->adjEdges.end(); eit++)
			std::cout << (*nit)->getIndex() << " " 
				<< (*eit).first->getIndex() << std::endl;

	return;
}

length_t Graph::newThreshold(length_t oldThreshold) const
{
	return oldThreshold + threshold;
}

void Graph::addEdge(Node* from, Node* to, length_t dist)
{
	from->addEdge(to, dist);
}
void Graph::addEdge(index_t from, index_t to, length_t dist)
{
	addEdge(pNodes[from], pNodes[to], dist);
}

