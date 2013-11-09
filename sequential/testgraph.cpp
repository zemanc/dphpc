#include "testgraph.h"
#include <stdlib.h>
#include <cmath>

Graph Testgraph::getLukasOriginalGraph(unsigned int graphsize) const
{
	Graph g = getSimpleGraph(graphsize);

	for (int i = 50; i < 70; i++)
		for (int j = 30; j < 80; j++)
			g.removeNode(graphsize*i + j - 1);

	srand(0);
	for (unsigned int i = 0; i < graphsize * 10; i++)
		g.removeNode(g.getNodeIndexByInternalIndex(rand() % (graphsize*graphsize- i)));

	return g;

}

Graph Testgraph::getSmileyGraph(unsigned int graphsize) const
{
	Graph g = getSimpleGraph(graphsize);

	int gs = int(graphsize);
	const int sr = graphsize/2;		// smiley radius
	const int er = graphsize/8;		// eye radius
	const int ep = graphsize/3;		// eye pos
	const int ml = graphsize/2;		// mouth length
	const int mh = graphsize/8;		// mouth height
	const int mp = graphsize*2/3;	// mouth pos
	pNtr_v nodesToRemove;

	for (int i = 0; i < gs; i++)
		for (int j = 0; j < gs; j++)
		{
			if ( sqrt(pow(int(i)-sr, 2) + pow(int(j)-sr, 2)) > sr  )
				nodesToRemove.insert(gs*i+j);
			else if ( sqrt(pow(int(i)-ep, 2) + pow(int(j)-ep, 2)) < er )
				nodesToRemove.insert(gs*i+j);
			else if ( sqrt(pow(int(i)-2*ep, 2) + pow(int(j)-ep, 2)) < er )
				nodesToRemove.insert(gs*i+j);
			else if ( j > mp && j < mp+mh && i > ml/2 && i < ml/2+ml )
				nodesToRemove.insert(gs*i+j);
// 				g.removeNode(gs*i + j);	// mouth
		}
	
	g.removeNodesEdges(nodesToRemove);	
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

