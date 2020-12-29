/*
 * MapMath.cpp
 *
 *  Created on: 12.08.2019
 *      Author: user
 */

// Formulas source: https://wiki.openstreetmap.org/wiki/Zoom_levels

#include "MapMath.h"
#include <e32math.h>
#include "Map.h"


// Constants
const TReal KEarthRadiusMinor = 6356752.3142; // According to WGS 84, in meters
const TReal KEarthRadiusMajor = 6378137.0;    // According to WGS 84, in meters
const TReal KEquatorLength = 2 * KPi * KEarthRadiusMajor; // Equatorial circumference of the Earth in meters


void MapMath::PixelsToMeters(const TReal64 &aLatitude, TZoom aZoom, TUint aPixels,
			TReal32 &aHorizontalDistance, TReal32 &aVerticalDistance)
	{
	TReal c;
	Math::Cos(c, aLatitude * KDegToRad);
	//TReal p;
	//Math::Pow(p, 2, aZoom + 8);
	//TInt p = 2 ** (aZoom + 8);
	TReal p;
	Math::Pow(p, 2, aZoom + 8);
	aHorizontalDistance = aPixels * KEquatorLength * c / p;
	aVerticalDistance = (Abs(KMinLatitudeMapBound) + Abs(KMinLatitudeMapBound)) /*~170deg*/
			* KDegToRad * KEarthRadiusMinor / p * aPixels; 
	}

void MapMath::PixelsToDegrees(const TReal64 &aLatitude, TZoom aZoom, TUint aPixels,
			/*TReal32*/ TReal64 &aHorizontalAngle, /*TReal32*/ TReal64 &aVerticalAngle)
	{
	TReal32 horizontalDistance;
	TReal32 verticalDistance;
	PixelsToMeters(aLatitude, aZoom, aPixels, horizontalDistance, verticalDistance);
	aHorizontalAngle =	horizontalDistance / (KEquatorLength / 360.0);
	aVerticalAngle =	verticalDistance   / (KEquatorLength / 360.0);
	}

TTile MapMath::GeoCoordsToTile(const TCoordinate &aCoord, TZoom aZoom)
	{
	TTileReal tileReal = GeoCoordsToTileReal(aCoord, aZoom);
	TTile tile;
	tile.iX = (TUint) tileReal.iX;
	tile.iY = (TUint) tileReal.iY;
	tile.iZ = tileReal.iZ;
	return tile;
	}

TTileReal MapMath::GeoCoordsToTileReal(const TCoordinate &aCoord, TZoom aZoom)
	{
	// https://wiki.openstreetmap.org/wiki/Slippy_map_tilenames#Lon..2Flat._to_tile_numbers_2
	
	TTileReal tile;
	tile.iZ = aZoom;
	
	// (int)(floor((lon + 180.0) / 360.0 * (1 << z))); 
	tile.iX = (aCoord.Longitude() + 180.0) / 360.0 * (1 << aZoom);
	
	//double latrad = lat * M_PI/180.0;
	//return (int)(floor((1.0 - asinh(tan(latrad)) / M_PI) / 2.0 * (1 << z)));
	//TReal latrad = aCoord.Latitude() * KPi;
	//tile.iY = (TUint) (1.0 - asinh(tan(latrad)) / KPi) / 2.0 * (1 << aZoom));
	// (1 - math.log(math.tan(math.radians(lat)) + 1 / math.cos(math.radians(lat))) / math.pi) / 2 * 2 ** zoom
	TReal latRad = aCoord.Latitude() * KDegToRad;
	TReal t;
	Math::Tan(t, latRad);
	TReal c;
	Math::Cos(c, latRad);
	TReal l;
	Math::Ln(l, t + 1.0 / c);
	
	TReal p;
	Math::Pow(p, 2, aZoom);
	tile.iY = (1.0 - l / KPi) / 2 * /*2 ** aZoom*/ p;
	
	return tile;
	}

TCoordinate MapMath::TileToGeoCoords(const TTileReal &aTile, TZoom aZoom)
	{
	/*double tilex2long(int x, int z) 
	{
		return x / (double)(1 << z) * 360.0 - 180;
	}

	double tiley2lat(int y, int z) 
	{
		double n = M_PI - 2.0 * M_PI * y / (double)(1 << z);
		return 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));
	}*/
	
	TCoordinate coord;
	TReal64 lon = aTile.iX / (TReal64)(1 << aZoom) * 360.0 - 180;
	TReal64 n = KPi - 2.0 * KPi * aTile.iY / (TReal64)(1 << aZoom);
	TReal64 pe;
	Math::Exp(pe, n);
	TReal64 ne;
	Math::Exp(ne, -n);
	TReal64 at;
	Math::ATan(at, 0.5 * (pe - ne));
	TReal64 lat = KRadToDeg * at;
	coord.SetCoordinate(lat, lon);
	return coord;	
	}

TCoordinate MapMath::TileToGeoCoords(const TTile &aTile, TZoom aZoom)
	{
	TTileReal tileReal;
	tileReal.iX = aTile.iX;
	tileReal.iY = aTile.iY;
	tileReal.iZ = aTile.iZ;
	return TileToGeoCoords(tileReal, aZoom);
	}

TPoint MapMath::GeoCoordsToProjectionPoint(const TCoordinate &aCoord, TZoom aZoom)
	{
	TTileReal tileReal = GeoCoordsToTileReal(aCoord, aZoom);
	TReal x, y;
	x = tileReal.iX * KTileSize;
	Math::Round(x, x, 0);
	y = tileReal.iY * KTileSize;
	Math::Round(y, y, 0);
	return TPoint(x, y);
	}

TCoordinate MapMath::ProjectionPointToGeoCoords(const TPoint &aPoint, TZoom aZoom)
	{
	TTileReal tileReal;
	tileReal.iX = aPoint.iX / TReal(KTileSize);
	tileReal.iY = aPoint.iY / TReal(KTileSize);
	tileReal.iZ = aZoom;
	return TileToGeoCoords(tileReal, aZoom);
	}

TTile MapMath::ProjectionPointToTile(const TPoint &aPoint, TZoom aZoom)
	{
	TTile tile;
	tile.iX = aPoint.iX / KTileSize;
	tile.iY = aPoint.iY / KTileSize;
	tile.iZ = aZoom;
	return tile;
	}

TPoint MapMath::TileToProjectionPoint(const TTile &aTile)
	{
	TPoint projectionPoint;
	projectionPoint.iX = aTile.iX * KTileSize;
	projectionPoint.iY = aTile.iY * KTileSize;
	return projectionPoint;
	}

// TTile

TBool operator== (const TTile &aTile1, const TTile &aTile2)
	{
	return (aTile1.iX == aTile2.iX) &&
		   (aTile1.iY == aTile2.iY) &&
		   (aTile1.iZ == aTile2.iZ);
	}
TBool operator!= (const TTile &aTile1, const TTile &aTile2)
	{
	return !(aTile1 == aTile2);
	}

void TTile::AsDes(TDes &aDes) const
	{
	_LIT(KFormat, "TTile(x=%d, y=%d, z=%d)");
	aDes.Format(KFormat, (TInt) iX, (TInt) iY, (TInt) iZ);
	}

void TTile::AsDes(TDes8 &aDes) const
	{
	_LIT8(KFormat, "TTile(x=%d, y=%d, z=%d)");
	aDes.Format(KFormat, (TInt) iX, (TInt) iY, (TInt) iZ);
	}

const TBufC<32> TTile::AsDes() const
	{
	TBuf<32> buff;
	_LIT(KFormat, "TTile(x=%d, y=%d, z=%d)");
	buff.Format(KFormat, (TInt) iX, (TInt) iY, (TInt) iZ);
	return buff;
	}

const TBufC8<32> TTile::AsDes8() const
	{
	TBuf8<32> buff8;
	_LIT8(KFormat, "TTile(x=%d, y=%d, z=%d)");
	buff8.Format(KFormat, (TInt) iX, (TInt) iY, (TInt)iZ);
	return buff8;
	}
