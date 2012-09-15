/*
  ==============================================================================

    GradientDesigner.h
    Created: 14 Sep 2012 6:14:50pm
    Author:  David Rowland

  ==============================================================================
*/

#ifndef __GRADIENTDESIGNER_H_28D37715__
#define __GRADIENTDESIGNER_H_28D37715__

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class GradientDesigner    : public Component,
                            public ComponentListener,
                            public ChangeBroadcaster
{
public:
    GradientDesigner (const ColourGradient& gradient);
    
    ~GradientDesigner();

    void setGradient (const ColourGradient& newGradient);
    
    const ColourGradient& getGradient();
    
    void paint (Graphics& g);

    void resized();

    void mouseDown (const MouseEvent& e);
    
    void componentMovedOrResized (Component& component,
                                  bool wasMoved,
                                  bool wasResized);
    
    void handleAsyncUpdate();
    
private:
    //==============================================================================
    ColourGradient gradient;

    //==============================================================================
    class ColourPoint;
    friend class ColourPoint;
    OwnedArray<ColourPoint> points;

    Rectangle<int> previewArea, pointsArea;
    bool isResizing;

    //==============================================================================
    int addColour (double position, const Colour& colour);
    void setColour (int index, const Colour& colour);
    void removeColour (int index);
    
    //==============================================================================
    void addPoint (double position, const Colour& colour);
    void removePoint (ColourPoint* pointToRemove);
    void updatePointColour (ColourPoint* pointToChange);

    bool constrainPointBounds (ColourPoint* point);
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GradientDesigner);
};


#endif  // __GRADIENTDESIGNER_H_28D37715__
