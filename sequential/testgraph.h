#ifndef __testgraph_h__

#define __testgraph_h__

#include "graph.h" 

class Testgraph
{
	public:

		template<class F, class G>
		void getGenericGraphFast(unsigned int, const F&, const G&, Graph&) const;
		void removeRandomNodes(Graph&, unsigned int, unsigned int) const;
};

#include "testgraph.getGenericGraphFast.h"
#include "testgraph.genericGraphs.h"

#endif

