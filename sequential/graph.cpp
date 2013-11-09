#include "graph.h"
#include <iostream>

void Graph::addNode(length_t xPos, length_t yPos)
{
	pNodes_v.push_back(new Node(xPos, yPos));
	return;
}

void Graph::removeNodesEdges(std::map<unsigned int, char>& nodesToRemove)
{
	pNode_v_it it_node = pNodes_v.begin();
	pEdg_v_it it_edg = pEdges_v.begin();
	std::map<unsigned int, char>::iterator it_ntr;
	// remove nodes
	while (it_node != pNodes_v.end() )
	{
		it_ntr = nodesToRemove.find((*it_node)->getIndex());
		if (it_ntr != nodesToRemove.end())
			it_node = pNodes_v.erase(it_node);
		else
			it_node++;
	}
	// remove edges
	while (it_edg != pEdges_v.end())
	{
		it_ntr = nodesToRemove.find((*it_node)->getIndex());
		if ( ( (*it_edg)->getFrom()->getIndex() == it_ntr->first)
		  || ( (*it_edg)->getTo()->getIndex() == it_ntr->first) )
			 it_edg = pEdges_v.erase(it_edg);
		else 
			it_edg++;	
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
	if ((from < pNodes_v.size()) && (to < pNodes_v.size()))
	{
		pEdges_v.push_back(new Edge(getNode(from), getNode(to)));
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

void Graph::getShortestPath(unsigned int from, unsigned int to, std::list<Node*>* vals)
{
	typedef std::list<Node*> node_list;

	node_list nowlater;

	//geht besser so als randomized... für den Moment. Später ist das wohl dann der "richtige" Index, nicht der interne!
	Node* start = pNodes_v[from];
	Node* end = pNodes_v[to];

	start->g = 0;
	start->h = start->HeurDistanceTo(end);
	start->f = start->g + start->h;

	start->list_pos = nowlater.insert(nowlater.begin(), start);

	double threshold = start->f; //start with shortest possible paths

	while (!nowlater.empty())
	{

		node_list::iterator it = nowlater.begin();
		while (it != nowlater.end())
		{
			Node* curNode = *it;

			//if under treshold, than work with it - otherwise ignore!
			if (curNode->f <= threshold)
			{
				if (curNode == end)
					return reconstructPath(vals, start, end);
					
				curNode->status = Node::closed;

				//for each neighbor
				for (Node::pEdg_v::iterator edge_it = curNode->adjEdges.begin(); 
					 edge_it != curNode->adjEdges.end(); edge_it++)
				{
					Node* edge_to = (*edge_it)->getTo();

					if (edge_to->status == Node::inactive)
					{
						//New Node!
						edge_to->g = curNode->g + (*edge_it)->getDistance();
						edge_to->h = edge_to->HeurDistanceTo(end);
						edge_to->f = edge_to->g + edge_to->h;

						edge_to->parent = curNode;

						//at the moment for simplifying, insert at the end
						node_list::iterator it2 = it;
						it2++;
						edge_to->list_pos = nowlater.insert(it2, edge_to);

						edge_to->status = Node::open;

					} else if (edge_to->status == Node::open) {
						
						double newDist = curNode->g + (*edge_it)->getDistance();
						if (edge_to->g > newDist)
						{
							//better node!

							edge_to->g =  newDist;
							edge_to->f = newDist + edge_to->h;

							edge_to->parent = curNode;

							node_list::iterator it2 = it;
							it2++;
							nowlater.splice(it2, nowlater, edge_to->list_pos);

							//now we could change the list, if it' sunder the threshold!
						}
					}
						
				}
				it = nowlater.erase(it);

			} else {
				it++;
			}
		}
		threshold = newThreshold(threshold);

	}
}

unsigned int Graph::getNodeIndexByInternalIndex(unsigned int i)
{
	return pNodes_v[i]->getIndex();
}
