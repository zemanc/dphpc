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

typedef std::set<unsigned int> pNtr_v;
typedef pNtr_v::iterator pNtr_v_it;
typedef std::vector<Edge*> pEdg_v;
typedef pEdg_v::iterator pEdg_v_it;
typedef std::vector<Node*> pNode_v;
typedef pNode_v::iterator pNode_v_it;

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

};

template<class F>
void Graph::getShortestPath(unsigned int from, unsigned int to, std::list<Node*>* vals, const F& dist)
{

	//geht besser so als randomized... für den Moment. Später ist das wohl dann der "richtige" Index, nicht der interne!
	Node* start = pNodes_v[from];
	Node* end = pNodes_v[to];

	//start heuristische funktion initialisieren
	start->g = 0;
	start->h = dist.get(start, end);
	start->f = start->g + start->h;

	double threshold = start->f; //start with shortest possible paths

	//init list
	Node* zero_node = new Node(0, 0);
	zero_node->prev = zero_node;
	zero_node->next = zero_node;

	//insert start into list
	zero_node->next = zero_node->prev = start;
	start->next = start->prev = zero_node;

	//solange nicht leer
	while (zero_node->next != zero_node)
	{
		//beginne bei erstem Node (nach zero_node)
		Node* nl_pos = zero_node->next;

		//wenn dieser wieder das zero_node ist, dann ist die Liste leer!
		while (nl_pos != zero_node)
		{
			//nur bearbeiten, wenn unter threshold. zero_node (f=0) ist ausgeschlossen
			if (nl_pos->f <= threshold)
			{
				//wenn fertig dann fertig
				if (nl_pos == end)
					return reconstructPath(vals, start, end);
					
				//node schliessen
				nl_pos->status = Node::closed;

				//for each neighbor
				for (Node::pEdg_v::iterator edge_it = nl_pos->adjEdges.begin(); 
					 edge_it != nl_pos->adjEdges.end(); edge_it++)
				{
					Node* edge_to = (*edge_it)->getTo();

					if (edge_to->status == Node::inactive)
					{
						//neuer Node!

						//berechne heuristische Beträge
						edge_to->g = nl_pos->g + (*edge_it)->getDistance();
						edge_to->h = dist.get(edge_to, end);
						edge_to->f = edge_to->g + edge_to->h;

						//für backtracking, damit wir wissen woher wir gekommen sind
						edge_to->parent = nl_pos;

						//direkt nachher einfügen
						edge_to->next = nl_pos->next;
						nl_pos->next->prev = edge_to;
						nl_pos->next = edge_to;
						edge_to->prev = nl_pos;

						edge_to->status = Node::open;

					} else if (edge_to->status == Node::open) {
						
						//wenn distanz besser ist
						double newDist = nl_pos->g + (*edge_it)->getDistance();
						if (edge_to->g > newDist)
						{
							//heuristische Beträge berechnen
							edge_to->g =  newDist;
							edge_to->f = newDist + edge_to->h;

							//backtracking setzen
							edge_to->parent = nl_pos;

							//edge_to verschieben!
							//remove 
							edge_to->prev->next = edge_to->next;
							edge_to->next->prev = edge_to->prev;

							//insert
							edge_to->next = nl_pos->next;
							nl_pos->next->prev = edge_to;
							nl_pos->next = edge_to;
							edge_to->prev = nl_pos;
						}
					}
						
				}
				//lösche nl_pos und setze auf das nächste
				Node* nl_next = nl_pos->next;
				nl_pos->prev->next = nl_pos->next;
				nl_pos->next->prev = nl_pos->prev;
				nl_pos->prev = NULL;
				nl_pos->next = NULL;
				nl_pos = nl_next;

			} else {
				//Sonst überspringe
				nl_pos = nl_pos->next;
			}
		}
		//Neuer threshold berechnen
		threshold = newThreshold(threshold);

	}
	delete zero_node;
}

#endif
