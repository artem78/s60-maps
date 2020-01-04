/*
 ============================================================================
 Name		: S60MapsDocument.h
 Author	  : artem78
 Copyright   : 
 Description : Declares document class for application.
 ============================================================================
 */

#ifndef __S60MAPSDOCUMENT_h__
#define __S60MAPSDOCUMENT_h__

#ifdef _DEBUG
#define LOGGING_ENABLED 1
#else
#define LOGGING_ENABLED 0
#endif

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class CS60MapsAppUi;
class CEikApplication;

// CLASS DECLARATION

/**
 * CS60MapsDocument application class.
 * An instance of class CS60MapsDocument is the Document part of the
 * AVKON application framework for the S60Maps example application.
 */
class CS60MapsDocument : public CAknDocument
	{
public:
	// Constructors and destructor

	/**
	 * NewL.
	 * Two-phased constructor.
	 * Construct a CS60MapsDocument for the AVKON application aApp
	 * using two phase construction, and return a pointer
	 * to the created object.
	 * @param aApp Application creating this document.
	 * @return A pointer to the created instance of CS60MapsDocument.
	 */
	static CS60MapsDocument* NewL(CEikApplication& aApp);

	/**
	 * NewLC.
	 * Two-phased constructor.
	 * Construct a CS60MapsDocument for the AVKON application aApp
	 * using two phase construction, and return a pointer
	 * to the created object.
	 * @param aApp Application creating this document.
	 * @return A pointer to the created instance of CS60MapsDocument.
	 */
	static CS60MapsDocument* NewLC(CEikApplication& aApp);

	/**
	 * ~CS60MapsDocument
	 * Virtual Destructor.
	 */
	virtual ~CS60MapsDocument();

public:
	// Functions from base classes

	/**
	 * CreateAppUiL
	 * From CEikDocument, CreateAppUiL.
	 * Create a CS60MapsAppUi object and return a pointer to it.
	 * The object returned is owned by the Uikon framework.
	 * @return Pointer to created instance of AppUi.
	 */
	CEikAppUi* CreateAppUiL();
	
	virtual CFileStore* OpenFileL(TBool aDoOpen,
			const TDesC& aFilename, RFs& aFs);


private:
	// Constructors

	/**
	 * ConstructL
	 * 2nd phase constructor.
	 */
	void ConstructL();

	/**
	 * CS60MapsDocument.
	 * C++ default constructor.
	 * @param aApp Application creating this document.
	 */
	CS60MapsDocument(CEikApplication& aApp);

	
public:
	void StoreL(CStreamStore& aStore,
			CStreamDictionary& aStreamDic) const;
	void RestoreL(const CStreamStore& aStore,
			const CStreamDictionary& aStreamDic);
	
	
	// Custom properties and methods
private:
#if LOGGING_ENABLED
	RFile iLogFile;
#endif

	};

#endif // __S60MAPSDOCUMENT_h__
// End of File
