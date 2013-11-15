#ifndef __node_h__

#define __node_h__

#include <vector>
#include <list>

class Edge;

class Node
{
	private:
		static unsigned int max_index; //kein index wird doppelt vergeben

		unsigned int index;
		length_t xPos;
		length_t yPos;

	public:
		Node(length_t, length_t);
		~Node();

		//allgemeine funktionen
		void setX(length_t);
		void setY(length_t);
		length_t getX() const;
		length_t getY() const;
		unsigned int getIndex() const;

		void print() const;

		//Nachbarn
		typedef std::vector<Edge*> pEdg_v;
		pEdg_v adjEdges;

		void addEdge(Edge*);
		void removeEdge(Edge*);

		//states für shortest Path
		enum state_t {closed, open, inactive, onPath};
		Node* parent; 

		//does it make a difference when those are private with an 
		//additional function (getF / getG / ...)
		state_t status;
		double f; //heuristic cost from start to end through this node
		double g; //effective cost from start to this node
		double h; //heuristic cost from this node to end


		//"nowlater"-list
		Node* next;
		Node* prev;

};

#endif
