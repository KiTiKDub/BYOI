#pragma once

#include "PluginProcessor.h"
#include "BinaryData.h"
#include "juce_core/juce_core.h"
#include "KitikUtility/GUI/LookAndFeel.h"
#include "KitikUtility/GUI/RotarySliderWithLabels.h"
#include "GUI/DragAndDrop.h"

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void updateRSWL();

    AudioPluginAudioProcessor& audioProcessor;

    kitik::Laf lnf;

    DragAndDropComp dragAndDrop{ audioProcessor };

    juce::URL url{ "https://kwhaley5.gumroad.com/" };
    juce::HyperlinkButton gumroad{ "Gumroad", url };

    juce::TextButton openBrowser{"Open Browser"};

    std::unique_ptr<kitik::RotarySliderWithLabels> tone, feedback, mix;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> toneAT, feedbackAT, mixAT;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
