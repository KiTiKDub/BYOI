#pragma once
#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_audio_utils/juce_audio_utils.h"
#include "../PluginProcessor.h"

struct DragAndDropComp : public juce::Component, public juce::FileDragAndDropTarget
{
    DragAndDropComp(AudioPluginAudioProcessor&);
    ~DragAndDropComp() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;

private:

    std::unique_ptr<juce::FileChooser> chooser;

    void mouseDoubleClick(const juce::MouseEvent &event) override
    {
      juce::ignoreUnused(event);

      chooser = std::make_unique<juce::FileChooser>("Select a sample", juce::File::getSpecialLocation(juce::File::userDesktopDirectory), "*.wav", "*.mp3", "*.aiff");

      auto folderChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

      chooser->launchAsync(folderChooserFlags, [this](const juce::FileChooser &myChooser)
      {
        auto audioFile = myChooser.getResult();
        if(audioFile.exists())
        {
          audioProcessor.loadAndSaveFile(audioFile.getFullPathName());
          repaint();
        }
      });

    }

    AudioPluginAudioProcessor& audioProcessor;

    juce::AudioThumbnailCache thumbnailCache;
    juce::AudioThumbnail thumbnail;
};
