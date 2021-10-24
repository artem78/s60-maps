/*
 ============================================================================
 Name		: S60MapsApplication.h
 Author	  : artem78
 Copyright   : 
 Description : Declares main application class.
 ============================================================================
 */

#ifndef __S60MAPSAPPLICATION_H__
#define __S60MAPSAPPLICATION_H__

// INCLUDES
#include <aknapp.h>
#include "S60Maps.hrh"

// UID for the application;
// this should correspond to the uid defined in the mmp file
const TUid KUidS60MapsApp =
	{
	_UID3
	};

// CLASS DECLARATION

/**
 * CS60MapsApplication application class.
 * Provides factory to create concrete document object.
 * An instance of CS60MapsApplication is the application part of the
 * AVKON application framework for the S60Maps example application.
 */
class CS60MapsApplication : public CAknApplication
	{
public:
	// Functions from base classes

	/**
	 * From CApaApplication, AppDllUid.
	 * @return Application's UID (KUidS60MapsApp).
	 */
	TUid AppDllUid() const;

protected:
	// Functions from base classes

	/**
	 * From CApaApplication, CreateDocumentL.
	 * Creates CS60MapsDocument document object. The returned
	 * pointer in not owned by the CS60MapsApplication object.
	 * @return A pointer to the created document object.
	 */
	CApaDocument* CreateDocumentL();
	
	// From CEikApplication
private:
	void GetDefaultDocumentFileName(TFileName& aDocumentName) const;
	
	
	// Custom properties and methods
public:
	// Return path to program root data directory
	void DataDir(TFileName &aDataDir) const;
	// Transform relative path to absolute from program root data directory
	void RelPathToAbsFromDataDir(const TDesC &aRelPath, TFileName &anAbsPath) const;
	void CacheDir(TFileName &aCacheDir) const;
	void AtlasesDir(TFileName &anAtlasesDir) const;
	};

#endif // __S60MAPSAPPLICATION_H__
// End of File
