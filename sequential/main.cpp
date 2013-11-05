#include "graph.h"
#include <list>

int main()
{
	Graph g;

	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 10; j++)
		{
			g.addNode(i, j);
		}
	

	std::vector<Edge*> pEdges_v;
	for (int i = 0; i < 10; i++)
		for (int j = 1; j < 10; j++)
		{
			g.addEdge(10*i+j-1, 10*i+j);
			g.addEdge(10*(j-1)+i, 10*j+i);
		}

 	g.removeEdge(33, 34);
	g.removeNode(69);
	g.removeNode(25);
	g.removeNode(26);
	g.removeNode(24);
	g.removeNode(34);

	std::list<Node*> *path = new std::list<Node*>;
	g.getShortestPath(1, 88, path);

 	g.printGml();

	return 0;
}


