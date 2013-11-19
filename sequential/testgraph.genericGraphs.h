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


struct SmileyGraph
{
	private:

		const length_t sr;		// smiley radius
		const length_t srr;		// smiley radius ^2
		const length_t cp;		// center
		const length_t er;		// eye radius
		const length_t err;		// eye radius ^2
		const length_t ep1;		// eye position 1
		const length_t ep2;		// eype position 2
		const length_t mupb;	// mouth upper border
		const length_t mlob;	// mouth lower border
		const length_t mleb;	// mouth left border
		const length_t mrib;	// mouth right border

	public:

		SmileyGraph( length_t radius )	: sr(radius)
										, srr(radius*radius)
										, cp(radius)
										, er(radius/4)
										, err(radius/4*radius/4)
										, ep1(radius*2/3)
										, ep2(radius*4/3)
										, mupb(radius*4/3)
										, mlob(radius*4/3+radius/4)
										, mleb(radius/2)
										, mrib(radius/2+radius)
			{};

		bool operator()(length_t x1, length_t y1, length_t x2, length_t y2) const
		{
			if ((( (x1 - cp) * (x1 - cp) + (y1 - cp) * (y1 - cp) <= srr )
				 && ( (x2 - cp) * (x2 - cp) + (y2 - cp) * (y2 - cp) <= srr ))
			&&	(( (x1 - ep1) * (x1 - ep1) + (y1 - ep1) * (y1 - ep1) >= err )
				&& ( (x2 - ep1) * (x2 - ep1) + (y2 - ep1) * (y2 - ep1) >= err ))
			&&	(( (x1 - ep2) * (x1 - ep2) + (y1 - ep1) * (y1 - ep1) >= err )
				&& ( (x2 - ep2) * (x2 - ep2) + (y2 - ep1) * (y2 - ep1) >= err ))
			&&	(  ( (y1 < mupb) || (y1 > mlob) )
				|| ( (y2 < mupb) || (y2 > mlob) )
				|| ( (x1 < mleb) || (x1 > mrib) )
				|| ( (x2 < mleb) || (x2 > mrib) ) )
			)
				return true;

			return false;
		};

};

#endif
