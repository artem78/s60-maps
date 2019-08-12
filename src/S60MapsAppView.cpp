/*
 ============================================================================
 Name		: S60MapsAppView.cpp
 Author	  : artem78
 Copyright   : 
 Description : Application view implementation
 ============================================================================
 */

// INCLUDE FILES
#include <coemain.h>
#include "S60MapsAppView.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CS60MapsAppView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CS60MapsAppView* CS60MapsAppView::NewL(const TRect& aRect)
	{
	CS60MapsAppView* self = CS60MapsAppView::NewLC(aRect);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CS60MapsAppView::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CS60MapsAppView* CS60MapsAppView::NewLC(const TRect& aRect)
	{
	CS60MapsAppView* self = new (ELeave) CS60MapsAppView;
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	return self;
	}

// -----------------------------------------------------------------------------
// CS60MapsAppView::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CS60MapsAppView::ConstructL(const TRect& aRect)
	{
	// Create a window for this application view
	CreateWindowL();

	// Set the windows size
	SetRect(aRect);

	// Activate the window, which makes it ready to be drawn
	ActivateL();
	}

// -----------------------------------------------------------------------------
// CS60MapsAppView::CS60MapsAppView()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CS60MapsAppView::CS60MapsAppView()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CS60MapsAppView::~CS60MapsAppView()
// Destructor.
// -----------------------------------------------------------------------------
//
CS60MapsAppView::~CS60MapsAppView()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CS60MapsAppView::Draw()
// Draws the display.
// -----------------------------------------------------------------------------
//
void CS60MapsAppView::Draw(const TRect& /*aRect*/) const
	{
	// Get the standard graphics context
	CWindowGc& gc = SystemGc();

	// Gets the control's extent
	TRect drawRect(Rect());

	// Clears the screen
	gc.Clear(drawRect);

	}

// -----------------------------------------------------------------------------
// CS60MapsAppView::SizeChanged()
// Called by framework when the view size is changed.
// -----------------------------------------------------------------------------
//
void CS60MapsAppView::SizeChanged()
	{
	DrawNow();
	}

// -----------------------------------------------------------------------------
// CS60MapsAppView::HandlePointerEventL()
// Called by framework to handle pointer touch events.
// Note: although this method is compatible with earlier SDKs, 
// it will not be called in SDKs without Touch support.
// -----------------------------------------------------------------------------
//
void CS60MapsAppView::HandlePointerEventL(const TPointerEvent& aPointerEvent)
	{

	// Call base class HandlePointerEventL()
	CCoeControl::HandlePointerEventL(aPointerEvent);
	}

// End of File
