#ifndef __graph__getShortestPath_h__

#define __graph__getShortestPath_h__

template<class F>
length_t Graph::getShortestPath(index_t from, index_t to, std::list<Node*>* vals, const F& dist)
{

	//geht besser so als randomized... für den Moment. Später ist das wohl dann der "richtige" Index, nicht der interne!
	Node* start = pNodes[from];
	Node* end = pNodes[to];

	//start heuristische funktion initialisieren
	start->g = 0;
	start->h = dist(start, end);
	start->f = start->g + start->h;

	double threshold = start->f; //start with shortest possible paths

	//init list
	Node* zero_node = new Node(0, 0, 0);
// 	zero_node->prev = zero_node;
// 	zero_node->next = zero_node;

	//insert start into list
	zero_node->next = zero_node->prev = start;
	start->next = start->prev = zero_node;

	//beginne bei erstem Node
	Node* nl_pos = zero_node->next;

	//wenn dieser wieder das zero_node ist, dann ist die Liste leer!
	while (nl_pos != zero_node)
	{
		//nur bearbeiten, wenn unter threshold. zero_node (f=0) ist ausgeschlossen
		if (nl_pos->f <= threshold)
		{
			//wenn fertig dann fertig
			if (nl_pos == end)
			{
				delete zero_node;
				return reconstructPath(vals, start, end, dist);
			}
				
			//node schliessen
			nl_pos->status = Node::closed;

			//for each neighbor
			for (Node::edges_it_t edge_it = nl_pos->adjEdges.begin(); 
				 edge_it != nl_pos->adjEdges.end(); edge_it++)
			{
				Node* edge_to = (*edge_it).first;

				if (edge_to->status == Node::inactive)
				{
					//neuer Node!

					//berechne heuristische Beträge
					edge_to->g = nl_pos->g + (*edge_it).second;
					edge_to->h = dist(edge_to, end);
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
					double newDist = nl_pos->g + (*edge_it).second;
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

		//dann sind wir am Ende der Liste und müssen von vorne beginnen
		if (nl_pos == zero_node)
		{
			//neuer Threshold berechnen
			threshold = newThreshold(threshold);

			//beginne bei erstem Node
			nl_pos = zero_node->next;
		}

	}

	delete zero_node;

	return -1;
}

template<class F>
length_t Graph::reconstructPath(std::list<Node*>* vals, Node* start, Node* end, const F& dist)
{

	double length = 0;

	Node* it = end;
	while (it != start)
	{
		length += dist(it, it->parent);

		vals->push_front(it);
		it->status = Node::onPath;
		it = it->parent;

	}

	vals->push_front(start);

	start->status = Node::onPath;

	return length;
}

#endif
