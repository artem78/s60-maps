/*
 ============================================================================
 Name		: S60Maps.pan
 Author	  : artem78
 Copyright   : 
 Description : This file contains panic codes.
 ============================================================================
 */

#ifndef __S60MAPS_PAN__
#define __S60MAPS_PAN__

/** S60Maps application panic codes */
enum TS60MapsPanics
	{
	ES60MapsUi = 1,
	ES60MapsGeneralPanic = 100
	};

inline void Panic(TS60MapsPanics aReason)
	{
	_LIT(applicationName, "S60Maps");
	User::Panic(applicationName, aReason);
	}

#endif // __S60MAPS_PAN__
