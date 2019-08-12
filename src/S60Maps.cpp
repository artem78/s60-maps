/*
 ============================================================================
 Name		: S60Maps.cpp
 Author	  : artem78
 Copyright   : 
 Description : Main application class
 ============================================================================
 */

// INCLUDE FILES
#include <eikstart.h>
#include "S60MapsApplication.h"

LOCAL_C CApaApplication* NewApplication()
	{
	return new CS60MapsApplication;
	}

GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication(NewApplication);
	}

