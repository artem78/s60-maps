/*
 * MapMath.h
 *
 *  Created on: 12.08.2019
 *      Author: user
 */

// ToDo: Separate type for lat, lon?

#ifndef MAPMATH_H_
#define MAPMATH_H_

#include <e32base.h>
#include <lbsposition.h>
#include "Defs.h"

class TTile;
class TTileReal;

class MapMath
	{
public:
	static void PixelsToMeters(const TReal64 &aLatitude, TZoom aZoom, TUint aPixels /*= 1*/,
			TReal32 &aHorizontalDistance, TReal32 &aVerticalDistance);
	static void PixelsToDegrees(const TReal64 &aLatitude, TZoom aZoom, TUint aPixels /*= 1*/,
			/*TReal32*/ TReal64 &aHorizontalAngle, /*TReal32*/ TReal64 &aVerticalAngle);
	static TTile GeoCoordsToTile(const TCoordinate &aCoord, TZoom aZoom);
	static TTileReal GeoCoordsToTileReal(const TCoordinate &aCoord, TZoom aZoom);
	static TCoordinate TileToGeoCoords(const TTileReal &aTile, TZoom aZoom);
	static TCoordinate TileToGeoCoords(const TTile &aTile, TZoom aZoom);
	static TPoint GeoCoordsToProjectionPoint(const TCoordinate &aCoord, TZoom aZoom);
	static TCoordinate ProjectionPointToGeoCoords(const TPoint &aPoint, TZoom aZoom);
	};

class TTile
	{
public:
	TUint iX;
	TUint iY;
	TZoom iZ;
	};

class TTileReal
	{
public:
	TReal iX;
	TReal iY;
	TZoom iZ;
	};

#endif /* MAPMATH_H_ */
