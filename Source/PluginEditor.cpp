#include "PluginProcessor.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include <memory>
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p), preDelayAT(audioProcessor.apvts, "predelay", preDelay),
      reverseAT(audioProcessor.apvts, "reverse", reverse), tempoAT(audioProcessor.apvts, "tempo", tempo),
      powerAT(audioProcessor.apvts, "power", power), startIRAT(audioProcessor.apvts, "startIR", startIR),
      endIRAT(audioProcessor.apvts,"endIR", endIR)
{
    setLookAndFeel(&lnf);
    updateRSWL();
    setSize (300, 425);
    power.setComponentID("Power");
    startIR.setComponentID("Trim Left");
    endIR.setComponentID("Trim Right");
    upload.setButtonText("Upload IR");

    if (const auto svg = juce::XmlDocument::parse(BinaryData::reverse_svg))
    {
        const auto drawable = juce::Drawable::createFromSVG(*svg);
        reverse.setImages(drawable.get());
        reverse.setToggleable(true);
        reverse.setToggleState(false, juce::NotificationType::dontSendNotification);
        reverse.setClickingTogglesState(true);
    }
    if (const auto svgMusic = juce::XmlDocument::parse(BinaryData::music_note_svg))
    {
        const auto music = juce::Drawable::createFromSVG(*svgMusic);

        if(const auto svgClock = juce::XmlDocument::parse(BinaryData::clock_svg))
        {
            const auto clockDrawing = juce::Drawable::createFromSVG(*svgClock);
            tempo.setImages(clockDrawing.get(), nullptr, music.get(), nullptr, music.get(), nullptr);
            tempo.setToggleable(true);
            tempo.setToggleState(false, juce::NotificationType::dontSendNotification);
            tempo.setClickingTogglesState(true);
        }
    }

    upload.onStateChange = [this, &p](){

        chooser = std::make_unique<juce::FileChooser>("Select a sample", juce::File::getSpecialLocation(juce::File::userDesktopDirectory), "*.wav", "*.mp3", "*.aiff");

        auto folderChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

        chooser->launchAsync(folderChooserFlags, [this, &p](const juce::FileChooser &myChooser)
        {
          auto audioFile = myChooser.getResult();
          if(audioFile.exists())
          {
            p.loadAndSaveFile(audioFile.getFullPathName());
            repaint();
          }
        });
    };

    addAndMakeVisible(*tone);
    addAndMakeVisible(*feedback);
    addAndMakeVisible(*mix);
    addAndMakeVisible(*fadeIn);
    addAndMakeVisible(*stretch);
    addAndMakeVisible(*fadeOut);
    addAndMakeVisible(preDelay);
    addAndMakeVisible(reverse);
    addAndMakeVisible(tempo);
    addAndMakeVisible(power);
    addAndMakeVisible(gumroad);
    addAndMakeVisible(dragAndDrop);
    addAndMakeVisible(startIR);
    addAndMakeVisible(endIR);
    addAndMakeVisible(upload);
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

    auto masterBounds = getLocalBounds().removeFromTop(25);
    /*auto powerBounds */ masterBounds.removeFromLeft(masterBounds.getWidth() * .2);
    /*auto gumroadBounds =*/ masterBounds.removeFromRight(masterBounds.getWidth() * .25);
    /*auto textBounds =*/ masterBounds.removeFromRight(masterBounds.getWidth()* .25);

    auto logo = juce::ImageCache::getFromMemory(BinaryData::KITIK_LOGO_NO_BKGD_png, BinaryData::KITIK_LOGO_NO_BKGD_pngSize);
    //g.drawImage(logo, getLocalBounds().toFloat(), juce::RectanglePlacement::centred);

    g.setColour (juce::Colours::white);
    g.setFont (25.0f);
    g.drawFittedText ("BYOI", masterBounds, juce::Justification::centredTop, 1);

    g.drawLine(0,155,getWidth(),155,1.f);
    g.drawLine(0,350,getWidth(),350,1.f);

}

void AudioPluginAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    auto visualizerBounds = bounds.removeFromTop(180);
    auto masterBounds = visualizerBounds.removeFromTop(30);
    auto knobBounds = bounds.removeFromBottom(75);

    auto powerBounds = masterBounds.removeFromLeft(masterBounds.getWidth() * .1);
    auto gumroadBounds = masterBounds.removeFromRight(masterBounds.getWidth() * .25);
    auto uploadBounds = masterBounds.removeFromRight(masterBounds.getWidth()*.25);

    auto toneBounds = knobBounds.removeFromLeft(knobBounds.getWidth() * .33);
    auto feedbackBounds = knobBounds.removeFromLeft(knobBounds.getWidth() * .5);
    auto mixBounds = knobBounds;

    auto modifyVizBounds = bounds.removeFromTop(bounds.getHeight() * .5);

    auto fadeInBounds = modifyVizBounds.removeFromLeft(modifyVizBounds.getWidth() * .33);
    auto stretchBounds = modifyVizBounds.removeFromLeft(modifyVizBounds.getWidth() * .5);
    auto fadeOutBounds = modifyVizBounds;

    bounds.removeFromTop(bounds.getHeight() * .25);
    bounds.removeFromBottom(bounds.getHeight() * .33);

    auto reverseBounds = bounds.removeFromLeft(bounds.getWidth() * .25);
    reverseBounds.removeFromLeft(reverseBounds.getWidth() * .25);
    reverseBounds.removeFromRight(reverseBounds.getWidth() * .33);
    auto preDelayBounds = bounds.removeFromLeft(bounds.getWidth() * .66);
    auto tempoBounds = bounds;
    tempoBounds.removeFromLeft(tempoBounds.getWidth() * .25);
    tempoBounds.removeFromRight(tempoBounds.getWidth() * .33);

    auto leftVisualizer = visualizerBounds;
    leftVisualizer.removeFromRight(leftVisualizer.getWidth() * .5);
    leftVisualizer.setHeight(leftVisualizer.getHeight() * 1.05);

    auto rightVisualizer = visualizerBounds;
    rightVisualizer.removeFromLeft(rightVisualizer.getWidth() * .5);
    rightVisualizer.setHeight(rightVisualizer.getHeight() * 1.05);

    auto font = juce::Font();
    gumroad.setFont(font, false);

    tone->setBounds(toneBounds);
    feedback->setBounds(feedbackBounds);
    mix->setBounds(mixBounds);
    fadeIn->setBounds(fadeInBounds);
    stretch->setBounds(stretchBounds);
    fadeOut->setBounds(fadeOutBounds);
    reverse.setBounds(reverseBounds);
    preDelay.setBounds(preDelayBounds);
    tempo.setBounds(tempoBounds);
    power.setBounds(powerBounds);
    gumroad.setBounds(gumroadBounds);
    startIR.setBounds(leftVisualizer);
    dragAndDrop.setBounds(visualizerBounds);
    endIR.setBounds(rightVisualizer);
    upload.setBounds(uploadBounds);
}

void AudioPluginAudioProcessorEditor::updateRSWL()
{
    auto &toneParam = kitik::getParam(audioProcessor.apvts, "tone");
    auto &feedbackParam = kitik::getParam(audioProcessor.apvts, "feedback");
    auto &mixParam = kitik::getParam(audioProcessor.apvts, "mix");
    auto &fadeInParam = kitik::getParam(audioProcessor.apvts, "fadeIn");
    auto &stretchParam = kitik::getParam(audioProcessor.apvts, "stretch");
    auto &fadeOutParam = kitik::getParam(audioProcessor.apvts, "fadeOut");

    tone = std::make_unique<kitik::RotarySliderWithLabels>(&toneParam, "Sections", "tone");
    feedback = std::make_unique<kitik::RotarySliderWithLabels>(&feedbackParam, "", "feedback");
    mix = std::make_unique<kitik::RotarySliderWithLabels>(&mixParam, "", "mix");
    fadeIn = std::make_unique<kitik::RotarySliderWithLabels>(&fadeInParam, "Fade In", "fadeIn");
    stretch = std::make_unique<kitik::RotarySliderWithLabels>(&stretchParam, "Stretch", "stretch");
    fadeOut = std::make_unique<kitik::RotarySliderWithLabels>(&fadeOutParam, "Fade Out", "fadeOut");

    makeAttachment(toneAT, audioProcessor.apvts, "tone", *tone);
    makeAttachment(feedbackAT, audioProcessor.apvts, "feedback", *feedback);
    makeAttachment(mixAT, audioProcessor.apvts, "mix", *mix);
    makeAttachment(fadeInAT, audioProcessor.apvts, "fadeIn", *fadeIn);
    makeAttachment(stretchAT, audioProcessor.apvts, "stretch", *stretch);
    makeAttachment(fadeOutAT, audioProcessor.apvts, "fadeOut", *fadeOut);

    addLabelPairs(tone->labels, 1, 3, toneParam, "");
    addLabelPairs(feedback->labels, 1, 3, feedbackParam, "%");
    addLabelPairs(mix->labels, 1, 3, mixParam, "%");
    addLabelPairs(fadeIn->labels, 1, 3, fadeInParam, "%");
    addLabelPairs(stretch->labels, 1, 3, stretchParam, "%");
    addLabelPairs(fadeOut->labels, 1, 3, fadeOutParam, "%");

    tone.get()->onValueChange = [this, &toneParam]()
    {
        addLabelPairs(tone->labels, 1, 3, toneParam, "");
    };
    feedback.get()->onValueChange = [this, &feedbackParam]()
    {
        addLabelPairs(feedback->labels, 1, 3, feedbackParam, "%");
    };
    mix.get()->onValueChange = [this, &mixParam]()
    {
        addLabelPairs(mix->labels, 1, 3, mixParam, "%");
    };
    fadeIn.get()->onValueChange = [this, &fadeInParam]()
    {
        addLabelPairs(fadeIn->labels, 1, 3, fadeInParam, "%");
    };
    stretch.get()->onValueChange = [this, &stretchParam]()
    {
        addLabelPairs(stretch->labels, 1, 3, stretchParam, "%");
    };
    fadeOut.get()->onValueChange = [this, &fadeOutParam]()
    {
        addLabelPairs(fadeOut->labels, 1, 3, fadeOutParam, "%");
    };
}
