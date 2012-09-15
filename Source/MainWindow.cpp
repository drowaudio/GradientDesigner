/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic structure for a simple desktop window.

  ==============================================================================
*/

#include "MainWindow.h"
#include "MainComponent.h"


//==============================================================================
MainAppWindow::MainAppWindow()
    : DocumentWindow ("MainAppWindow",
                      Colours::lightgrey,
                      DocumentWindow::allButtons)
{
    // At this point you should call setContentOwned() to give the window
    // a component containing the content you want to show..

    
    setContentOwned (new MainComponent(), false);

    centreWithSize (500, 400);
    setVisible (true);
    setResizable (true, true);
}

MainAppWindow::~MainAppWindow()
{
}

void MainAppWindow::closeButtonPressed()
{
    JUCEApplication::getInstance()->systemRequestedQuit();
}
