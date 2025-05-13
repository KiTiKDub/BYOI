#pragma once
#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_graphics/juce_graphics.h"
#include "KitikUtility/GUI/LookAndFeel.h"

struct BYOI_lnf : public kitik::Laf
{
    BYOI_lnf() {}
    ~BYOI_lnf() {}

    void drawLinearSlider(juce::Graphics&, int x, int y, int width, int height,
    float sliderPos, float minSliderPos, float maxSliderPos,
    const juce::Slider::SliderStyle, juce::Slider&) override;

    void drawButtonBackground (juce::Graphics&, juce::Button&, const juce::Colour& backgroundColour,
                                               bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
};
