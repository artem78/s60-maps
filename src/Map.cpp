/*
 * Map.cpp
 *
 *  Created on: 14.08.2019
 *      Author: user
 */

#include "Map.h"
#include "S60MapsAppView.h"

CMapLayerBase::CMapLayerBase(/*const*/ CS60MapsAppView* aMapView) :
		iMapView(aMapView)
	{
	}

CMapLayerStub::CMapLayerStub(/*const*/ CS60MapsAppView* aMapView) :
		CMapLayerBase(aMapView)
	{
	iCoords[0] = TCoordinate(40.7283,-73.9942); // New York
	iCoords[1] = TCoordinate(51.5072, -0.1275); // London
	iCoords[2] = TCoordinate(-34.9333, 138.5833); // Adelaide 
	iCoords[3] = TCoordinate(48.8333, 2.3333); // Paris 
	iCoords[4] = TCoordinate(55.7558, 37.6178); // Moscow
	iCoords[5] = TCoordinate(0, 0); // 0, 0*/
	}

void CMapLayerStub::Draw(CWindowGc &aGc)
	{
	const TInt KMarkSize = 5;
	TSize penSize(KMarkSize, KMarkSize);
	aGc.SetPenSize(penSize);
	//aGc.SetPenColor()
	
	TInt i;
	for (i = 0; i < iCoords.Count(); i++)
		{
		TCoordinate coord = iCoords[i];
		if (iMapView->CheckPointVisibility(coord)) // Check before drawing
			{
			TPoint point;
			point = iMapView->GeoCoordsToScreenCoords(coord);
			aGc.Plot(point);
			}
		}
	}
