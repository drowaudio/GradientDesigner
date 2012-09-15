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
/**
    This component is used to display and edit colour gradients.
    
    A linear representation of the gradient is displayed along with markers that
    can be dragged around to re-position the points of the gradient.
    Right-clicking on a marker will show a colour selector, dragging a marker
    up or down will remove it. Click in the space just below the display to add
    new markers.
 
    The designer keeps its own internal copy of the gradient displayed and you
    can be notified of changes to this by registering as ChangeListener.
 
    @see ColourGradient, ChangeListener
 */
class GradientDesigner    : public Component,
                            public ComponentListener,
                            public ChangeBroadcaster
{
public:
    //==============================================================================
    /** Note that the designer only takes a copy of the gradient and never changes
        the original. To find out when changes are made to the gradient, register
        yourself as a ChangeListener to the designer and call getGradient() when
        you recive change callbacks.
     
        @param gradient     The source gradient to initially show. This must be a
                            valid gradient i.e. have it's start and end colours set.
     */
    GradientDesigner (const ColourGradient& gradient);
    
    /** Destructor. */
    ~GradientDesigner();

    /** Sets a gradient to be the represented by the designer. This will override
        any existing gradient so make sure you take a copy of it if needed.
        
        @param gradient     A new source gradient. This must be a valid gradient
                            i.e. have it's start and end colours set.
     */
    void setGradient (const ColourGradient& newGradient);
    
    /** Returns the gradient currently represented by the designer.
     */
    const ColourGradient& getGradient();
    
    //==============================================================================
    /** @internal */
    void paint (Graphics& g);

    /** @internal */
    void resized();

    /** @internal */
    void mouseDown (const MouseEvent& e);
    
    /** @internal */
    void componentMovedOrResized (Component& component,
                                  bool wasMoved,
                                  bool wasResized);
    
private:
    //==============================================================================
    class ColourPoint;
    friend class ColourPoint;

    ColourGradient gradient;
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
