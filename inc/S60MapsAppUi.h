/*
 ============================================================================
 Name		: S60MapsAppUi.h
 Author	  : artem78
 Copyright   : 
 Description : Declares UI class for application.
 ============================================================================
 */

#ifndef __S60MAPSAPPUI_h__
#define __S60MAPSAPPUI_h__

// INCLUDES
#include <aknappui.h>
#include <f32file.h>

// FORWARD DECLARATIONS
class CS60MapsAppView;

// CLASS DECLARATION
/**
 * CS60MapsAppUi application UI class.
 * Interacts with the user through the UI and request message processing
 * from the handler class
 */
class CS60MapsAppUi : public CAknAppUi, public MFileManObserver
	{
public:
	// Constructors and destructor

	/**
	 * ConstructL.
	 * 2nd phase constructor.
	 */
	void ConstructL();

	/**
	 * CS60MapsAppUi.
	 * C++ default constructor. This needs to be public due to
	 * the way the framework constructs the AppUi
	 */
	CS60MapsAppUi();

	/**
	 * ~CS60MapsAppUi.
	 * Virtual Destructor.
	 */
	virtual ~CS60MapsAppUi();

private:
	// Functions from base classes

	/**
	 * From CEikAppUi, HandleCommandL.
	 * Takes care of command handling.
	 * @param aCommand Command to be handled.
	 */
	void HandleCommandL(TInt aCommand);

	/**
	 *  HandleStatusPaneSizeChange.
	 *  Called by the framework when the application status pane
	 *  size is changed.
	 */
	void HandleStatusPaneSizeChange();

	/**
	 *  From CCoeAppUi, HelpContextL.
	 *  Provides help context for the application.
	 *  size is changed.
	 */
	CArrayFix<TCoeHelpContext>* HelpContextL() const;

private:
	// Data

	/**
	 * The application view
	 * Owned by CS60MapsAppUi
	 */
	CS60MapsAppView* iAppView;

public:
	TStreamId StoreL(CStreamStore& aStore) const;
	void RestoreL(const CStreamStore& aStore, TStreamId aStreamId);
	void ExternalizeL(RWriteStream& aStream) const;
	void InternalizeL(RReadStream& aStream);

	// MFileManObserver
public:
	MFileManObserver::TControl NotifyFileManStarted();
	MFileManObserver::TControl NotifyFileManOperation();
	MFileManObserver::TControl NotifyFileManEnded();
	
	// Custom properties and methods
private:
	CFileMan* iFileMan;
	
	void ClearTilesCache();
	};

#endif // __S60MAPSAPPUI_h__
// End of File
