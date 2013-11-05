#include "graph.h"
#include <iostream>

void Graph::addNode(length_t xPos, length_t yPos)
{
	pNodes_v.push_back(new Node(xPos, yPos));
	return;
}
void Graph::removeNode(unsigned int index)
{

	for  (unsigned int i = 0; i < pEdges_v.size(); i++)
	{
		if ((pEdges_v[i]->getFrom()->getIndex() == index)
		  || (pEdges_v[i]->getTo()->getIndex() == index))
			removeEdge(i);
	}

	for (unsigned int i = 0; i < pNodes_v.size(); i++)
		if (pNodes_v[i]->getIndex() == index)
		{
			delete pNodes_v[i];
			pNodes_v.erase(pNodes_v.begin() + i);
		}

}
Node* Graph::getNode(unsigned int index)
{
	for (unsigned int i = 0; i < pNodes_v.size(); i++)
		if (pNodes_v[i]->getIndex() == index)
			return pNodes_v[i];
	
	return 0;
}

void Graph::removeEdge(unsigned int index)
{
	pEdges_v[index]->getFrom()->removeEdge(pEdges_v[index]);
	delete pEdges_v[index];
	pEdges_v.erase(pEdges_v.begin() + index);
}
bool Graph::removeEdge(unsigned int from, unsigned int to)
{
	for (unsigned int i = 0; i < pEdges_v.size(); i++)
	{
		if ((pEdges_v[i]->getFrom()->getIndex() == from)
		  		&& pEdges_v[i]->getTo()->getIndex() == to)
		{	
		  	removeEdge(i);
			return true;
		}
	}

	return false;

}

bool Graph::addEdge(unsigned int from, unsigned int to)
{
	if ((from < pNodes_v.size()) && (to < pNodes_v.size()))
	{
		pEdges_v.push_back(new Edge(pNodes_v[from], pNodes_v[to]));
		return true;
	} else {
		return false;
	}
}

Graph::~Graph()
{
	for (unsigned int i = 0; i < pEdges_v.size(); i++)
		delete pEdges_v[i];

	for (unsigned int i = 0; i < pNodes_v.size(); i++)
		delete pNodes_v[i];
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
			return "#FFFFFF";
		default:
			return "#000000";
	}
}
void Graph::printGml() const
{
	std::cout << "graph [" << std::endl;
	std::cout << '\t' << "directed 1";

	for (unsigned int i = 0; i < pNodes_v.size(); i++)
		std::cout << '\t' << "node [" << std::endl
			      << "\t\tid " << pNodes_v[i]->getIndex() << std::endl
			      << "\t\tlabel \"" << pNodes_v[i]->getIndex() << "\"" << std::endl
				  << "\t\tgraphics [ fill \""
				      << getColor(pNodes_v[i]->status)
					  << "\" ]" << std::endl
				  << "\t]" << std::endl << std::endl;

	std::cout << std::endl;

	for (unsigned int i = 0; i < pEdges_v.size(); i++)
		std::cout << "\tedge [" << std::endl
		          << "\t\tsource " << pEdges_v[i]->getFrom()->getIndex() << std::endl
				  << "\t\ttarget " << pEdges_v[i]->getTo()->getIndex() << std::endl
				  << "\t]" << std::endl << std::endl;
	
	std::cout << "]" << std::endl;
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
}

double Graph::newThreshold(double oldThreshold)
{
	return oldThreshold + 1;
}

void Graph::getShortestPath(unsigned int from, unsigned int to, std::list<Node*>* vals)
{
	std::list<Node*> nowlater;

	Node* start = getNode(from);
	Node* end = getNode(to);

	start->g = 0;
	start->h = start->HeurDistanceTo(end);
	start->f = start->g + start->h;

	nowlater.push_back(start);

	double threshold = start->f; //start with shortest possible paths

	while (true)
	{
		std::list<Node*>::iterator it = nowlater.begin();
		while (it != nowlater.end())
		{

			Node* curNode = *it;

			//if under treshold, than work with it - otherwise ignore!
			if (curNode->f <= threshold)
			{

				if (curNode == end)
					return;
					
				curNode->status = Node::closed;
				it = nowlater.erase(it);

				//for each neighbor
				for (Node::pEdg_v::iterator edge_it = curNode->adjEdges.begin(); 
					 edge_it != curNode->adjEdges.end(); edge_it++)
				{

					Node* edge_to = (*edge_it)->getTo();

					if (edge_to->status == Node::inactive)
					{
						//New Node!
						edge_to->g = curNode->f + (*edge_it)->getDistance();
						edge_to->h = edge_to->HeurDistanceTo(end);
						edge_to->f = edge_to->g + edge_to->h;

						edge_to->parent = curNode;

						//at the moment for simplifying, insert in the end
						nowlater.push_back(edge_to);

						edge_to->status = Node::open;
					} else if (edge_to->status == Node::open) {
						
						double newDist = curNode->g + (*edge_it)->getDistance();
						if (edge_to->g > newDist)
						{
							//better node!

							edge_to->g =  newDist;
							edge_to->f = newDist + edge_to->h;

							edge_to->parent = curNode;

							//now we could change the list, if it' sunder the threshold!
						}
					}
						
				}
			}
		it++;
		}

		threshold = newThreshold(threshold);

	}
}
