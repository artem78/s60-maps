/*
 * MapMath.h
 *
 *  Created on: 12.08.2019
 *      Author: user
 */

// ToDo: Separate type for lat, lon and zoom?

#ifndef MAPMATH_H_
#define MAPMATH_H_

#include <e32base.h>
#include <lbsposition.h>

class TTile;
class TTileReal;

class MapMath
	{
public:
	static void PixelsToMeters(const TReal64 &aLatitude, /*TUint8*/ TInt aZoom, TUint aPixels /*= 1*/,
			TReal32 &aHorizontalDistance, TReal32 &aVerticalDistance);
	static void PixelsToDegrees(const TReal64 &aLatitude, /*TUint8*/ TInt aZoom, TUint aPixels /*= 1*/,
			/*TReal32*/ TReal64 &aHorizontalAngle, /*TReal32*/ TReal64 &aVerticalAngle);
	static TTile GeoCoordsToTile(const TCoordinate &aCoord, /*TUint8*/ TInt aZoom);
	static TTileReal GeoCoordsToTileReal(const TCoordinate &aCoord, /*TUint8*/ TInt aZoom);
	static TCoordinate TileToGeoCoords(const TTileReal &aTile, /*TUint8*/ TInt aZoom);
	static TCoordinate TileToGeoCoords(const TTile &aTile, /*TUint8*/ TInt aZoom);
	};

class TTile
	{
public:
	TUint iX;
	TUint iY;
	TUint iZ;
	};

class TTileReal
	{
public:
	TReal iX;
	TReal iY;
	TUint iZ;
	};

#endif /* MAPMATH_H_ */
