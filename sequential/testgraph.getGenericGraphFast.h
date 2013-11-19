#ifndef __testgraph_getGenericGraphFast_h__

#define __testgraph_getGenericGraphFast_h__

template<class F, class G>
Graph Testgraph::getGenericGraphFast(unsigned int graphsize, const F& dist, 
									 const G& isEdge) const
{
	Graph g;
	const int gs = int(graphsize);

	// add nodes
	for (int i = 0; i < gs; i++)
		for (int j = 0; j < gs; j++)
			g.pNodes_v.push_back(new Node(i, j));

	// add edges
	pNode_v_it it1 = g.pNodes_v.begin();
	pNode_v_it it2 = g.pNodes_v.begin() + 1;
	pNode_v_it it3 = g.pNodes_v.begin() + gs;
	pNode_v_it it4 = g.pNodes_v.begin() + gs + 1;

	//pNodes_v[0] = 0 / 0
	//pNodes_v[1] = 0 / 1
	//pNodes_v[2] = 0 / 2
	for (int i = 0; i < gs; i++)
		for (int j = 0; j < gs; j++)
		{
			if (j < gs - 1)
			{
				if (isEdge(i, j, i, j+1) )
					g.pEdges_v.push_back(new Edge(*it1, *it2, dist));

				if (isEdge(i, j+1, i, j) )
					g.pEdges_v.push_back(new Edge(*it2, *it1, dist));
			}
			if (i < gs - 1)
			{
				if (isEdge(i, j, i+1, j) )
					g.pEdges_v.push_back(new Edge(*it1, *it3, dist));

				if (isEdge(i+1, j, i, j) )
					g.pEdges_v.push_back(new Edge(*it3, *it1, dist));
			}

			if (F::edge_t == Graph::edge_type::dir8)
			{
				if ( i < gs - 1 && j < gs - 1)
				{
					if (isEdge(i, j, i+1, j+1) )
						g.pEdges_v.push_back(new Edge(*it1, *it4, dist));

					if (isEdge(i+1, j+1, i, j) )
						g.pEdges_v.push_back(new Edge(*it4, *it1, dist));

					if (isEdge(i+1, j, i, j+1) )
						g.pEdges_v.push_back(new Edge(*it3, *it2, dist));

					if (isEdge(i, j+1, i+1, j) )
						g.pEdges_v.push_back(new Edge(*it2, *it3, dist));

				}
			}

			it1++;
			it2++;
			it3++;
			it4++;
		}

	return g;
}

#endif
