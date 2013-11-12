#include "graph.h"

class Testgraph
{
	public:

		enum edge_type {dir4, dir8};

		Graph getLukasOriginalGraph(unsigned int) const;
		Graph getSmileyGraph(unsigned int, edge_type) const;
		Graph getSimpleGraph(unsigned int, edge_type) const;

		void removeRandomNodes(Graph&, unsigned int, unsigned int) const;
};
