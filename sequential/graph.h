#ifndef __graph_h__

#define __graph_h__

#include <vector>
#include <set>
#include <list>
#include <string>

#include "node.h"

//do never remove Nodes completely! Only remove Edges!
// Nodes -> Vector (difficult to remove)
// Edges -> vector (not easy to remove but okay because smaller)

class Graph
{
	private:

		length_t newThreshold(length_t) const;
		std::string getColor(Node::state_t) const;

	public:

		typedef std::set<index_t> pNtr_v; // for removeNodesEdges

		typedef std::vector<Node*> pNode_v_t;
		typedef pNode_v_t::iterator pNode_v_it;
		typedef pNode_v_t::const_iterator pNode_v_cit;

		typedef unsigned int graphsize_t;
		
		enum graph_type {dir4, dir8};

		~Graph();
		Graph() {};

		pNode_v_t pNodes;

		//graph.cpp
		void addNode(length_t, length_t);
// 		void addAllEdges8Directions(graphsize_t);
// 		void addAllEdges4Directions(graphsize_t);
		void removeNodesEdges(pNtr_v);

		//graph.addEdges.h
 		template<class F> void addEdge(Node*, Node*, const F&);
		template<class F> void addEdge(index_t, index_t, const F&);

		//graph.cpp
		void addEdge(Node*, Node*, length_t);
		void addEdge(index_t, index_t, length_t);


		graphsize_t nodeCount() const { return pNodes.size(); };

		void cleanup();

		void printTgf() const;
		void printGml() const;

		template<class F>
   			length_t getShortestPath(index_t, index_t, std::list<Node*>*, const F&);

		template<class F>
		length_t reconstructPath(std::list<Node*>* vals, Node*, Node*, const F&);

		template<class F>
		void randomDisplaceAllNodes(length_t, const F&);



};

#include "graph.addEdges.h"
#include "graph.distances.h"
#include "graph.getShortestPath.h"
#include "graph.randomDisplaceAllNodes.h"

#endif
