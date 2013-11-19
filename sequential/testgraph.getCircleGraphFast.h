#ifndef __testgraph_getCircleGraphFast_h__

#define __testgraph_getCircleGraphFast_h__

template<class F>
Graph Testgraph::getCircleGraphFast(unsigned int graphsize, const F& dist) const
{
	Graph g;
	const int gs = int(graphsize);
	const int cp = gs/2;
	const int r = gs/4;
	const int rr = r*r;

	// add nodes
	for (int i = 0; i < gs; i++)
		for (int j = 0; j < gs; j++)
			g.pNodes_v.push_back(new Node(i, j));

	// add edges
	pNode_v_it it1 = g.pNodes_v.begin();
	pNode_v_it it2 = g.pNodes_v.begin() + 1;
	pNode_v_it it3 = g.pNodes_v.begin() + gs;

	for (int i = 1; i <= gs; i++)
		for (int j = 1; j <= gs; j++)
		{
			if (j < gs)
			{
				if ( pow(i-1-cp, 2) + pow(j-1-cp, 2) >= rr
					&& pow(i-1-cp, 2) + pow(j-cp, 2) >= rr )
				{
					g.pEdges_v.push_back(new Edge(*it1, *it2, dist));
					g.pEdges_v.push_back(new Edge(*it2, *it1, dist));
				}
			}
			if (i < gs)
			{
				if ( pow(i-1-cp, 2) + pow(j-1-cp, 2) >= rr
					&& pow(i-cp, 2) + pow(j-1-cp, 2) >= rr )
				{
					g.pEdges_v.push_back(new Edge(*it1, *it3, dist));
					g.pEdges_v.push_back(new Edge(*it3, *it1, dist));
				}
			}
			it1++;
			it2++;
			it3++;
		}
	return g;
}

#endif
