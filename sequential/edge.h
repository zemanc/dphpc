#ifndef __edge_h__

#define __edge_h__

typedef double length_t;

#include "node.h"

class Edge 
{
	private:
		Node* from;
		Node* to;
		length_t dist;

	public: 
		template<class F> Edge(Node*, Node*, const F&);
		Edge(Node*, Node*, length_t);

		inline Node* getFrom() const { return from; };
		inline Node* getTo() const { return to; };
		inline void setDistance(length_t d) { dist = d; return; };
		inline length_t getDistance() const { return dist; };

};

template<class F>
Edge::Edge(Node* pFrom, Node* pTo, const F& distance)
											: from(pFrom)
											, to(pTo)
{
	pFrom->addEdge(this);
	this->dist = distance(pFrom, pTo);
}
	


#endif
