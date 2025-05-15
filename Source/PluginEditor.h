#pragma once

#include "PluginProcessor.h"
#include "BinaryData.h"
#include "juce_core/juce_core.h"
#include "GUI/BYOILnf.h"
#include "KitikUtility/GUI/RotarySliderWithLabels.h"
#include "GUI/DragAndDrop.h"
#include "juce_gui_basics/juce_gui_basics.h"

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

    BYOI_lnf lnf;

    DragAndDropComp dragAndDrop{ audioProcessor };

    juce::URL url{ "https://kwhaley5.gumroad.com/" };
    juce::HyperlinkButton gumroad{ "Gumroad", url };

    std::unique_ptr<kitik::RotarySliderWithLabels> tone, feedback, mix, fadeIn, stretch, fadeOut;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> toneAT, feedbackAT, mixAT, fadeInAT, stretchAT, fadeOutAT;

    juce::Slider preDelay{juce::Slider::SliderStyle::LinearBar, juce::Slider::TextEntryBoxPosition::NoTextBox};
    juce::AudioProcessorValueTreeState::SliderAttachment preDelayAT;

    juce::DrawableButton reverse{"", juce::DrawableButton::ButtonStyle::ImageOnButtonBackground}, tempo{"", juce::DrawableButton::ButtonStyle::ImageOnButtonBackground};
    juce::ToggleButton power;
    juce::TextButton upload{"Uplaod"};
    juce::AudioProcessorValueTreeState::ButtonAttachment reverseAT, tempoAT, powerAT;

    juce::Slider startIR{juce::Slider::SliderStyle::LinearBar, juce::Slider::TextEntryBoxPosition::NoTextBox},
                 endIR{juce::Slider::SliderStyle::LinearBar, juce::Slider::TextEntryBoxPosition:: NoTextBox};
    juce::AudioProcessorValueTreeState::SliderAttachment startIRAT, endIRAT;

    std::unique_ptr<juce::FileChooser> chooser;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
