/*
 ============================================================================
 Name		: LBSSatelliteExtended.h
 Author	  : artem78
 Version	 : 1.0
 Copyright   : 
 Description : Extended satellite info class
 ============================================================================
 */

#ifndef LBSSATELLITEEXTENDED_H
#define LBSSATELLITEEXTENDED_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <lbssatellite.h>

// CLASS DECLARATION

/**
 *  Adds position and geometric dilution of precision calculation methods
 *  to TPositionSatelliteInfo class
 * 
 */
class TPositionSatelliteInfoExtended : public TPositionSatelliteInfo
	{
public:
	/**
	 * @return Position dilution of precision or NaN
	 */
	TReal PositionDoP() const;
	
	/**
	 * @return Geometric dilution of precision or NaN
	 */
	TReal GeometricDoP() const;
	};

#endif // LBSSATELLITEEXTENDED_H
