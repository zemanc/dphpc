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

	// initialize heuristic functions of start node
	start->g = 0;
	start->h = dist(start, end);
	start->f = start->g + start->h;

	double threshold = start->f; // start with shortest possible path

	// initialize lists whit zero nodes
	// zero nodes are never below threshold (f = UINT_MAX)
	Node* nowlist = new Node(0, 0, UINT_MAX); 
	Node* laterlist= new Node(0, 0, UINT_MAX);

	// insert start into nowlist
	nowlist->next = nowlist->prev = start;
	start->next = start->prev = nowlist;
	
	// laterlist is empty at the beginning
	laterlist->next = laterlist->prev = laterlist;
	
	// define open and later states
	// the definition of these states will change
	// when we increase the threshold (switch lists)
	Node::state_t open_state = Node::open1;
	Node::state_t later_state = Node::open2;

	// nodes in nowlist get state "open"
	// nodes in laterlist get state "later"
	start->status = open_state;
	nowlist->status = open_state;
	laterlist->status = later_state;

	bool found = false; // true if we have reached the end node
	bool not_found = false; // true if we cannot reach the end node

	#pragma omp parallel default(shared)
	{

	// start with the first node
	Node* nl_pos = nowlist->next;
	// this will the node in the later list that the thread has used
	// last - will be important when we increase the threshold and
	// switch the lists (continue with this node in nowlist) 
	Node* last_later_node = laterlist;

	// here the threads have to synchronize again to make sure that
	// no node will get nowlist (zero node) because maybe nowlist->next
	// is already gone
	#pragma omp barrier

#ifdef DEBUG
int nr = omp_get_thread_num();
#endif
	// as long as we haven't reached the end node and the end node
	// is reachable (or we haven't found out that it isn't reachable)
	while (!found && !not_found)
	{

		bool nl_pos_locked = false;

		// when we are here, a node can be:
		//	- closed, if it just has been removed
		//	- open, if everything is alright
		//	- later, if it just has been moved to the laterlist
		// (it cannot be inactive, because then it couldn't be nl_pos)

#ifdef DEBUG2
s.lock();
std::cout << "thread " << nr <<  " tries to get lock on " << nl_pos->getIndex() << " (nl_pos)" << " status: " << nl_pos->status << std::endl;
s.unlock();
#endif
		// try locking the node
		nl_pos_locked = nl_pos->lock.try_lock();

#ifdef DEBUG
s.lock();
if (nl_pos_locked) std::cout << "thread " << nr <<  " locked " << nl_pos->getIndex() << " (nl_pos)" << " status: " << nl_pos->status << std::endl;
s.unlock();
#endif
		if (nl_pos_locked)
		{

			// if we got here we've gotten the lock and we can work
			// with this node
			if (nl_pos->status == open_state && nl_pos-> f <= threshold)
			{
				// this is nice - we are in the nowlist  and < threshold
#ifdef DEBUG2
s.lock();
std::cout << "thread " << nr << " hat einen Node in open_state <= threshold (Node" << nl_pos->getIndex() << ")" << std::endl;
s.unlock();
#endif
				// we are in the nowlist and < threshold
				// there's a lot to do...
					
				// check if we are there yet
				if (nl_pos == end)
				{
					// unlock the node, tell the other threads
					// and exit the while
					nl_pos->lock.unlock();

#ifdef DEBUG
s.lock();
if (nl_pos_locked) std::cout << "thread " << nr << " unlocked " << nl_pos->getIndex() << " (nl_pos)" << " status: " << nl_pos->status << std::endl;
s.unlock();
#endif
					found = true;
					break;
				}
				
				// we set the node to closed already here so that
				// no other thread will waste ressources with
				// waiting on this node 
				nl_pos->status = Node::closed;

				// for each outgoing edge
				for (Node::edges_it_t edge_it = nl_pos->adjEdges.begin(); 
					 edge_it != nl_pos->adjEdges.end(); edge_it++)
				{
					// first is a pointer to the node to which the
					// edge leads 
					Node* edge_to = (*edge_it).first;

					// this node (edge_to) can be
					//	- inactive: we have to add it to the nowlist
					//	- open: we maybe have to update distances
					//	- later: we maybe have to update distances
					//		and get it back to the nowlist
					//	- closed: we won't do anything here
					//		(we never look at closed nodes, they
					//		are "good enough")

					// IMPORTANT! It can also be that a node is just being
					// moved to the laterlist. But then it is marked as
					// closed in order to avoid deadlocks.
					// So it can happen that a node is being moved to
					// the laterlist but there would actually be a better
					// path to this node but we don't update it and the
					// node will still have the old/worse path in the
					// laterlist. However, this cannot be avoided and has
					// close to no influence on the result of the algorithm
					
					if (edge_to->status != Node::closed)
					{
						// it's not closed => we will work with this node
						// => we need a lock to at least check f and
						// maybe update it
						
						// Notice: In the meantime edge_to could have
						// been set to closed but this is not a big issue.
						// In this case we just do nothing and unlock again.

#ifdef DEBUG
s.lock();
std::cout << "thread " << nr << " tries to get lock on " << edge_to->getIndex() << " (edge_to)" << " status: " << edge_to->status << std::endl;
s.unlock();
#endif
						// spin until we were able to lock it or 
						// it has been closed by another thread
						bool edge_to_locked = false;
						while ((edge_to->status != Node::closed)
							&& (!(edge_to_locked = edge_to->lock.try_lock())));

#ifdef DEBUG
s.lock();
if (edge_to_locked) std::cout << "thread " << nr << " locked " << edge_to->getIndex() << " (edge_to)" << " status: " << edge_to->status << std::endl;
s.unlock();
#endif
						// now we can check the different cases
						if (edge_to_locked)
						{
							if (edge_to->status == open_state)
							{
								// the node is open and we got the lock
								
								// if the new distance is better
								double newDist = nl_pos->g + (*edge_it).second;
								if (edge_to->g > newDist)
								{
									// adjust distance and
									// recalculate heuristic values
									edge_to->g =  newDist;
									edge_to->f = newDist + edge_to->h;
									
									// change parent for backtracking
									edge_to->parent = nl_pos;
								}
								// there's no status change
							}
							else if (edge_to->status == Node::inactive)
							{
								// it's a new node we can add to the nowlist

								// calculate distance and heuristics
								edge_to->g = nl_pos->g + (*edge_it).second;
								edge_to->h = dist(edge_to, end);
								edge_to->f = edge_to->g + edge_to->h;

								// set parent for backtracking
								edge_to->parent = nl_pos;

								// insert it right after our node in the
								// nowlist => we need locks on nl_pos and
								// edge_to
								// we already have both locks, so let's go :)
								edge_to->next = nl_pos->next;
								nl_pos->next->prev = edge_to;
								nl_pos->next = edge_to;
								edge_to->prev = nl_pos;

								// edge_to is now in the nowlist...
								edge_to->status = open_state;

							} else if (edge_to->status == later_state)
							{
								// we got the lock and the node is in
								// the laterlist, let's see if we have
								// to do anything...
								// notice: this is the only thread working
								// 	with edge_to

								// if the new distance is better
								double newDist = nl_pos->g + (*edge_it).second;
								if (edge_to->g > newDist)
								{
									// update distance and heuristics
									edge_to->g =  newDist;
									edge_to->f = newDist + edge_to->h;

									// change parent for backtracking
									edge_to->parent = nl_pos;

									// since we adjusted the disctance
									// we will move it back to the nowlist.
									// we will insert it just after our
									// nl_pos. in order to remove it from
									// the later list we first have to lock
									// our predecessor.
									// when we get the lock we first have to
									// check if it is still our predecessor,
									// because it could also have been
									// deleted/moved in the meantime
									bool edge_to_prev_locked = false;
									Node* edge_to_prev;
									do
									{
										// it's the wrong one :(
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
									
									// remove node from laterlist
									edge_to_prev->next = edge_to->next;
									edge_to->next->prev = edge_to_prev;

									// unlock our predecessor
									edge_to_prev->lock.unlock();
#ifdef DEBUG
s.lock();
if (edge_to_prev_locked) std::cout << "thread " << nr << " unlocked " << edge_to_prev->getIndex() << " (edge_to_prev)" << " status: " << edge_to_prev->status << std::endl;
s.unlock();
#endif

									// insert node into nowlist
									edge_to->next = nl_pos->next;
									nl_pos->next->prev = edge_to;
									nl_pos->next = edge_to;
									edge_to->prev = nl_pos;

									// change state to open
									edge_to->status = open_state;
									
								} // END IF (edge_to->g > newDist)
								
							} // END IF (edge_to->status == ??)
							
							// now we can release our lock
							edge_to->lock.unlock();
#ifdef DEBUG
s.lock();
std::cout << "thread " << nr << " unlocked " << edge_to->getIndex() << " (edge_to)" << " status: " << edge_to->status << std::endl;
s.unlock();
#endif
						} // END IF (edge_to_locked)
						
					} // END IF ( (edge_to->status == Node::inactive)
						// || (edge_to->status == later_state))
					
				} // END FOR

				nl_pos->deleted = true;

				// do not release the lock of nl_pos
				// because the node is closed now, we will do this later
#ifdef DEBUG
s.lock();
std::cout << "thread " << nr << " unlocked " << nl_pos->getIndex() << " (nl_pos)" << " status: " << nl_pos->status << std::endl;
s.unlock();
#endif

			}

			if (nl_pos->status == open_state && nl_pos->f > threshold)
			{
				// in this case we are in the nowlist and > threshold
				// this means we have to move the node into the laterlist
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
				// first lock the node's predecessor and make sure it's
				// still its predecessor after we got th lock
				// we will temporary set the node to closed so that no
				// other thread will try to access this node through an
				// edge because this could lead to a deadlock.
				// regarding this see also the comment further above 
				// marked with "IMPORTANT!"
				nl_pos->status = Node::closed;

				bool nl_pos_prev_locked = false;
				Node* nl_pos_prev;
				do
				{
					// it's the wrong one :(
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

				// remove nl_pos from nowlist
				nl_pos_prev->next = nl_pos->next;
				nl_pos->next->prev = nl_pos_prev;

				// unlock the predecessor
				nl_pos_prev->lock.unlock();
#ifdef DEBUG
s.lock();
std::cout << "thread " << nr << " unlocked " << nl_pos_prev->getIndex() << " (nl_pos_prev)" << " status: " << nl_pos_prev->status << std::endl;
s.unlock();
#endif

				// now we have to get a lock on a node in the laterlist
				// we will first try the last_later_node which is the last
				// node this thread worked on in the laterlist.
				// if it's no longer there or if it is locked we will
				// lock the zero node because this one will always be
				// in the laterlist
				// although this looks like it could lead to a jam after
				// the zero node this hasn't proved to be a problem.
				// (however, we were only able to test the code with
				// max. 8 cores running concurrently)
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

				// insert the node into the laterlist
				last_later_node->next->prev = nl_pos;
				nl_pos->next = last_later_node->next;
				last_later_node->next = nl_pos;
				nl_pos->prev = last_later_node;
				
				// change the status to later
				nl_pos->status = later_state;

				last_later_node->lock.unlock();
#ifdef DEBUG
s.lock();
std::cout << "thread " << nr << " unlocked " << last_later_node->getIndex() << " status: " << laterlist->status << std::endl;
s.unlock();
#endif

				// update last_later_node
				// (the node we worked on last in the laterlist)
				last_later_node = nl_pos;

				// and unlock nl_pos since it has been moved now
				nl_pos->lock.unlock();
#ifdef DEBUG
s.lock();
std::cout << "thread " << nr << " unlocked " << nl_pos->getIndex() << " (nl_pos)" << " status: " << nl_pos->status << std::endl;
s.unlock();
#endif

				// let's go to the next node, although we cannot be sure
				// that this is a "good" node
				nl_pos = nl_next;

			} 

			else if (nl_pos->status == Node::closed) {
#ifdef DEBUG2
s.lock();
std::cout << "thread " << nr << " hat einen Node in closed state (Node " << nl_pos->getIndex() << ")" << std::endl;
s.unlock();
#endif
				// if the status has been set to closed we will just
				// unlock the node and go to the next node because the
				// pointer certainly will still be right

				if (nl_pos->deleted)
				{
					// nl_pos is closed but still in the nowlist, 
					// so we can remove it from nowlist
					// it's already locked, so we have to remove its predecessor.
					// also here we have to check if the locked node is
					// still the predecessor of nl_pos

					// nl_pos is locked

					Node* nl_pos_prev = nl_pos->prev;
					bool nl_pos_prev_locked = nl_pos_prev->lock.try_lock();
	#ifdef DEBUG
	s.lock();
	if (nl_pos_prev_locked) std::cout << "thread " << nr << " locked " << nl_pos_prev->getIndex() << " (nl_pos_prev)" << " status: " << nl_pos_prev->status << std::endl;
	s.unlock();
	#endif

					if (nl_pos_prev_locked)
					{
						if (nl_pos->prev == nl_pos_prev)
						{
									
							// now we can remove it
							nl_pos_prev->next = nl_pos->next;
							nl_pos->next->prev = nl_pos_prev;

							nl_pos->deleted = false;

							// now here's a small chance that both lists are empty
							// nowlist can only be empty here because the potentially
							// last node just got removed
							// we first test without locks and then try to lock.
							// if we get both locks we check if it's really empty.
							// it looks a bit complicated but we just try to get
							// both locks on nowlist and laterlist, so that nobody
							// can insert anything in the meantime.
							// if both lists are empty we can be sure that the end
							// node has not been found
							if ((nowlist->next == nowlist) && (laterlist->next == laterlist))
							{
								if (nowlist->lock.try_lock())
								{
									if (laterlist->lock.try_lock())
									{
										// if both lists are empty we are done:
										// we won't stay in the while loop
										// because we cannot reach the end node :(
										if ((nowlist->next == nowlist)
											  &&(laterlist->next == laterlist))
											not_found = true;

										laterlist->lock.unlock();
									}
									nowlist->lock.unlock();
								}
							}

						} 

						// release the lock of the predecessor
						nl_pos_prev->lock.unlock();
					}
	#ifdef DEBUG
	s.lock();
	std::cout << "thread " << nr << " unlocked " << nl_pos_prev->getIndex() << " (nl_pos_prev)" << " status: " << nl_pos_prev->status << std::endl;
	s.unlock();
	#endif
				}

				nl_pos->lock.unlock();
				
#ifdef DEBUG
s.lock();
std::cout << "thread " << nr << " unlocked " << nl_pos->getIndex() << " (nl_pos)" << " status: " << nl_pos->status << std::endl;
s.unlock();
#endif
				nl_pos = nl_pos->next;

			} else if (nl_pos->status == later_state) {
#ifdef DEBUG2
s.lock();
std::cout << nr << " hat einen Node in later state (Node " << nl_pos->getIndex() << ")" << std::endl;
s.unlock();
#endif
				// otherwise we anyway are in the laterlist and therefore
				// only have to release the lock. because we are in the
				// laterlist we will get a new node anyway...
				nl_pos->lock.unlock();
#ifdef DEBUG
s.lock();
std::cout << "thread " << nr << " unlocked " << nl_pos->getIndex() << " (nl_pos)" << " status: " << nl_pos->status << std::endl;
s.unlock();
#endif
			} // END IF (nl_pos->status == ???)

		} else {
			
			// if we are here nl_pos couldn't be locked for some reason,
			// most probably because another thread is working here,
			// so let's just move on...
			// in order to evenly distribute the threads on the list we
			// will not just go to the next node but we will skip some more
			// nodes. the arbitrary value of 150 has proven to provide a 
			// relatively good distribution which results in a faster algorithm
			for (int i = 0; i < 150; i++)
				nl_pos = nl_pos->next;

		} //END IF (nl_pos_locked)

		// if the status is later we have to somehow get our straying
		// thread back on track :)
		// we would like to be in the nowlist, so let's use the zero node.
		// it hasn't been proven to be a problem in terms of performance
		// that all threads use this node as a re-entry node
		// (at least not with max. 8 threads) 
		if (nl_pos->status == later_state)
			nl_pos = nowlist->next;
		else if (nl_pos == nowlist)
			nl_pos = nowlist->next;		// never look at the zero node

		// if the nowlist is empty we will start the next round
		// with a new threshold
		if (nowlist == nowlist->next)
		{
			// let's first wait for all threads
			// they will all get here and realize that the list is empty
			#pragma omp barrier

			// just one thread calculates the new thresholds and
			// swaps the nowlist and the laterlist
			#pragma omp single
			{
				// new threshold
				threshold = newThreshold(threshold);

				//swap
				Node* tmp = nowlist;
				nowlist = laterlist;
				laterlist = tmp;

				// change the definition of the states
				Node::state_t tmps = open_state;
				open_state = later_state;
				later_state = tmps;

			} //IMPLICIT BARRIER

			// and let's move on with the last_later_node.
			// this is the last node each thread worked on in the laterlist
			// that has now become the nowlist.
			// doing this we have the threads already distributed over the
			// list and if we are lucky we still have this node in the cache.
			// if it is no longer there or if it is the zero node we will
			// just use the one after the zero node.
			// also here this might look like it might lead to some jam after
			// the zero node. however, it hasn't been proven to be a problem,
			// at least not with max. 8 threads
			nl_pos = last_later_node;
			last_later_node = laterlist;
			if (nl_pos->status == later_state || nl_pos == nowlist)
				nl_pos = nowlist->next;

			// and let's wait again until all threads are ready
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
	}// END PRAGMA OMP PARALLEL

	// clean up
	delete nowlist;
	delete laterlist;

	// if we have found the end node we are done and can go back home
	// by reconstructing the path. this is nice :)
	if (found)
		return reconstructPath(vals, start, end, dist);
	else
		return -1;	// this is not nice, we weren't able to get to end
}

template<class F>
length_t Graph::reconstructPath(std::list<Node*>* vals, Node* start, Node* end, const F& dist)
{
	// here we will just use the parent pointers to go back to the
	// start node. we will sum up the distance while doing this.
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
