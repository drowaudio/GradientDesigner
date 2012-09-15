/*
  ==============================================================================

    GradientDesigner.cpp
    Created: 14 Sep 2012 6:14:50pm
    Author:  David Rowland

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "GradientDesigner.h"


//==============================================================================
class GradientDesigner::ColourPoint :   public Component,
                                        public ChangeListener
{
public:
    //==============================================================================
    ColourPoint (GradientDesigner& owner_,
                 double pointPosition, const Colour& pointColour)
        : owner (owner_), position (pointPosition), colour (pointColour), draggable (true)
    {
    }
    
    virtual ~ColourPoint()
    {
    }
    
    void paint (Graphics& g)
    {
        Rectangle<float> bounds (getLocalBounds().toFloat());
        const float hw = bounds.getWidth() / 2.0f;
        Rectangle<int> colourBounds (getLocalBounds().removeFromBottom (getWidth()).reduced (2));
        
        Path arrowPath;
        arrowPath.addTriangle (0.0f, hw + 1,
                               (float) getWidth(), hw + 1,
                               (float) getLocalBounds().getCentreX(), 0.0f);

        // first draw the arrow and colour squares
        g.setColour (Colours::lightgrey);
        g.fillPath (arrowPath);
        g.fillRect (colourBounds.translated (1, 1));

        g.setColour (Colours::grey);
        g.drawHorizontalLine (hw, 0.0f, bounds.getRight());


        g.setColour (Colours::white);
        g.fillRect (colourBounds);

        g.setColour (colour);
        g.fillRect (colourBounds);

        // now draw the black outline
        Path outline;
        outline.startNewSubPath (0.0f, hw);
        outline.lineTo (bounds.getCentreX(), 0.0f);
        outline.lineTo (bounds.getRight(), hw);
        
        g.setColour (Colours::black);
        g.strokePath (outline, PathStrokeType (1.0f));
        g.drawHorizontalLine (getHeight() - 1, 0.0f, bounds.getRight());
        g.drawVerticalLine (0, hw, bounds.getHeight());
        g.drawVerticalLine (getWidth() - 1, hw, bounds.getHeight());
    }
    
    void mouseDown (const MouseEvent& e)
    {
        if (e.mods.isPopupMenu())
        {
            ColourSelector* colourSelector = new ColourSelector();
            colourSelector->setCurrentColour (colour);
            colourSelector->addChangeListener (this);
            colourSelector->setColour (ColourSelector::backgroundColourId, Colours::transparentBlack);
            colourSelector->setSize (300, 400);
            
            CallOutBox::launchAsynchronously (colourSelector, getScreenBounds(), nullptr);
        }
        else if (draggable)
        {
            dragger.startDraggingComponent (this, e);
        }
    }

    void mouseDrag (const MouseEvent& e)
    {
        if (draggable)
        {
            dragger.dragComponent (this, e, nullptr);

            const int startOffset = e.getDistanceFromDragStartY() < 0 ? -e.getMouseDownY()
                                                                      : e.getMouseDownY();
                        
            if ((abs (e.getDistanceFromDragStartY()) + startOffset) > 50)
                owner.removePoint (this);
        }
    }

    void changeListenerCallback (ChangeBroadcaster* source)
    {
        ColourSelector* cs = dynamic_cast <ColourSelector*> (source);
        colour = cs->getCurrentColour();
        owner.updatePointColour (this);
    }
    
private:
    //==============================================================================
    friend class GradientDesigner;
                     
    GradientDesigner& owner;
    double position;
    Colour colour;
    bool draggable;
    
    ComponentDragger dragger;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ColourPoint);
};


//==============================================================================
GradientDesigner::GradientDesigner (const ColourGradient& sourceGradient)
    : gradient  (sourceGradient), isResizing (false)
{
    setGradient (sourceGradient);
}

GradientDesigner::~GradientDesigner()
{
}

void GradientDesigner::setGradient (const ColourGradient& newGradient)
{
    jassert (newGradient.getNumColours() >= 2); // this needs to be an initialised gradient!

    points.clear();
    gradient.clearColours();
    
    for (int i = 0; i < newGradient.getNumColours(); ++i)
        addPoint (newGradient.getColourPosition (i), newGradient.getColour (i));
    
    points.getFirst()->draggable = false;
    points.getLast()->draggable = false;

    resized();
    repaint();
}

const ColourGradient& GradientDesigner::getGradient()
{
    return gradient;
}

void GradientDesigner::paint (Graphics& g)
{
    ColourGradient previewGradient (gradient);
    previewGradient.point1 = previewArea.getTopLeft().toFloat();
    previewGradient.point2 = previewArea.getTopRight().toFloat();
    previewGradient.isRadial = false;

    g.fillAll (Colours::white);
    g.fillCheckerBoard (previewArea, 10, 10, Colour (0xffdddddd), Colour (0xffffffff));

    g.setGradientFill (previewGradient);
    g.fillRect (previewArea);
    
    g.setColour (Colours::grey);
    g.drawRect (previewArea, 1);
}

void GradientDesigner::resized()
{
    const int pointW = 12;
    const int pointH = 18;
    
    ScopedValueSetter<bool> setter (isResizing, true);

    previewArea = getLocalBounds().reduced (pointW).withY (10).withHeight (getHeight() / 2);
    pointsArea = previewArea.withHeight (pointH).translated (0, previewArea.getHeight());
    
    for (int i = 0; i < points.size(); ++i)
    {
        ColourPoint* point = points.getUnchecked (i);
        
        if (point != nullptr)
            point->setBounds (previewArea.getX() + (previewArea.getWidth() + 2) * point->position - (pointW / 2), previewArea.getBottom(), pointW, pointH);
    }
}

void GradientDesigner::mouseDown (const MouseEvent& e)
{
    if (pointsArea.contains (e.getMouseDownPosition()))
    {
        const double position = (e.getMouseDownX() - previewArea.getX()) / (double) (previewArea.getWidth() + 2);
        addPoint (position, gradient.getColourAtPosition (position));
    }
}

void GradientDesigner::componentMovedOrResized (Component& component,
                                                bool wasMoved,
                                                bool wasResized)
{
    if (wasMoved && ! isResizing)
    {
        for (int i = 0; i < points.size(); ++i)
        {
            ColourPoint* point = points.getUnchecked (i);
            
            if (point == &component)
            {
                if (constrainPointBounds (point))
                    return;
                
                const double newPosition = (point->getBounds().getCentreX() - previewArea.getX()) / (double) (previewArea.getWidth() + 2);

                point->position = newPosition;
                gradient.removeColour (i);
                const int newIndex = addColour (newPosition, point->colour);

                points.move (i, newIndex);
                
                repaint();
            }
        }
    }
}

//==============================================================================
int GradientDesigner::addColour (double position, const Colour& colour)
{
    const int newIndex = gradient.addColour (position, colour);

    sendChangeMessage();
    
    return newIndex;
}

void GradientDesigner::setColour (int index, const Colour& colour)
{
    gradient.setColour (index, colour);
    
    sendChangeMessage();
}

void GradientDesigner::removeColour (int index)
{
    gradient.removeColour (index);

    sendChangeMessage();
}

//==============================================================================
void GradientDesigner::addPoint (double position, const Colour& colour)
{
    ColourPoint* newPoint = new ColourPoint (*this, position, colour);
    addAndMakeVisible (newPoint);
    
    const int newIndex = addColour (position, colour);
    points.insert (newIndex, newPoint);

    resized();
    repaint();

    newPoint->addComponentListener (this);
}

void GradientDesigner::removePoint (ColourPoint* pointToRemove)
{
    const int index = points.indexOf (pointToRemove);
    removeColour (index);
    points.remove (index);
    
    resized();
    repaint();
}

void GradientDesigner::updatePointColour (ColourPoint* pointToChange)
{
    for (int i = 0; i < points.size(); ++i)
    {
        ColourPoint* point = points[i];
        
        if (point == pointToChange)
        {
            setColour (i, point->colour);
            repaint();

            break;
        }
    }
}

bool GradientDesigner::constrainPointBounds (ColourPoint* point)
{
    bool hasBeenConstrained = false;
    
    if (point->getBounds().getCentreX() < previewArea.getX())
    {
        point->setBounds (previewArea.getX() - (point->getWidth() / 2), previewArea.getBottom(),
                          point->getWidth(), point->getHeight());
        hasBeenConstrained = true;
    }

    if (point->getBounds().getCentreX() > previewArea.getRight())
    {
        point->setBounds (previewArea.getRight() - (point->getWidth() / 2), previewArea.getBottom(),
                          point->getWidth(), point->getHeight());
        hasBeenConstrained = true;
    }

    if (point->getBounds().getY() > previewArea.getBottom()
        || point->getBounds().getY() < previewArea.getBottom())
    {
        point->setBounds (point->getX(), previewArea.getBottom(),
                          point->getWidth(), point->getHeight());
        hasBeenConstrained = true;
    }
    
    return hasBeenConstrained;
}
