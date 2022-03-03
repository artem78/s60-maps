/*
 ============================================================================
 Name		: LBSSatelliteExtended.cpp
 Author	  : artem78
 Version	 : 1.0
 Copyright   : 
 Description : Extended satellite info class
 ============================================================================
 */

#include "LBSSatelliteExtended.h"
#include <e32math.h>
#include "Logger.h"

TReal TPositionSatelliteInfoExtended::PositionDoP() const
	{
	TReal nan = 0.0 / 0.0; // NaN   ToDo: Not very good
	TReal tmp1, tmp2;
	TInt r = Math::Pow(tmp1, HorizontalDoP(), 2);
	if (r != KErrNone)
		return nan;
	r = Math::Pow(tmp2, VerticalDoP(), 2);
	if (r != KErrNone)
		return nan;
	TReal pdop;
	r = Math::Sqrt(pdop, tmp1 + tmp2);
	if (r != KErrNone)
		return nan;
	return pdop;
	}

TReal TPositionSatelliteInfoExtended::GeometricDoP() const
	{
	TReal nan = 0.0 / 0.0; // NaN   ToDo: Not very good
	TReal tmp1, tmp2;
	TInt r = Math::Pow(tmp1, PositionDoP(), 2);
	if (r != KErrNone)
		return nan;
	r = Math::Pow(tmp2, TimeDoP(), 2);
	if (r != KErrNone)
		return nan;
	TReal gdop;
	r = Math::Sqrt(gdop, tmp1 + tmp2);
	if (r != KErrNone)
		return nan;
	return gdop;
	}

TInt TPositionSatelliteInfoExtended::NumOfVisibleSatellites(TInt aMinSignalStrength) const
	{
	TInt counter = 0;
	
	TSatelliteData satData;
	
	for (TInt i = 0; i < NumSatellitesInView(); i++)
		{
		if (GetSatelliteData(i, satData) == KErrNone)
			{
			DEBUG(_L("Sat #%d signal = %d"), satData.SatelliteId(), satData.SignalStrength());
			if (satData.SignalStrength() >= aMinSignalStrength)
				{
				counter++;
				}
			}
		}
	
	DEBUG(_L("Num = %d/%d"), counter, NumSatellitesInView());
	
	return counter;
	}
