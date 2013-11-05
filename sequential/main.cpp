#include "graph.h"
#include <list>
#include <stdlib.h>
#include <iostream>

int main()
{
	Graph g;

	int max = 100;
	for (int i = 0; i < max; i++)
		for (int j = 0; j < max; j++)
		{
			g.addNode(i, j);
		}
	

	std::vector<Edge*> pEdges_v;
	for (int i = 0; i < max; i++)
		for (int j = 1; j < max; j++)
		{
			g.addEdge(max*i+j-1, max*i+j);
			g.addEdge(max*(j-1)+i, max*j+i);
		}

	for (int i = 50; i < 70; i++)
		for (int j = 30; j < 80; j++)
			g.removeNode(max*i + j - 1);

	srand(0);
	for (int i = 0; i < max * 10; i++)
		g.removeNode(g.getNodeIndexByInternalIndex(rand() % (max*max- i)));

  	g.printGml();
	std::list<Node*> *path = new std::list<Node*>;
	g.getShortestPath(6, 6655, path);

 	g.printGml();

	return 0;
}


