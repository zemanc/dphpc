#include "testgraph.h"

void Testgraph::removeRandomNodes(Graph& g, Graph::graphsize_t count) const
{
	Graph::graphsize_t max = g.nodeCount();

	Graph::pNtr_v nodesToRemove;
	for (unsigned int i = 0; i < count; i++)
	{
		index_t num;
		while (nodesToRemove.find(num = rand() % max ) != nodesToRemove.end());

		nodesToRemove.insert(num);
	}
	g.removeNodesEdges(nodesToRemove);
}

#include <iostream>
void Testgraph::removeRandomEdges(Graph& g, Graph::graphsize_t count) const
{
	Graph::graphsize_t max = g.nodeCount();

	for (unsigned int i = 0; i < count; i++)
	{
		index_t nodenum;
		while (g.pNodes[nodenum = rand() % max]->adjEdges.size() == 0);

		unsigned int aEs = g.pNodes[nodenum]->adjEdges.size();
 		index_t edgenum = rand() % aEs;
		g.pNodes[nodenum]->adjEdges.erase(g.pNodes[nodenum]->adjEdges.begin() + edgenum);
	}
}
