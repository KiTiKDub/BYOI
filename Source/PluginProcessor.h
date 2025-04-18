#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_formats/juce_audio_formats.h>
// #include 

//==============================================================================
class AudioPluginAudioProcessor final : public juce::AudioProcessor
{
public:
    //==============================================================================
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "parameters", createParameterLayout() };

private:

    juce::AudioParameterFloat* fadeIn{nullptr};
    juce::AudioParameterFloat* fadeOut{nullptr};
    juce::AudioParameterFloat* startIR{nullptr};
    juce::AudioParameterFloat* endIR{nullptr};
    juce::AudioParameterFloat* stretch{nullptr};
    juce::AudioParameterFloat* predelay{nullptr};
    juce::AudioParameterFloat* tone{nullptr};
    juce::AudioParameterFloat* feedback{nullptr};
    juce::AudioParameterFloat* mix{nullptr};
    
    juce::AudioParameterBool* reverse{nullptr};
    juce::AudioParameterBool* power{nullptr};

    juce::AudioFormatManager manager;
    std::unique_ptr<juce::AudioFormatReader> reader{ nullptr };
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
