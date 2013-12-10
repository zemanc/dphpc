#ifndef __node_h__

#define __node_h__

#include <vector>
#include <list>
#include <utility>

#include "lock.h"

typedef double length_t;
typedef unsigned int index_t;

class Node
{
	private:
		index_t index;
		length_t xPos;
		length_t yPos;

	public:

		typedef std::pair<Node*, length_t> edge_t;
		typedef std::vector<edge_t> edges_t;
		typedef std::vector<edge_t>::iterator edges_it_t;

		lock_t lock;

		Node(length_t, length_t, index_t);
		~Node();

		//allgemeine funktionen
		void setX(length_t x) { xPos = x; };
		void setY(length_t y) { yPos = y; };
		length_t getX() const { return xPos; };
		length_t getY() const { return yPos; };
		index_t getIndex() const { return index; };

		void addEdge(Node*, length_t);
// 		void addEdge(edge_t);
// 		void removeEdge(Node*);

		void print() const;

		//Nachbarn
		edges_t adjEdges;

		//states für shortest Path
		enum state_t {closed, open1, open2, inactive, onPath};
		Node* parent; 
		state_t status;
		length_t f;	//heuristic cost from start to end through this node
		length_t g;	//effective cost from start to this node
		length_t h;	//heuristic cost from this node to end

		//"nowlater"-list
		Node* next;
		Node* prev;
		bool deleted;

};

#endif
