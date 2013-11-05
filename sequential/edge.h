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

		Node* getFrom() const;
		Node* getTo() const;
		length_t getDistance() const;

};


#endif
