#include "testgraph.h"

void Testgraph::removeRandomNodes(Graph& g, unsigned int count, unsigned int max) const
{
	pNtr_v nodesToRemove;
	for (unsigned int i = 0; i < count; i++)
	{
		unsigned int num;
		while (nodesToRemove.find(num = rand() % max ) != nodesToRemove.end());
			nodesToRemove.insert(num);
	}
	g.removeNodesEdges(nodesToRemove);
}
