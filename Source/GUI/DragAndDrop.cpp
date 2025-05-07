#include "DragAndDrop.h"

DragAndDropComp::DragAndDropComp(AudioPluginAudioProcessor& p) : audioProcessor(p), thumbnailCache(1), thumbnail(512, p.manager, thumbnailCache)
{

}

DragAndDropComp::~DragAndDropComp()
{

}

void DragAndDropComp::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

    g.setColour(juce::Colours::whitesmoke);
    g.drawRect(getLocalBounds());

    auto normalFile = audioProcessor.getAudioFile();
    if(normalFile.exists())
    {
        thumbnail.setSource(new juce::FileInputSource(normalFile));
        thumbnail.drawChannel(g, getLocalBounds(), 0.0, thumbnail.getTotalLength(), 1, 1.f);
    }
    
    // auto audioFile = audioProcessor.getMonoImpulseLocation().getChildFile("mono_impulse.wav");
    // if(audioFile.exists())
    // {
    //     thumbnail.setSource(new juce::FileInputSource(audioFile));
    //     thumbnail.drawChannel(g, getLocalBounds(), 0.0, thumbnail.getTotalLength(), 1, 1.f);
    // }
}

void DragAndDropComp::resized()
{

}

bool DragAndDropComp::isInterestedInFileDrag(const juce::StringArray& files)
{
    for (auto file : files)
    {
        if (file.contains(".wav") || file.contains(".mp3") || file.contains(".aif"))
        {
        }
        else
        {
            return false;
        }
    }

    return true;
}

void DragAndDropComp::filesDropped(const juce::StringArray& files, int x, int y)
{
    juce::ignoreUnused(x, y);
    
    if (isInterestedInFileDrag(files))
    {
        for (auto file : files)
        {
            audioProcessor.loadAndSaveFile(file);
            repaint();
        }
    }
}