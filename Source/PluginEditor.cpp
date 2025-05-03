#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setLookAndFeel(&lnf);
    updateRSWL();
    setSize (300, 400);

    addAndMakeVisible(*tone);
    addAndMakeVisible(*feedback);
    addAndMakeVisible(*mix);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    auto newFont = juce::Font(juce::Typeface::createSystemTypefaceFor(BinaryData::offshore_ttf, BinaryData::offshore_ttfSize));
    g.setFont(newFont);

    g.fillAll(juce::Colours::black);

    auto logo = juce::ImageCache::getFromMemory(BinaryData::KITIK_LOGO_NO_BKGD_png, BinaryData::KITIK_LOGO_NO_BKGD_pngSize);
    //g.drawImage(logo, getLocalBounds().toFloat(), juce::RectanglePlacement::centred);

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    //g.drawFittedText ("KiTiK Plugin Template", getLocalBounds(), juce::Justification::centredTop, 1);

    g.drawLine(0,150,getWidth(),150,1.f);
    g.drawLine(0,325,getWidth(),325,1.f);

}

void AudioPluginAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    auto visualizerBounds = bounds.removeFromTop(150);
    auto knobBounds = bounds.removeFromBottom(75);

    auto toneBounds = knobBounds.removeFromLeft(knobBounds.getWidth() * .33);
    auto feedbackBounds = knobBounds.removeFromLeft(knobBounds.getWidth() * .5);
    auto mixBounds = knobBounds;

    tone->setBounds(toneBounds);
    feedback->setBounds(feedbackBounds);
    mix->setBounds(mixBounds);
}

void AudioPluginAudioProcessorEditor::updateRSWL()
{
    auto &toneParam = kitik::getParam(audioProcessor.apvts, "tone");
    auto &feedbackParam = kitik::getParam(audioProcessor.apvts, "feedback");
    auto &mixParam = kitik::getParam(audioProcessor.apvts, "mix");

    tone = std::make_unique<kitik::RotarySliderWithLabels>(&toneParam, "Sections", "tone");
    feedback = std::make_unique<kitik::RotarySliderWithLabels>(&feedbackParam, "", "feedback");
    mix = std::make_unique<kitik::RotarySliderWithLabels>(&mixParam, "", "mix");

    makeAttachment(toneAT, audioProcessor.apvts, "tone", *tone);
    makeAttachment(feedbackAT, audioProcessor.apvts, "feedback", *feedback);
    makeAttachment(mixAT, audioProcessor.apvts, "mix", *mix);

    addLabelPairs(tone->labels, 1, 3, toneParam, "");
    addLabelPairs(feedback->labels, 1, 3, feedbackParam, "");
    addLabelPairs(mix->labels, 1, 3, mixParam, "");

    tone.get()->onValueChange = [this, &toneParam]()
    {
        addLabelPairs(tone->labels, 1, 3, toneParam, "");
    };
    feedback.get()->onValueChange = [this, &feedbackParam]()
    {
        addLabelPairs(feedback->labels, 1, 3, feedbackParam, "");
    };
    mix.get()->onValueChange = [this, &mixParam]()
    {
        addLabelPairs(mix->labels, 1, 3, mixParam, "");
    };
}
