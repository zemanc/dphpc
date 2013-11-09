#ifndef __graph_h__

#define __graph_h__

#include <vector>
#include <map>
#include <list>
#include <string>

#include "edge.h"
#include "node.h"

class Graph
{
	private:
		typedef std::vector<Edge*> pEdg_v;
		typedef pEdg_v::iterator pEdg_v_it;
		typedef std::vector<Node*> pNode_v;
		typedef pNode_v::iterator pNode_v_it;
// 		typedef std::map<unsigned int, char> pNtr_v;
// 		typedef pNtr_v::iterator pNtr_v_it;

		pNode_v pNodes_v;
		pEdg_v pEdges_v;

		double newThreshold(double);
		std::string getColor(Node::state_t) const;

	public:
		~Graph();

		// no need to provide performant inserting and removing!
		void addNode(length_t, length_t);
		void removeNodesEdges(std::map<unsigned int, char>&);
		bool addEdge(unsigned int, unsigned int);
		void removeNode(unsigned int);
		bool removeEdge(unsigned int, unsigned int);
		void removeEdge(unsigned int);
		void removeEdge(pEdg_v_it&);
		Node* getNode(unsigned int);

		void printTgf() const;
		void printGml() const;

		void getShortestPath(unsigned int, unsigned int, std::list<Node*>*);
		void reconstructPath(std::list<Node*>* vals, Node*, Node*);

		unsigned int getNodeIndexByInternalIndex(unsigned int);

};



#endif
