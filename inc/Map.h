/*
 * Map.h
 *
 *  Created on: 14.08.2019
 *      Author: user
 */

#ifndef MAP_H_
#define MAP_H_

//#include "S60MapsAppView.h"
#include <w32std.h>
#include <lbsposition.h>


// Forward declaration
class CS60MapsAppView;


// Classes

class CMapLayerBase : public CBase
	{
protected:
	CS60MapsAppView* iMapView;
public:
	CMapLayerBase(/*const*/ CS60MapsAppView* aMapView);
	virtual void Draw(CWindowGc &aGc) = 0;
	};

// Stub layer (for testing)
class CMapLayerStub : public CMapLayerBase
	{
private:
	TFixedArray<TCoordinate, 6> iCoords;
public:
	CMapLayerStub(/*const*/ CS60MapsAppView* aMapView);
	void Draw(CWindowGc &aGc);
	};

#endif /* MAP_H_ */
