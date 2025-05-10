#include "BYOILnf.h"
#include "juce_core/juce_core.h"

// void BYOI_lnf::drawDrawableButton (juce::Graphics& g, juce::DrawableButton& button,
//                                          bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
// {
//     bool toggleState = button.getToggleState();

//     g.fillAll (toggleState ? juce::Colours::black : juce::Colour(64u, 194u, 230u));

//     const int textH = (button.getStyle() == juce::DrawableButton::ImageAboveTextLabel)
//                         ? juce::jmin (16, button.proportionOfHeight (0.25f))
//                         : 0;

//     if (textH > 0)
//     {
//         g.setFont ((float) textH);

//         g.setColour (toggleState ? juce::Colours::black : juce::Colour(64u, 194u, 230u)
//                         .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.4f));

//         g.drawFittedText (button.getButtonText(),
//                           2, button.getHeight() - textH - 1,
//                           button.getWidth() - 4, textH,
//                           juce::Justification::centred, 1);
//     }
// }

void BYOI_lnf::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle style, juce::Slider& slider)
{
    using namespace juce;

    if (slider.isBar()) //Need to reduce size to add name
    {
        float sliderHeight = slider.getHeight();
        g.setColour(juce::Colour(64u, 194u, 230u));
        g.fillRect(slider.isHorizontal() ? Rectangle<float>(static_cast<float> (x) -1.f, 0 +.5f, sliderPos - (float)x + 1, (float)(sliderHeight))
            : Rectangle<float>((float)x + 0.5f, sliderPos, (float)width - 1.0f, (float)y + ((float)height - sliderPos)));

        auto valueRect = juce::Rectangle<float>(0, 0 + 1, width, height);
        auto nameRect = juce::Rectangle<float>(0, 0, slider.getWidth(), sliderHeight);

        auto valueStr = slider.textFromValueFunction(slider.getValue());
        auto name = static_cast<juce::String>(slider.getName());

        auto font = g.getCurrentFont();
        g.setColour(juce::Colours::white);
        g.setFont(sliderHeight/2);
        g.drawFittedText(name, nameRect.toNearestInt(), juce::Justification::topLeft, 1);
        g.drawFittedText(valueStr, valueRect.toNearestInt(), juce::Justification::centred, 1);

        drawLinearSliderOutline(g, x, y, width, height, style, slider);
    }
    else
    {
        auto isTwoVal = (style == Slider::SliderStyle::TwoValueVertical || style == Slider::SliderStyle::TwoValueHorizontal);
        auto isThreeVal = (style == Slider::SliderStyle::ThreeValueVertical || style == Slider::SliderStyle::ThreeValueHorizontal);

        auto trackWidth = jmin(6.0f, slider.isHorizontal() ? (float)height * 0.25f : (float)width * 0.25f);

        Point<float> startPoint(slider.isHorizontal() ? (float)x : (float)x + (float)width * 0.5f,
            slider.isHorizontal() ? (float)y + (float)height * 0.5f : (float)(height + y));

        Point<float> endPoint(slider.isHorizontal() ? (float)(width + x) : startPoint.x,
            slider.isHorizontal() ? startPoint.y : (float)y);

        Path backgroundTrack;
        backgroundTrack.startNewSubPath(startPoint);
        backgroundTrack.lineTo(endPoint);
        g.setColour(Colours::dimgrey);
        g.strokePath(backgroundTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

        Path valueTrack;
        Point<float> minPoint, maxPoint, thumbPoint;

        if (isTwoVal || isThreeVal)
        {
            minPoint = { slider.isHorizontal() ? minSliderPos : (float)width * 0.5f,
                         slider.isHorizontal() ? (float)height * 0.5f : minSliderPos };

            if (isThreeVal)
                thumbPoint = { slider.isHorizontal() ? sliderPos : (float)width * 0.5f,
                               slider.isHorizontal() ? (float)height * 0.5f : sliderPos };

            maxPoint = { slider.isHorizontal() ? maxSliderPos : (float)width * 0.5f,
                         slider.isHorizontal() ? (float)height * 0.5f : maxSliderPos };
        }
        else
        {
            auto kx = slider.isHorizontal() ? sliderPos : ((float)x + (float)width * 0.5f);
            auto ky = slider.isHorizontal() ? ((float)y + (float)height * 0.5f) : sliderPos;

            minPoint = startPoint;
            maxPoint = { kx, ky };
        }

        auto thumbWidth = getSliderThumbRadius(slider);

        valueTrack.startNewSubPath(minPoint);
        valueTrack.lineTo(isThreeVal ? thumbPoint : maxPoint);
        g.setColour(Colour(64u, 194u, 230u));
        g.strokePath(valueTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

        if (!isTwoVal)
        {
            auto name = slider.getName();
            auto value = static_cast<String>(slider.getValue());
//            auto thumb = slider.getThumbBeingDragged();

            auto font = g.getCurrentFont();
            //auto newWidth = font.getStringWidth(name);
            auto newWidth = trackWidth * 1.5;
            auto whiteRect = Rectangle<float>(static_cast<float> (newWidth + 5), static_cast<float> (thumbWidth + 5)).withCentre(isThreeVal ? thumbPoint : maxPoint);
            auto thumbRect = whiteRect.reduced(1, 1);
            g.setColour(Colour(186u, 34u, 34u));
            g.fillRect(whiteRect);
            g.setColour(Colours::black);
            g.fillRect(thumbRect);

            g.setColour(juce::Colours::whitesmoke);
            //g.drawText(thumb ? name : value, thumbRect, Justification::centred, 1);
        }

        if (isTwoVal || isThreeVal)
        {
            auto sr = jmin(trackWidth, (slider.isHorizontal() ? (float)height : (float)width) * 0.4f);
            auto pointerColour = slider.findColour(Slider::thumbColourId);

            if (slider.isHorizontal())
            {
                drawPointer(g, minSliderPos - sr,
                    jmax(0.0f, (float)y + (float)height * 0.5f - trackWidth * 2.0f),
                    trackWidth * 2.0f, pointerColour, 2);

                drawPointer(g, maxSliderPos - trackWidth,
                    jmin((float)(y + height) - trackWidth * 2.0f, (float)y + (float)height * 0.5f),
                    trackWidth * 2.0f, pointerColour, 4);
            }
            else
            {
                drawPointer(g, jmax(0.0f, (float)x + (float)width * 0.5f - trackWidth * 2.0f),
                    minSliderPos - trackWidth,
                    trackWidth * 2.0f, pointerColour, 1);

                drawPointer(g, jmin((float)(x + width) - trackWidth * 2.0f, (float)x + (float)width * 0.5f), maxSliderPos - sr,
                    trackWidth * 2.0f, pointerColour, 3);
            }
        }
    }
}

//==============================================================================
void BYOI_lnf::drawButtonBackground (juce::Graphics& g,
                                           juce::Button& button,
                                           const juce::Colour& backgroundColour,
                                           bool shouldDrawButtonAsHighlighted,
                                           bool shouldDrawButtonAsDown)
{
    using namespace juce;
    ignoreUnused(backgroundColour, shouldDrawButtonAsDown, shouldDrawButtonAsHighlighted);

    auto fullBounds = button.getLocalBounds();
    auto cornerSize = fullBounds.getWidth() * .15;

    juce::Rectangle<float> r;

    r.setSize(fullBounds.getWidth(), fullBounds.getHeight());
    r.setCentre(fullBounds.getCentre().toFloat());
    r.translate(-1, 0);

    auto color = button.getToggleState() ? juce::Colour(64u, 194u, 230u) : juce::Colours::dimgrey;
    g.setColour(color);
    g.fillRoundedRectangle(r, cornerSize);

    auto bounds = r.toFloat();
    g.setColour(juce::Colours::black);
    g.drawRoundedRectangle(bounds.getCentreX() - bounds.getWidth() / 2, bounds.getCentreY() - bounds.getHeight() / 2, bounds.getWidth(), bounds.getHeight(), cornerSize, 2);

}
