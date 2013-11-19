#ifndef __testgraph_h__

#define __testgraph_h__

#include "graph.h" 

class Testgraph
{
	public:

		enum edge_type {dir4, dir8};

		Graph getLukasOriginalGraph(unsigned int) const;
		Graph getSmileyGraph(unsigned int, edge_type) const;
		Graph getSimpleGraph(unsigned int, edge_type) const;
		Graph getCircleGraph(unsigned int, edge_type) const;

		template<class F>
		Graph getCircleGraphFast(unsigned int, const F&) const;

		void removeRandomNodes(Graph&, unsigned int, unsigned int) const;
};

#include "testgraph.getCircleGraphFast.h"

#endif

