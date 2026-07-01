#ifndef ROUTING_H
#define ROUTING_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <lbsposition.h>
#include "HTTPClient2.h"

// Forward declarations

class CTrack;
class MRoutingObserver;
//class MRoutingApiObserver;
class COrsRoutingApi;

// CLASS DECLARATION

class MRoutingApiObserver
	{
protected:
	virtual void OnRoutePointAddedL(const TCoordinate& aCoord) = 0;
	virtual void OnFailedL() = 0;
	
	friend class COrsRoutingApi;
	};


/* Calculates route from point A to point B */
class CRouting : public CBase, public MRoutingApiObserver
	{
public:
	// Constructors and destructor

	~CRouting();
	static CRouting* NewL(MRoutingObserver* aObserver);
	static CRouting* NewLC(MRoutingObserver* aObserver);

private:
	CRouting(MRoutingObserver* aObserver);
	void ConstructL();
	
	// From MRoutingApiObserver
private:
	virtual void OnRoutePointAddedL(const TCoordinate& aCoord);
	virtual void OnFailedL();
	
	// New members
private:
	TBool iIsSrcSet, iIsDstSet;
	TCoordinate iSrcCoord, iDstCoord;
	CTrack* iTrack;
	MRoutingObserver* iObserver; // not owned
	COrsRoutingApi* iApi;
	
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


/* Retrieves route using api.openrouteservice.org */
class COrsRoutingApi : public CBase
	{
	// Constructor / Destructor
public:
	static COrsRoutingApi* NewL(MRoutingApiObserver* aObserver);
	~COrsRoutingApi();

private:
	COrsRoutingApi(MRoutingApiObserver* aObserver);
	void ConstructL();

	// New members
private:
	MRoutingApiObserver* iObserver; // not owned
	//CHTTPClient2* iHttpClient;

public:
	void SendRequestL(const TCoordinate& aSrc, const TCoordinate& aDst);

	};

//class MRoutingApiObserver
//	{
//protected:
//	virtual void OnRoutePointAddedL(const TCoordinate& aCoord) = 0;
//	virtual void OnFailedL() = 0;
//	};



#endif // ROUTING_H
