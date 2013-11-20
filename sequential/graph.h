#ifndef __graph_h__

#define __graph_h__

#include <vector>
//#include <unordered_set>
#include <set>
#include <list>
#include <string>

#include "edge.h"
#include "node.h"

typedef std::set<unsigned int> pNtr_v;
typedef pNtr_v::iterator pNtr_v_it;
typedef std::vector<Edge*> pEdg_v;
typedef pEdg_v::iterator pEdg_v_it;
typedef pEdg_v::const_iterator pEdg_v_cit;
typedef std::vector<Node*> pNode_v;
typedef pNode_v::iterator pNode_v_it;
typedef pNode_v::const_iterator pNode_v_cit;

class Graph
{
	private:

		double newThreshold(double);
		std::string getColor(Node::state_t) const;

	public:
		enum edge_type {dir4, dir8};

		~Graph();
		Graph() {};
		Graph(std::string); //nicht fertig

		pNode_v pNodes_v;
		pEdg_v pEdges_v;

		void addNode(length_t, length_t);
		void addAllEdges8Directions(unsigned int);
		void addAllEdges4Directions(unsigned int);
 		void removeNodesEdges(pNtr_v);					// do not use
		bool addEdge(unsigned int, unsigned int);		// do not use
		void removeNode(unsigned int);					// do not use
		bool removeEdge(unsigned int, unsigned int);	// do not use
		void removeEdge(unsigned int);					// do not use
		void removeEdge(pEdg_v_it&);					// do not use
		Node* getNode(unsigned int);

		void printTgf() const;
		void printGml() const;
		
		unsigned int nodeCount() const { return pNodes_v.size(); };

		template<class F>
   			double getShortestPath(unsigned int, unsigned int, std::list<Node*>*, const F&);

		template<class F>
		double reconstructPath(std::list<Node*>* vals, Node*, Node*, const F&);

		unsigned int getNodeIndexByInternalIndex(unsigned int);

		template<class F>
		void randomDisplaceAllNodes(double, const F&);

		void save(std::string fn) const;

};

#include "graph.distances.h"
#include "graph.getShortestPath.h"
#include "graph.randomDisplaceAllNodes.h"

#endif
