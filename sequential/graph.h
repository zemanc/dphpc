#ifndef __graph_h__

#define __graph_h__

#include <vector>
//#include <unordered_set>
#include <set>
#include <list>
#include <string>

#include "edge.h"
#include "node.h"
#include <cmath>
#include <random>

typedef std::set<unsigned int> pNtr_v;
typedef pNtr_v::iterator pNtr_v_it;
typedef std::vector<Edge*> pEdg_v;
typedef pEdg_v::iterator pEdg_v_it;
typedef pEdg_v::const_iterator pEdg_v_cit;
typedef std::vector<Node*> pNode_v;
typedef pNode_v::iterator pNode_v_it;
typedef pNode_v::const_iterator pNode_v_cit;

struct EuklidDistance {
	EuklidDistance() {};
	length_t get(Node* const pFrom, Node* const pTo) const
	{
		return std::sqrt((pTo->getX() - pFrom->getX()) * (pTo->getX() - pFrom->getX()) 
			            +(pTo->getY() - pFrom->getY()) * (pTo->getY() - pFrom->getY()));
	};
};

struct ManhattanDistance 
{
	ManhattanDistance() {};
	length_t get(Node* const pFrom, Node* const pTo) const
	{
		return std::abs(pTo->getX() - pFrom->getX()) + std::abs(pTo->getY() - pFrom->getY());
	};
};

class Graph
{
	private:
		pNode_v pNodes_v;
		pEdg_v pEdges_v;

		double newThreshold(double);
		std::string getColor(Node::state_t) const;

	public:
		~Graph();
		Graph() {};
		Graph(std::string);

		void addNode(length_t, length_t);
 		void removeNodesEdges(pNtr_v);
		void addAllEdges8Directions(unsigned int);
		void addAllEdges4Directions(unsigned int);
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
   			void getShortestPath(unsigned int, unsigned int, std::list<Node*>*, const F&);

		void reconstructPath(std::list<Node*>* vals, Node*, Node*);

		unsigned int getNodeIndexByInternalIndex(unsigned int);

		template<class F>
		void randomDisplaceAllNodes(double, const F&);

		void save(std::string fn) const;

};

#include "graph.getShortestPath.h"

template<class F>
void Graph::randomDisplaceAllNodes(double sigma, const F& dist)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::normal_distribution<> d(0., sigma);
	for (pNode_v_it it = pNodes_v.begin(); it != pNodes_v.end(); it++)
	{
		(*it)->setX((*it)->getX() + d(gen));
		(*it)->setY((*it)->getY() + d(gen));
	}

	for (pEdg_v_it it = pEdges_v.begin(); it != pEdges_v.end(); it++)
		(*it)->setDistance(dist.get((*it)->getFrom(), (*it)->getTo()));
}

#endif
