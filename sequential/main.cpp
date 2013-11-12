#include "graph.h"
#include <list>
#include <iostream>
#include "testgraph.h"

int main()
{

	Testgraph tg;
// 	Graph g = tg.getSimpleGraph(100);
// 	Graph g = tg.getLukasOriginalGraph(100);
	Graph g = tg.getSmileyGraph(100, Testgraph::dir8);
	tg.removeRandomNodes(g, 2000, 10000);

	std::list<Node*> *path = new std::list<Node*>;
// 	g.getShortestPath(1, 5549, path);
	g.getShortestPath(2686, 2481, path);	// smiley values

  	g.printGml();

	delete path;

	return 0;
}


