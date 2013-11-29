#ifndef __testgraph_h__

#define __testgraph_h__

#include "graph.h" 

class Testgraph
{
	public:

		template<class F, class G>
		void getGenericGraphFast(Graph::graphsize_t, const F&, const G&, Graph&) const;
		void removeRandomNodes(Graph&, Graph::graphsize_t) const;
		void removeRandomEdges(Graph&, Graph::graphsize_t) const;
};

#include "testgraph.getGenericGraphFast.h"
#include "testgraph.genericGraphs.h"

#endif

