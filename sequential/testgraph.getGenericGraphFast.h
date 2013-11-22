#ifndef __testgraph_getGenericGraphFast_h__

#define __testgraph_getGenericGraphFast_h__

template<class F, class G>
void Testgraph::getGenericGraphFast(Graph::graphsize_t graphsize, const F& dist, 
									 const G& isEdge, Graph& g) const
{
	const int gs = static_cast<int>(graphsize);

	// add nodes
	for (int i = 0; i < gs; i++)
		for (int j = 0; j < gs; j++)
			g.addNode(i, j);

	// add edges
	Graph::pNode_v_it it1 = g.pNodes.begin();
	Graph::pNode_v_it it2 = g.pNodes.begin() + 1;
	Graph::pNode_v_it it3 = g.pNodes.begin() + gs;
	Graph::pNode_v_it it4 = g.pNodes.begin() + gs + 1;

	for (int i = 0; i < gs; i++)
		for (int j = 0; j < gs; j++)
		{
			if (j < gs - 1)
			{
				if (isEdge(i, j, i, j+1) )
					g.addEdge(*it1, *it2, dist);

				if (isEdge(i, j+1, i, j) )
					g.addEdge(*it2, *it1, dist);
			}
			if (i < gs - 1)
			{
				if (isEdge(i, j, i+1, j) )
					g.addEdge(*it1, *it3, dist);

				if (isEdge(i+1, j, i, j) )
					g.addEdge(*it3, *it1, dist);
			}

			if (F::edge_t == Graph::graph_type::dir8)
			{
				if ( i < gs - 1 && j < gs - 1)
				{
					if (isEdge(i, j, i+1, j+1) )
						g.addEdge(*it1, *it4, dist);

					if (isEdge(i+1, j+1, i, j) )
						g.addEdge(*it4, *it1, dist);

					if (isEdge(i+1, j, i, j+1) )
						g.addEdge(*it3, *it2, dist);

					if (isEdge(i, j+1, i+1, j) )
						g.addEdge(*it2, *it3, dist);

				}
			}

			it1++;
			it2++;
			it3++;
			it4++;
		}

}

#endif
