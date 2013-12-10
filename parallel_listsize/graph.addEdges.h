#ifndef __graph_addEdges_h__

#define __graph_addEdges_h__

template<class F>
void Graph::addEdge(index_t from, index_t to, const F& dist)
{
	addEdge(pNodes[from], pNodes[to], dist);
}

template<class F>
void Graph::addEdge(Node* from, Node* to, const F& dist)
{
	double d = dist(from, to);
	addEdge(from, to, d);
}
#endif
