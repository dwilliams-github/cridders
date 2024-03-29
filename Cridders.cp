// ===========================================================================
//	Cridders.cp 				�1994-1998 Metrowerks Inc. All rights reserved.
// ===========================================================================
//	This file contains the starter code for a basic PowerPlant project

#include "Cridders.h"

#include <LGrowZone.h>
#include <PP_Messages.h>
#include <PP_Resources.h>
#include <PPobClasses.h>
#include <UDrawingState.h>
#include <UMemoryMgr.h>
#include <URegistrar.h>

#include <LWindow.h>
#include <LCaption.h>

// put declarations for resource ids (ResIDTs) here


const PP_PowerPlant::ResIDT	wind_SampleWindow = 128;	// EXAMPLE, create a new window


// ===========================================================================
//		� Main Program
// ===========================================================================

int main()
{
								
	SetDebugThrow_(PP_PowerPlant::debugAction_Alert);	// Set Debugging options
	SetDebugSignal_(PP_PowerPlant::debugAction_Alert);

	PP_PowerPlant::InitializeHeap(3);					// Initialize Memory Manager
														// Parameter is number of Master Pointer
														// blocks to allocate
	
	PP_PowerPlant::UQDGlobals::InitializeToolbox(&qd);	// Initialize standard Toolbox managers
	
	new PP_PowerPlant::LGrowZone(20000);				// Install a GrowZone function to catch
														// low memory situations.

	Cridders	theApp;									// replace this with your App type
	theApp.Run();
	
	return 0;
}


// ---------------------------------------------------------------------------
//		� Cridders
// ---------------------------------------------------------------------------
//	Constructor

Cridders::Cridders()
{
	RegisterClass_(PP_PowerPlant::LWindow);		// You must register each kind of
	RegisterClass_(PP_PowerPlant::LCaption);	// PowerPlant classes that you use
												// in your PPob resource.
}


// ---------------------------------------------------------------------------
//		� ~Cridders
// ---------------------------------------------------------------------------
//	Destructor

Cridders::~Cridders()
{
}

// ---------------------------------------------------------------------------
//		� StartUp
// ---------------------------------------------------------------------------
//	This method lets you do something when the application starts up
//	without a document. For example, you could issue your own new command.

void
Cridders::StartUp()
{
	ObeyCommand(PP_PowerPlant::cmd_New, nil);		// EXAMPLE, create a new window
}

// ---------------------------------------------------------------------------
//		� ObeyCommand
// ---------------------------------------------------------------------------
//	This method lets the application respond to commands like Menu commands

Boolean
Cridders::ObeyCommand(
	PP_PowerPlant::CommandT	inCommand,
	void					*ioParam)
{
	Boolean		cmdHandled = true;

	switch (inCommand) {
	
		// Handle command messages (defined in PP_Messages.h).
		case PP_PowerPlant::cmd_New:
										
			PP_PowerPlant::LWindow	*theWindow =
									PP_PowerPlant::LWindow::CreateWindow(wind_SampleWindow, this);
			ThrowIfNil_(theWindow);
			
			// LWindow is not initially visible in PPob resource
			theWindow->Show();
			break;

		// Any that you don't handle, such as cmd_About and cmd_Quit,
		// will be passed up to LApplication
		default:
			cmdHandled = PP_PowerPlant::LApplication::ObeyCommand(inCommand, ioParam);
			break;
	}
	
	return cmdHandled;
}

// ---------------------------------------------------------------------------
//		� FindCommandStatus
// ---------------------------------------------------------------------------
//	This method enables menu items.

void
Cridders::FindCommandStatus(
	PP_PowerPlant::CommandT	inCommand,
	Boolean					&outEnabled,
	Boolean					&outUsesMark,
	PP_PowerPlant::Char16	&outMark,
	Str255					outName)
{

	switch (inCommand) {
	
		// Return menu item status according to command messages.
		case PP_PowerPlant::cmd_New:
			outEnabled = true;
			break;

		// Any that you don't handle, such as cmd_About and cmd_Quit,
		// will be passed up to LApplication
		default:
			LApplication::FindCommandStatus(inCommand, outEnabled,
												outUsesMark, outMark, outName);
			break;
	}
}
