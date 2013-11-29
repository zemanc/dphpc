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

struct HolyGraph
{
	private:
		
		const length_t hb1up;	// horizontal bar 1 upper border
		const length_t hb1lo;	// horizontal bar 1 lower border
		const length_t hb1ri;	// horizontal bar 1 right border
		const length_t vb1le;	// vertical bar 1 left border
		const length_t vb1ri;	// vertical bar 1 right border
		const length_t vb1up;	// vertical bar 1 upper border
		const length_t vb1lo;	// vertical bar 1 lower border
		const length_t hb2up;	// horizontal bar 2 upper border
		const length_t hb2lo;	// horizontal bar 2 lower border
		const length_t hb2le;	// horizontal bar 2 left border
		const length_t vb2le;	// vertical bar 2 left border
		const length_t vb2ri;	// vertical bar 2 right border
		const length_t vb2up;	// vertical bar 2 upper border
		const length_t vb2lo;	// vertical bar 2 lower border

	public:

		HolyGraph( length_t n )	: hb1up(2*n/10), hb1lo(3*n/10), hb1ri(9*n/10)
								, vb1le(6*n/10), vb1ri(7*n/10)
								, vb1up(1*n/10), vb1lo(5*n/10)
								, hb2up(7*n/10), hb2lo(8*n/10), hb2le(1*n/10)
								, vb2le(3*n/10), vb2ri(4*n/10)
								, vb2up(5*n/10), vb2lo(9*n/10)
			{};

	bool operator()(length_t x1, length_t y1, length_t x2, length_t y2) const
	{
		if (	( (x1 > hb1ri || y1 < hb1up || y1 > hb1lo)
				&& (x2 > hb1ri || y2 < hb1up || y2 > hb1lo) )
			&& 	( (x1 < hb2le || y1 < hb2up || y1 > hb2lo)
				&& (x2 < hb2le || y2 < hb2up || y2 > hb2lo) )
			&&	( (x1 < vb1le || x1 > vb1ri || y1 < vb1up || y1 > vb1lo)
				&& (x2 < vb1le || x2 > vb1ri || y2 < vb1up || y2 > vb1lo) )
			&&	( (x1 < vb2le || x1 > vb2ri || y1 < vb2up || y1 > vb2lo)
				&& (x2 < vb2le || x2 > vb2ri || y2 < vb2up || y2 > vb2lo) )
		)
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
		const length_t ep2;		// eye position 2
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
