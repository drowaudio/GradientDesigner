/*
  ==============================================================================

    MainComponent.h
    Created: 14 Sep 2012 11:51:31pm
    Author:  David Rowland

  ==============================================================================
*/

#ifndef __MAINCOMPONENT_H_7EA5AE9E__
#define __MAINCOMPONENT_H_7EA5AE9E__

#include "../JuceLibraryCode/JuceHeader.h"
#include  "GradientDesigner.h"


//==============================================================================
/*
*/
class MainComponent :   public Component,
                        public ChangeListener
{
public:
    MainComponent();
    
    ~MainComponent();

    void paint (Graphics&);
    
    void resized();

    void changeListenerCallback (ChangeBroadcaster* source);
    
private:
    //==============================================================================
    GradientDesigner gradientDesigner;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent);
};


#endif  // __MAINCOMPONENT_H_7EA5AE9E__
