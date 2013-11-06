#include "testgraph.h"
#include <stdlib.h>

Graph Testgraph::getLukasOriginalGraph(unsigned int graphsize) const
{
	Graph g = getSimpleGraph(graphsize);;

	for (int i = 50; i < 70; i++)
		for (int j = 30; j < 80; j++)
			g.removeNode(graphsize*i + j - 1);

	srand(0);
	for (unsigned int i = 0; i < graphsize * 10; i++)
		g.removeNode(g.getNodeIndexByInternalIndex(rand() % (graphsize*graphsize- i)));

	return g;

}

Graph Testgraph::getSimpleGraph(unsigned int graphsize) const
{
	Graph g;

	for (unsigned int i = 0; i < graphsize; i++)
		for (unsigned int j = 0; j < graphsize; j++)
			g.addNode(i, j);

	for (unsigned int i = 0; i < graphsize; i++)
		for (unsigned int j = 1; j < graphsize; j++)
		{
			g.addEdge(graphsize*i+j-1, graphsize*i+j);
			g.addEdge(graphsize*i+j, graphsize*i+j-1);
			g.addEdge(graphsize*(i-1)+j, graphsize*i+j);
			g.addEdge(graphsize*i+j, graphsize*(i-1)+j);
		}

	return g;

}
