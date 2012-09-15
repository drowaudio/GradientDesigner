/*
  ==============================================================================

    MainComponent.cpp
    Created: 14 Sep 2012 11:51:31pm
    Author:  David Rowland

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
    : gradientDesigner (ColourGradient (Colours::blue, 0, 0,
                                        Colours::red, 0, 0,
                                        false))
{
    addAndMakeVisible (&gradientDesigner);
    gradientDesigner.addChangeListener (this);
}

MainComponent::~MainComponent()
{
    gradientDesigner.removeChangeListener (this);
}

void MainComponent::paint (Graphics& g)
{
    Rectangle<int> radialBoundsInt (getLocalBounds().removeFromBottom (getLocalBounds().getCentreY()));
    Rectangle<float> radialBounds (radialBoundsInt.toFloat());
    
    ColourGradient cg (gradientDesigner.getGradient());
    cg.isRadial = true;
    cg.point1 = radialBounds.getTopLeft();
    cg.point2 = radialBounds.getBottomRight();
    
    g.setGradientFill (cg);
    g.fillRect (radialBoundsInt);
}

void MainComponent::resized()
{
    gradientDesigner.setBounds (getLocalBounds().withHeight (getLocalBounds().getCentreY()));
}

void MainComponent::changeListenerCallback (ChangeBroadcaster* source)
{
    if (source == &gradientDesigner)
    {
        repaint();
    }
}
