#ifndef __testgraph_genericGraphs_h__

#define __testgraph_genericGraphs_h__

#include <cmath>

struct CircleGraph
{
	
	private: 
		const length_t cp;
		const length_t r;
		const length_t rr;

	public:

		CircleGraph(length_t center, length_t radius) : cp(center)
													  , r (radius)
													  , rr(radius*radius)
			{};

		bool operator()(length_t x1, length_t y1, length_t x2, length_t y2) const
		{
			if (( (x1 - cp) * (x1 - cp) + (y1 - cp) * (y1 - cp) >= rr )
			 && ( (x2 - cp) * (x2 - cp) + (y2 - cp) * (y2 - cp) >= rr ))
				return true;

			return false;
		};
};

#endif
