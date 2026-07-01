/*
 ============================================================================
 Name		: Routing.cpp
 Author	  : 
 Version	 : 
 Copyright   : 
 Description : CRouting implementation
 ============================================================================
 */

#include "Routing.h"
//////////
#include <e32math.h>
/////////
#include "S60MapsAppUi.h"

CRouting::CRouting(MRoutingObserver* aObserver) :
		iIsSrcSet(EFalse),
		iIsDstSet(EFalse),
		iObserver(aObserver)
	{
	}

CRouting::~CRouting()
	{
	delete iApi;
	delete iTrack;
	}

CRouting* CRouting::NewLC(MRoutingObserver* aObserver)
	{
	CRouting* self = new (ELeave) CRouting(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CRouting* CRouting::NewL(MRoutingObserver* aObserver)
	{
	CRouting* self = CRouting::NewLC(aObserver);
	CleanupStack::Pop(); // self;
	return self;
	}

void CRouting::ConstructL()
	{
	iTrack = CTrack::NewL();
	iApi = COrsRoutingApi::NewL(this);
	}

void CRouting::FindRoute/*L*/()
	{
	if (not iIsSrcSet or not iIsDstSet)
		return;
	
	iTrack->Reset();
	iApi->SendRequestL(iSrcCoord, iDstCoord);
	}

void CRouting::Reset()
	{
	iIsSrcSet = EFalse;
	iIsDstSet = EFalse;
	iTrack->Reset();
	}

void CRouting::Source(TCoordinate& aSrc)
	{
	aSrc.SetCoordinate(iSrcCoord.Latitude(), iSrcCoord.Longitude());
	}

void CRouting::SetSource(const TCoordinate& aSrc)
	{
	iSrcCoord.SetCoordinate(aSrc.Latitude(), aSrc.Longitude());
	iIsSrcSet = ETrue;
	}

void CRouting::Destination(TCoordinate& aDst)
	{
	aDst.SetCoordinate(iDstCoord.Latitude(), iDstCoord.Longitude());
	}

void CRouting::SetDestination(const TCoordinate& aDst)
	{
	iDstCoord.SetCoordinate(aDst.Latitude(), aDst.Longitude());
	iIsDstSet = ETrue;
	}

void CRouting::OnRoutePointAddedL(const TCoordinate& aCoord)
	{
	iTrack->AddPointL(aCoord);
	}
void CRouting::OnFailedL()
	{
	iObserver->OnRouteFailedL();
	}


// CTrack

CTrack* CTrack::NewL()
	{
	CTrack* self = new (ELeave) CTrack();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self;
	return self;
	}

CTrack::CTrack()
	{
	}

void CTrack::ConstructL()
	{
	const TInt KGranularity = 50;
	
	iPoints = RArray<TCoordinate>(KGranularity);
	}

CTrack::~CTrack()
	{
	iPoints.Close();
	}

void CTrack::AddPointL(const TCoordinate& aCoord)
	{
	iPoints.AppendL(aCoord);
	}

const TCoordinate& CTrack::operator[](TInt anIndex) const
	{
	return iPoints[anIndex];
	}


// COrsRoutingApi

COrsRoutingApi* COrsRoutingApi::NewL(MRoutingApiObserver* aObserver)
	{
	COrsRoutingApi* self = new (ELeave) COrsRoutingApi(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self;
	return self;
	}

COrsRoutingApi::COrsRoutingApi(MRoutingApiObserver* aObserver)
		: iObserver(aObserver)
	{
	}

void COrsRoutingApi::ConstructL()
	{
	//CS60MapsAppUi* appUi = static_cast<CS60MapsAppUi*>(CCoeEnv::Static()->AppUi());
	//iHttpClient = CHTTPClient2::NewL(this, appUi->iSockServ, appUi->iConn);
	}

COrsRoutingApi::~COrsRoutingApi()
	{
	//delete iHttpClient;
	}

void COrsRoutingApi::SendRequestL(const TCoordinate& aSrc, const TCoordinate& aDst)
	{
	//////////////
	if (Math::Random() % 2)
		{
		iObserver->OnRoutePointAddedL(aSrc);
		iObserver->OnRoutePointAddedL(aDst);
		}
	else
		{
		iObserver->OnFailedL();
		}
	////////////////
	}
