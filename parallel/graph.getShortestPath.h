#ifndef __graph__getShortestPath_h__

#define __graph__getShortestPath_h__

// #define DEBUG
#include <climits>

#include <iostream>
#include "lock.h"

template<class F>
length_t Graph::getShortestPath(index_t from, index_t to, std::list<Node*>* vals, const F& dist)
{
#ifdef DEBUG
lock_t s;
#endif

	Node* start = pNodes[from];
	Node* end = pNodes[to];

	//start heuristische Funktion initialisieren
	start->g = 0;
	start->h = dist(start, end);
	start->f = start->g + start->h;

	double threshold = start->f; //start with shortest possible paths

	//init list
	Node* nowlist = new Node(0, 0, UINT_MAX); 
	Node* laterlist= new Node(0, 0, UINT_MAX); //kein Node <= Threshold!!

	//insert start into nowlist
	nowlist->next = nowlist->prev = start;
	start->next = start->prev = nowlist;
	
	//laterlist leer
	laterlist->next = laterlist->prev = laterlist;
	
	//definiere erster open_state
	Node::state_t open_state = Node::open1;
	Node::state_t later_state = Node::open2;

	//start und nowlist sind open, laterlist ist later
	start->status = open_state;
	nowlist->status = open_state;
	laterlist->status = later_state;

	bool found = false; //wenn found=true, dann haben wir das Ziel gefunden
	bool not_found = false; //wenn not_found=true, dann ist die Suche zwecklos

	#pragma omp parallel default(shared)
	{

	//beginne bei erstem Node, diese Liste ist sicher nicht leer
	Node* nl_pos = nowlist->next;
	Node* last_later_node = laterlist;

	//hier müssen wir nochmals aufeinander warten, da sonst ein Node z.B.
	//nowlist kriegen könnte, da nowlist->next bereits weg ist
	#pragma omp barrier
	int nr = omp_get_thread_num();

	//wir können hier nicht einfach testen, ob die Listen beide leer sind
	//denn während wir dies testen, kann sich das schon wieder ändern!
	//deshalb machen wir diese Kontrolle woanders
	while (!found && !not_found)
	{

		bool nl_pos_locked = false;

		//wenn wir hier sind, kann der Node
		//- closed sein, falls er gerade eben entfernt wurde
		//- open sein, wenn alles gut ist
		//- later sein, wenn er soeben verschoben wurde
		//Er kann nicht inactive sein - er käme als nl_pos nicht in Frage!

#ifdef DEBUG2
s.lock();
std::cout << "thread " << nr <<  " tries to get lock on " << nl_pos->getIndex() << " (nl_pos)" << " status: " << nl_pos->status << std::endl;
s.unlock();
#endif
		nl_pos_locked = nl_pos->lock.try_lock();

#ifdef DEBUG
s.lock();
if (nl_pos_locked) std::cout << "thread " << nr <<  " locked " << nl_pos->getIndex() << " (nl_pos)" << " status: " << nl_pos->status << std::endl;
s.unlock();
#endif
		if (nl_pos_locked)
		{

			//wenn wir hierhin kommen, haben wir also einen Lock gekriegt auf einen Node, den
			//wir nun definitiv bearbeiten

			if (nl_pos->status == open_state)
			{
				//Nun ist das der "gute Fall"! Wir sind in der Nowlist, diesen Node müssen wir bearbeiten

				//nun kann entweder > threshold oder <=threshold sein
				if (nl_pos->f <= threshold)
				{

#ifdef DEBUG2
s.lock();
std::cout << "thread " << nr << " hat einen Node in open_state <= threshold (Node" << nl_pos->getIndex() << ")" << std::endl;
s.unlock();
#endif
					//in diesem Fall müssen wir vieles machen...
						
					//wenn fertig dann fertig
					if (nl_pos == end)
					{
						//hier müssen wir noch den einen lock freigeben!
						nl_pos->lock.unlock();

#ifdef DEBUG
s.lock();
if (nl_pos_locked) std::cout << "thread " << nr << " unlocked " << nl_pos->getIndex() << " (nl_pos)" << " status: " << nl_pos->status << std::endl;
s.unlock();
#endif

						//die anderen Threads sollten auch benachrichtigt werden...
						found = true;
						break;
					}
					
					//für jede wegführende Kante
					for (Node::edges_it_t edge_it = nl_pos->adjEdges.begin(); 
						 edge_it != nl_pos->adjEdges.end(); edge_it++)
					{
						//.first ist der Node, wo die Kante edge_it hinführt
						Node* edge_to = (*edge_it).first;

						//edge_to kann 
						// - inactive sein, dann müssen wir ihn in die Nowlist einfügen
						// - open sein, dann passen wir den Wert an
						// - later sein, dann müssen wir ihn evtl. zurückholen
						// - closed sein, dann machen wir nichts

						//closed dürfen wir von Beginn weg ausschliessen, denn die kommen
						//eh nie zurück :-((
					
						//BEACHTE! Es kann auch sein, dass der Node gerade in die Laterlist verschoben wird
						//dann ist aber der Node als closed gekennzeichnet, also kann es zu keinem 
						//Deadlock kommen! Was aber passieren kann, ist, dass der Node gerade verschoben wird
						//wir aber in diesem Moment merken, dass eigentlich ein besserer Pfad existieren 
						//würde. In diesem Fall ändern wir das nicht mehr, der Node landet mit der falschen
						//Schätzung in der Laterlist. Das können wir aber nicht ändern und macht fast nichts aus

						//node schliessen, das machen wir bereits hier, damit niemand Ressourcen verschwendet
						//auf diesen Node zu warten
						nl_pos->status = Node::closed;
						
						if (edge_to->status != Node::closed)
						{
							//nun brauchen wir sicher mal den lock, da wir in jedem Fall
							//f vergleichen und evtl. anpassen müssen
							//beachte: edge_to kann trotzdem plötzlich closed oder now_state werden!
							//das ist aber nicht tragisch, denn FALLS wir den lock bekommen, geben wir
							//ihn einfach unverrichteter Dinge wieder zurück

#ifdef DEBUG
s.lock();
std::cout << "thread " << nr << " tries to get lock on " << edge_to->getIndex() << " (edge_to)" << " status: " << edge_to->status << std::endl;
s.unlock();
#endif
							bool edge_to_locked = false;
							while ((edge_to->status != Node::closed)
								&& (!(edge_to_locked = edge_to->lock.try_lock())));
#ifdef DEBUG
s.lock();
if (edge_to_locked) std::cout << "thread " << nr << " locked " << edge_to->getIndex() << " (edge_to)" << " status: " << edge_to->status << std::endl;
s.unlock();
#endif

							//nun dürfen wir definitiv die Fallunterscheidung machen
							if (edge_to_locked)
							{
								if (edge_to->status == open_state)
								{
									//wenn der Node nun open ist und wir den Lock bekommen haben
									//dann können wir anpassen

									//wenn distanz besser ist
									double newDist = nl_pos->g + (*edge_it).second;
									if (edge_to->g > newDist)
									{
										//heuristische Beträge neu berechnen
										edge_to->g =  newDist;
										edge_to->f = newDist + edge_to->h;

										//backtracking anpassen
										edge_to->parent = nl_pos;
									}

									//keine Statusänderung
								}
								else if (edge_to->status == Node::inactive)
								{
									//neuer Node! Also einfach einfügen

									//berechne heuristische Beträge
									edge_to->g = nl_pos->g + (*edge_it).second;
									edge_to->h = dist(edge_to, end);
									edge_to->f = edge_to->g + edge_to->h;

									//für backtracking, damit wir wissen woher wir gekommen sind
									edge_to->parent = nl_pos;

									//direkt nachher einfügen
									//dazu benötigen wir die locks edge_to und nl_pos, 
									//die haben wir ja aber beide schon! also loooos!
									edge_to->next = nl_pos->next;
									nl_pos->next->prev = edge_to;
									nl_pos->next = edge_to;
									edge_to->prev = nl_pos;

									//edge_to ist nun in der openlist...
									edge_to->status = open_state;

								} else if (edge_to->status == later_state)
								{

									//nun sind wir im later_state, da schauen wir zuerst mal
									//ob überhaupt handlungsbedarf besteht ...
									//beachte: wir sind die einzigen, die an edge_to arbeiten!

									//wenn distanz besser ist
									double newDist = nl_pos->g + (*edge_it).second;
									if (edge_to->g > newDist)
									{
										//heuristische Beträge berechnen
										edge_to->g =  newDist;
										edge_to->f = newDist + edge_to->h;

										//backtracking anpassen
										edge_to->parent = nl_pos;

										//edge_to kommt nun wieder zurück in die now_list!
										//wir fügen es direkt nach nl_pos ein
										//zum entfernen aus der later_list müssen wir noch
										//den Vorgänger locken!
										//nachdem wir einen lock kriegen, müssen wir prüfen,
										//ob das wirklich der Vorgänger ist, denn dieser 
										//könnte auch gerade noch gelöscht worden sein
										//bzw. zurücktransferiert wordens ein

										bool edge_to_prev_locked = false;
										Node* edge_to_prev;
										do
										{
											//nun haben wir wohl den falschen gekriegt :)
											if (edge_to_prev_locked)
												edge_to_prev->lock.unlock();

											edge_to_prev = edge_to->prev;
											edge_to_prev_locked = edge_to_prev->lock.try_lock();
										} while ((!edge_to_prev_locked)
											  || (edge_to->prev != edge_to_prev));
#ifdef DEBUG
s.lock();
if (edge_to_prev_locked) std::cout << "thread " << nr << " locked " << edge_to_prev->getIndex() << " (edge_to_prev)" << " status: " << edge_to_prev->status << std::endl;
s.unlock();
#endif

										//Nun können wir ja endlich handeln, also entfernen, einfügen

										//remove 
										edge_to->prev->next = edge_to->next;
										edge_to->next->prev = edge_to->prev;

										//edge_to_prev können wir wieder freigeben!
										edge_to_prev->lock.unlock();
#ifdef DEBUG
s.lock();
if (edge_to_prev_locked) std::cout << "thread " << nr << " unlocked " << edge_to_prev->getIndex() << " (edge_to_prev)" << " status: " << edge_to_prev->status << std::endl;
s.unlock();
#endif

										//insert
										edge_to->next = nl_pos->next;
										nl_pos->next->prev = edge_to;
										nl_pos->next = edge_to;
										edge_to->prev = nl_pos;

										//nun haben wir edge_to in nowlist eingefügt
										edge_to->status = open_state;
										
									} // END IF (edge_to->g > newDist)
									
								} //END IF (edge_to->status == ??)
								
								//nun wieder unlocken
								edge_to->lock.unlock();
#ifdef DEBUG
s.lock();
std::cout << "thread " << nr << " unlocked " << edge_to->getIndex() << " (edge_to)" << " status: " << edge_to->status << std::endl;
s.unlock();
#endif
							} //END IF (edge_to_locked)
							
						} // END IF ( (edge_to->status == Node::inactive) || (edge_to->status == later_state))
						
					} //END FOR

					//lösche nl_pos aus nowlist
					//nl_pos ist schon gelockt, also müssen wir noch den Vorgänger locken

					//auch hier müssen wir nach dem locken immer schauen, ob wir wirklich
					//den richtigen hatten
					bool nl_pos_prev_locked = false;
					Node* nl_pos_prev;
					do
					{
						//nun haben wir wohl den falschen gekriegt :)
						if (nl_pos_prev_locked)
							nl_pos_prev->lock.unlock();

						nl_pos_prev = nl_pos->prev;

						nl_pos_prev_locked = nl_pos_prev->lock.try_lock();

					} while ((!nl_pos_prev_locked)
						  || (nl_pos->prev != nl_pos_prev));
	#ifdef DEBUG
	s.lock();
	if (nl_pos_prev_locked) std::cout << "thread " << nr << " locked " << nl_pos_prev->getIndex() << " (nl_pos_prev)" << " status: " << nl_pos_prev->status << std::endl;
	s.unlock();
	#endif

					//nun dürfen wir löschen ...
					nl_pos->prev->next = nl_pos->next;
					nl_pos->next->prev = nl_pos->prev;
	// 				nl_pos->prev = NULL;
	// 				nl_pos->next = NULL; //dies legen wir natürlich nicht fest

					//den lock müssen wir natürlich wieder freigeben :)
					nl_pos_prev->lock.unlock();
	#ifdef DEBUG
	s.lock();
	std::cout << "thread " << nr << " unlocked " << nl_pos_prev->getIndex() << " (nl_pos_prev)" << " status: " << nl_pos_prev->status << std::endl;
	s.unlock();
	#endif

					//nl_pos muss auch mal frei werden :)
					nl_pos->lock.unlock();
	#ifdef DEBUG
	s.lock();
	std::cout << "thread " << nr << " unlocked " << nl_pos->getIndex() << " (nl_pos)" << " status: " << nl_pos->status << std::endl;
	s.unlock();
	#endif


					//nun gehen wir mal zum nächsten
					nl_pos = nl_pos->next;

					//hier besteht nun eine kleine Chance, dass beide Listen leer sind
					//deshalb überprüfen wir das genau hier! nowlist kann nur hier
					//leer werden, da der letzte Node in der nowlist hier entfernt
					//wird!
					//wir testen zuerst ohne Locks, dann versuchen wir zu locken,
					//und wenn wir beide kriegen, schauen wir, ob wir recht haben.
					//das ganze sieht recht komplex aus, aber wir versuchen hier beide Locks auf
					//Nowlist und laterlist zu kriegen, dann kann niemand gerade versuchen
					//dort was einzufügen, deshalb können wir uns über not_found sicher sein
					if ((nowlist->next == nowlist) && (laterlist->next == laterlist))
					{
						if (nowlist->lock.try_lock())
						{
							if (laterlist->lock.try_lock())
							{
								//wenn es nun noch zutrifft, dann sind wir definitiv
								//am Ende!
								if ((nowlist->next == nowlist)
									  &&(laterlist->next == laterlist))
									not_found = true;

								laterlist->lock.unlock();
							}
							nowlist->lock.unlock();
						}
					}

				} else {
					//In diesem Fall sind wir in der open_list und > threshold
					//d.h. in later_list kopieren
#ifdef DEBUG2
s.lock();
std::cout << "thread " << nr << " hat einen Node in open_state > threshold (Node" << nl_pos->getIndex() << ")" << std::endl;
s.unlock();
#endif

#ifdef DEBUG
s.lock();
std::cout << "thread " << nr << " tries to get lock on " << nl_pos->prev->getIndex() << " (nl_pos_prev)" << " status: " << nl_pos->prev->status << std::endl;
s.unlock();
#endif

					//locke zuerst Vorgänger, dieser kann nie nach hinten geschoben werden
					//Wir setzen ihn temporär auf closed, damit niemand zur gleichen Zeit
					//eine Kante zu diesem Node hat (edge_to). dies würde zu einem
					//deadlock führen. Oben genauer beschrieben mit BEACHTE!
					nl_pos->status = Node::closed;

					bool nl_pos_prev_locked = false;
					Node* nl_pos_prev;
					do
					{
						//nun haben wir wohl den falschen gekriegt :)
						if (nl_pos_prev_locked)
							nl_pos_prev->lock.unlock();

						nl_pos_prev = nl_pos->prev;

						nl_pos_prev_locked = nl_pos_prev->lock.try_lock();

					} while ((!nl_pos_prev_locked) 
						 || (nl_pos->prev != nl_pos_prev));
#ifdef DEBUG
s.lock();
std::cout << "thread " << nr << " locked " << nl_pos_prev->getIndex() << " (nl_pos_prev)" << " status: " << nl_pos_prev->status << std::endl;
s.unlock();
#endif
					Node* nl_next = nl_pos->next;

					//nl_pos entfernen
					nl_pos->prev->next = nl_pos->next;
					nl_pos->next->prev = nl_pos->prev;

					//und schon wieder freigeben :)
					nl_pos->prev->lock.unlock();
#ifdef DEBUG
s.lock();
std::cout << "thread " << nr << " unlocked " << nl_pos_prev->getIndex() << " (nl_pos_prev)" << " status: " << nl_pos_prev->status << std::endl;
s.unlock();
#endif

					//nun müssen wir noch den lock auf die laterlist kriegen
					//da laterlist immer in der laterlist bleibt, ist das ganz
					//unkompliziert, aber eigentlich trotzdem katastrophal :)

					if (last_later_node->status == later_state)
					{
						if (last_later_node->lock.try_lock())
						{
							if (last_later_node->status != later_state)
							{
								last_later_node->lock.unlock();
								last_later_node = laterlist;
								last_later_node->lock.lock();
							}
						} else {
							last_later_node = laterlist;
							last_later_node->lock.lock();
						}
					} else {
						last_later_node = laterlist;
						last_later_node->lock.lock();
					}
#ifdef DEBUG
s.lock();
std::cout << "thread " << nr << " locked " << last_later_node->getIndex() << " status: " << laterlist->status << std::endl;
s.unlock();
#endif

					//einfügen
					last_later_node->next->prev = nl_pos;
					nl_pos->next = last_later_node->next;
					last_later_node->next = nl_pos;
					nl_pos->prev = last_later_node;
					
					//den Status nicht vergessen anzupassen!
					nl_pos->status = later_state;

					last_later_node->lock.unlock();
#ifdef DEBUG
s.lock();
std::cout << "thread " << nr << " unlocked " << last_later_node->getIndex() << " status: " << laterlist->status << std::endl;
s.unlock();
#endif

					last_later_node = nl_pos;

					//nl_pos muss noch freigegeben werden
					nl_pos->lock.unlock();
#ifdef DEBUG
s.lock();
std::cout << "thread " << nr << " unlocked " << nl_pos->getIndex() << " (nl_pos)" << " status: " << nl_pos->status << std::endl;
s.unlock();
#endif

					//setze auf nächstes, wobei wir natürlich nicht sagen können, ob das was gutes ist ...
					nl_pos = nl_next;
				
				}
					
			} else if (nl_pos->status == Node::closed) {
#ifdef DEBUG2
s.lock();
std::cout << "thread " << nr << " hat einen Node in closed state (Node " << nl_pos->getIndex() << ")" << std::endl;
s.unlock();
#endif
				//nun kann der Status halt auf closed gewechselt haben, dann gehen
				//wir einfach weiter, denn der Pointer stimmt noch
				
				nl_pos->lock.unlock();
#ifdef DEBUG
s.lock();
std::cout << "thread " << nr << " unlocked " << nl_pos->getIndex() << " (nl_pos)" << " status: " << nl_pos->status << std::endl;
s.unlock();
#endif
				nl_pos = nl_pos->next;

			} else {
#ifdef DEBUG2
s.lock();
std::cout << nr << " hat einen Node in later state (Node " << nl_pos->getIndex() << ")" << std::endl;
s.unlock();
#endif
				//sonst sind wir eh in der later-list gelandet, dann müssen wir nur
				//noch den lock freigeben, da wir in der laterlist sind, kriegen wir
				//eh einen neuen Node...
				nl_pos->lock.unlock();
#ifdef DEBUG
s.lock();
std::cout << "thread " << nr << " unlocked " << nl_pos->getIndex() << " (nl_pos)" << " status: " << nl_pos->status << std::endl;
s.unlock();
#endif
			} // END IF (nl_pos->status == ???)

		} else {
			
			//wenn wir hier sind, wurde also nl_pos aus irgendeinem Grund nicht
			//gelockt, vermutlich, weil grad wer anders an so einem Ort probiert
			//also einfach weitergehen...
			//int n = rand() % 5 + 1;
			for (int i = 0; i <= 5; i++)
				nl_pos = nl_pos->next;

		} //END IF (nl_pos_locked)

		//nun wenn der status later ist, dann müssen wir unseren Core irgendwie
		//wieder auf den richtigen Weg zurückführen... ABTRÜNNIGER!!;)
		if (nl_pos->status == later_state)
			nl_pos = nowlist->next;
		else if (nl_pos == nowlist)
			nl_pos = nowlist->next;

		//wenn nowlist leer, dann nächster Durchlauf
		if (nowlist == nowlist->next)
		{
			//dazu müssen wir mal auf die anderen warten, denn das sollten ja alle
			//merken
			#pragma omp barrier

			//ein einziger berechnet den shared threshold und ändert die Listen
			#pragma omp single
			{
				threshold = newThreshold(threshold);

				//swap
				Node* tmp = nowlist;
				nowlist = laterlist;
				laterlist = tmp;

				//zudem müssen wir noch die states wechseln
				Node::state_t tmps = open_state;
				open_state = later_state;
				later_state = tmps;

			} //IMPLICIT BARRIER

			//und wir beginnen nun wieder von vorne
			nl_pos = last_later_node;
			last_later_node = laterlist;
			if (nl_pos->status == later_state || nl_pos == nowlist)
				nl_pos = nowlist->next;

			//warte nochmals auf alle
			#pragma omp barrier

		} // END IF (nowlist == nowlist->next)

#ifdef DEBUG
s.lock();
std::cout << std::endl;
s.unlock();
#endif


	} // END WHILE

#ifdef DEBUG
s.lock();
std::cout << "I'm thread " << nr << " and I got out of the while! Shame on me!" << std::endl;
s.unlock();
#endif
	}//END PRAGMA OMP PARALLEL

	//bisschen aufräumen
	delete nowlist;
	delete laterlist;

	//und nun bin ich müde und will zurück :)
	if (found)
		return reconstructPath(vals, start, end, dist);
	else
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
