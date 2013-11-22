#ifndef __graph__randomDisplaceAllNodes_h__

#define __graph__randomDisplaceAllNodes_h__

#include <random>

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
		(*it)->setDistance(dist((*it)->getFrom(), (*it)->getTo()));
}

#endif
