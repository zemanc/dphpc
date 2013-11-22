#ifndef __graph__randomDisplaceAllNodes_h__

#define __graph__randomDisplaceAllNodes_h__

#include <random>

template<class F>
void Graph::randomDisplaceAllNodes(length_t sigma, const F& dist)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::normal_distribution<> d(0., sigma);
	for (pNode_v_it it = pNodes.begin(); it != pNodes.end(); it++)
	{
		(*it)->setX((*it)->getX() + d(gen));
		(*it)->setY((*it)->getY() + d(gen));
	}

	for (pNode_v_it nit = pNodes.begin(); nit != pNodes.end(); nit++)
		for (Node::edges_it_t eit = (*nit)->adjEdges.begin(); 
             eit != (*nit)->adjEdges.end(); eit++)
			(*eit).second = dist(*nit, (*eit).first);

}

#endif
