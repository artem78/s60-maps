/*
 * MapMath.h
 *
 *  Created on: 12.08.2019
 *      Author: artem78
 */

// ToDo: Separate type for lat, lon?

#ifndef MAPMATH_H_
#define MAPMATH_H_

#include <e32base.h>
#include <lbsposition.h>
#include "Defs.h"
#include "Utils.h"


// Constants
const TInt KTileSize = 256; // Standard tile height and width in pixels
const TInt KMinProjectionCoordXY = 0;


class TTile;

class MapMath
	{
private:
	class TTileReal
		{
	public:
		TReal iX;
		TReal iY;
		TZoom iZ;
		};

	
	static TTileReal GeoCoordsToTileReal(const TCoordinate &aCoord, TZoom aZoom);
	static TCoordinate TileToGeoCoords(const TTileReal &aTile, TZoom aZoom);
	
public:
	static void PixelsToMeters(const TReal64 &aLatitude, TZoom aZoom, TUint aPixels /*= 1*/,
			TReal32 &aHorizontalDistance, TReal32 &aVerticalDistance);
	static void PixelsToDegrees(const TReal64 &aLatitude, TZoom aZoom, TUint aPixels /*= 1*/,
			/*TReal32*/ TReal64 &aHorizontalAngle, /*TReal32*/ TReal64 &aVerticalAngle);
	static void MetersToPixels(const TReal64 &aLatitude, TZoom aZoom, TReal32 aDistance,
			/*TUint*/ TInt &aHorizontalPixels, /*TUint*/ TInt &aVerticalPixels);
	static TTile GeoCoordsToTile(const TCoordinate &aCoord, TZoom aZoom);
	static TCoordinate TileToGeoCoords(const TTile &aTile);
	static TPoint GeoCoordsToProjectionPoint(const TCoordinate &aCoord, TZoom aZoom);
	static TCoordinate ProjectionPointToGeoCoords(const TPoint &aPoint, TZoom aZoom);
	static TTile ProjectionPointToTile(const TPoint &aPoint, TZoom aZoom);
	static TPoint TileToProjectionPoint(const TTile &aTile);
	static inline TInt MaxProjectionCoordXY(TZoom aZoom)
		{ return MathUtils::Pow2(aZoom) * KTileSize - 1; }
	};

class TTile
	{
public:
	TUint32 iX;
	TUint32 iY;
	TZoom iZ;
	
	friend TBool operator== (const TTile &aTile1, const TTile &aTile2);
	friend TBool operator!= (const TTile &aTile1, const TTile &aTile2);
	
	void AsDes(TDes &aDes) const;
	void AsDes(TDes8 &aDes) const;
	const TBufC<32> AsDes() const;
	const TBufC8<32> AsDes8() const;
	};

#endif /* MAPMATH_H_ */
