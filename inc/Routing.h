#ifndef ROUTING_H
#define ROUTING_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <lbsposition.h>

// Forward declarations

class CTrack;
class MRoutingObserver;

// CLASS DECLARATION

/* Calculates route from point A to point B */
class CRouting : public CBase
	{
public:
	// Constructors and destructor

	~CRouting();
	static CRouting* NewL(MRoutingObserver* aObserver);
	static CRouting* NewLC(MRoutingObserver* aObserver);

private:
	CRouting(MRoutingObserver* aObserver);
	void ConstructL();
	
	// New members
private:
	TBool iIsSrcSet, iIsDstSet;
	TCoordinate iSrcCoord, iDstCoord;
	CTrack* iTrack;
	MRoutingObserver* iObserver; // not owned
	
public:
	void Source(TCoordinate& aSrc);
	void SetSource(const TCoordinate& aSrc);
	void Destination(TCoordinate& aDst);
	void SetDestination(const TCoordinate& aDst);
	void FindRoute/*L*/();
	void Reset();
	inline const CTrack* Track() const
		{ return iTrack; };

	};

/* Class for track (array of TCoordinate) */
class CTrack : public CBase
	{
	// Constructor / Destructor
public:
	static CTrack* NewL();
	~CTrack();

private:
	CTrack();
	void ConstructL();

	// New members
private:
	RArray<TCoordinate> iPoints;

public:
	void AddPointL(const TCoordinate& aCoord);
	inline TInt Count() const
		{ return iPoints.Count(); };
//	inline void GetPoint(TCoordinate &aCoord, TInt aIdx) const
//		{ aCoord = iPoints[aIdx]; };
	inline void Reset()
		{ iPoints.Reset(); };

	// Operators override
public:
	const TCoordinate& operator[](TInt anIndex) const;
	
	};


class MRoutingObserver
	{
protected:
	virtual void OnRouteFound() = 0;
	virtual void OnRouteFailedL() = 0;
	
	friend class CRouting;
	};


#endif // ROUTING_H
