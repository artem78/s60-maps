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

CRouting::CRouting() :
		iIsSrcSet(EFalse),
		iIsDstSet(EFalse)
	{
	}

CRouting::~CRouting()
	{
	delete iTrack;
	}

CRouting* CRouting::NewLC()
	{
	CRouting* self = new (ELeave) CRouting();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CRouting* CRouting::NewL()
	{
	CRouting* self = CRouting::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void CRouting::ConstructL()
	{
	iTrack = CTrack::NewL();
	}

void CRouting::FindRoute/*L*/()
	{
	if (not iIsSrcSet or not iIsDstSet)
		return;
	
	///////////
	iTrack->Reset();
	iTrack->AddPointL(iSrcCoord);
	iTrack->AddPointL(iDstCoord);
	///////
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
