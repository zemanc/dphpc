#include "testgraph.h"

void Testgraph::removeRandomNodes(Graph& g, Graph::graphsize_t count, Graph::graphsize_t max) const
{
	Graph::pNtr_v nodesToRemove;
	for (unsigned int i = 0; i < count; i++)
	{
		unsigned int num;
		while (nodesToRemove.find(num = rand() % max ) != nodesToRemove.end());
			nodesToRemove.insert(num);
	}
	g.removeNodesEdges(nodesToRemove);
}
