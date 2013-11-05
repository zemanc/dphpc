#ifndef __graph_h__

#define __graph_h__

#include <vector>
#include <list>
#include <string>

#include "edge.h"
#include "node.h"

class Graph
{
	private:
		std::vector<Node*> pNodes_v;
		std::vector<Edge*> pEdges_v;

		double newThreshold(double);
		std::string getColor(Node::state_t) const;

	public:
		~Graph();

		// no need to provide performand inserting and removing!
		void addNode(length_t, length_t);
		bool addEdge(unsigned int, unsigned int);
		void removeNode(unsigned int);
		bool removeEdge(unsigned int, unsigned int);
		void removeEdge(unsigned int);
		Node* getNode(unsigned int);

		void printTgf() const;
		void printGml() const;

		void getShortestPath(unsigned int, unsigned int, std::list<Node*>*);

};



#endif
