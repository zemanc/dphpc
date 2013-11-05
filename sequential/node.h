#ifndef __node_h__

#define __node_h__

#include <vector>

class Edge;

class Node
{
	private:
		static unsigned int max_index;
		unsigned int index;
		length_t xPos;
		length_t yPos;

	public:
		typedef std::vector<Edge*> pEdg_v;

		enum state_t {closed, open, inactive, onPath};
		Node* parent;


		pEdg_v adjEdges;

		Node(length_t, length_t);
		~Node();

		length_t getX() const;
		length_t getY() const;
		unsigned int getIndex() const;

		void addEdge(Edge*);
		
		length_t HeurDistanceTo(Node*) const;

		void removeEdge(Edge*);

		//does it make a difference when those are private with an 
		//additional function (getF / getG / ...)
		state_t status;
		double f; //heuristic cost from start to end through this node
		double g; //effective cost from start to this node
		double h; //heuristic cost from this node to end

		void print() const;

};

#endif
