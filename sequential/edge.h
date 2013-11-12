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
		Edge(Node*, Node*);
		Edge(Node*, Node*, length_t);

		inline Node* getFrom() const { return from; };
		inline Node* getTo() const { return to; };
		inline length_t getDistance() const { return dist; };

};


#endif
