#ifndef __node_h__

#define __node_h__

#include <vector>
#include <list>

class Edge;

class Node
{
	private:
		unsigned int index;
		length_t xPos;
		length_t yPos;

	public:
		static unsigned int max_index; //kein index wird doppelt vergeben

		Node(length_t, length_t);
		~Node();

		//allgemeine funktionen
		void setX(length_t);
		void setY(length_t);
		length_t getX() const;
		length_t getY() const;
		unsigned int getIndex() const;
		void removeEdge(Edge*);

		void print() const;

		//Nachbarn
		typedef std::vector<Edge*> pEdg_v;
		pEdg_v adjEdges;

		void addEdge(Edge*);

		//states für shortest Path
		enum state_t {closed, open, inactive, onPath};
		Node* parent; 
		state_t status;
		length_t f;	//heuristic cost from start to end through this node
		length_t g;	//effective cost from start to this node
		length_t h;	//heuristic cost from this node to end

		//"nowlater"-list
		Node* next;
		Node* prev;

};

#endif
