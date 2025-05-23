#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "juce_core/juce_core.h"
#include "juce_dsp/juce_dsp.h"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
{
    fadeIn = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("fadeIn"));
    fadeOut = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("fadeOut"));
    startIR = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("startIR"));
    endIR = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("endIR"));
    stretch = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("stretch"));
    predelay = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("predelay"));
    tone = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("tone"));
    feedback = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("feedback"));
    mix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("mix"));

    reverse = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("reverse"));
    tempo = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("tempo"));
    power = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("power"));

    manager.registerBasicFormats();

}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumInputChannels());

    convolver.reset();
    convolver.prepare(spec);

}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();

    juce::ignoreUnused (midiMessages, totalNumInputChannels);
    if(!power->get() || !monoHasFile)
        return;

    juce::AudioBuffer<float> irProcessed;

    auto block = juce::dsp::AudioBlock<float>(buffer);
    auto context = juce::dsp::ProcessContextReplacing<float>(block);

    convolver.process(context);

}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto tree = juce::ValueTree::readFromData(data, static_cast<size_t>(sizeInBytes));
    if (tree.isValid())
        apvts.replaceState(tree);
}

juce::AudioProcessorValueTreeState::ParameterLayout AudioPluginAudioProcessor::createParameterLayout()
{
    using namespace juce;
    AudioProcessorValueTreeState::ParameterLayout layout;
    auto zeroToOneRange = NormalisableRange<float>(0,100,.01);
    auto stretchRange = NormalisableRange<float>(25, 400, .01);
    auto predealyRangeNEEDSFIXING = NormalisableRange<float>(0,5000,.01,.3f); //this needs to change to be more logarithmic, will come back later. skew may fix
    auto toneRange = NormalisableRange<float>(-100,100,1); //Subject to change
    auto feedbackRange = NormalisableRange<float>(-100,100,1); //just in case tone changes

    layout.add(std::make_unique<AudioParameterFloat>(ParameterID{"fadeIn",1}, "Fade In", zeroToOneRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(ParameterID{"fadeOut",1}, "Fade Out", zeroToOneRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(ParameterID{"startIR",1}, "Start Position", zeroToOneRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(ParameterID{"endIR",1}, "End Position", zeroToOneRange, 100));
    layout.add(std::make_unique<AudioParameterFloat>(ParameterID{"stretch",1}, "Stretch", stretchRange, 100));
    layout.add(std::make_unique<AudioParameterFloat>(ParameterID{"predelay",1}, "preDelay Range", predealyRangeNEEDSFIXING, 0));
    layout.add(std::make_unique<AudioParameterFloat>(ParameterID{"tone",1}, "Tone", toneRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(ParameterID{"feedback",1}, "Feedback", feedbackRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>(ParameterID{"mix",1}, "Mix", zeroToOneRange, 100));

    layout.add(std::make_unique<AudioParameterBool>(ParameterID{"reverse",1}, "Reverse", false));
    layout.add(std::make_unique<AudioParameterBool>(ParameterID{"tempo",1}, "Tempo", false));
    layout.add(std::make_unique<AudioParameterBool>(ParameterID{"power",1}, "Power", true));


    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}

void AudioPluginAudioProcessor::loadAndSaveFile(const juce::String& path)
{
    audioFile = juce::File(path);
    reader.reset(manager.createReaderFor(audioFile));
    reader->sampleRate = getSampleRate();

    int length = static_cast<int>(reader->lengthInSamples);
    waveform.setSize(2,length);
    reader->read(&waveform, 0, length, 0, true, true);

    setMonoWaveform();
}

void AudioPluginAudioProcessor::setMonoWaveform()
{
    waveformMono.setSize(1, waveform.getNumSamples());
    waveformMono.clear();

    waveformMono.copyFrom(0, 0, waveform, 0, 0, waveform.getNumSamples());
    waveformMono.applyGain(.5f);
    waveformMono.addFrom(0,0, waveform, 1, 0, waveform.getNumSamples(), .5f);

    writeMonoToFile();
}

void AudioPluginAudioProcessor::writeMonoToFile()
{
    juce::WavAudioFormat format;
    auto directory = getMonoImpulseLocation();
    auto file = directory.getChildFile("mono_impulse.wav");
    if (file.exists())
        file.deleteFile();
    std::unique_ptr<juce::AudioFormatWriter> writer;
    writer.reset(format.createWriterFor(new juce::FileOutputStream(file),
        getSampleRate(),
        1,
        24,
        {},
        0));
    if (writer != nullptr)
        writer->writeFromAudioSampleBuffer(waveformMono, 0, waveformMono.getNumSamples());

    convolver.loadImpulseResponse(file, juce::dsp::Convolution::Stereo::no, juce::dsp::Convolution::Trim::no, 0);
}

juce::File AudioPluginAudioProcessor::getMonoImpulseLocation()
{
    #if JUCE_WINDOWS
        auto kitikFolder = juce::File::getSpecialLocation(juce::File::commonApplicationDataDirectory).getChildFile("Application Support").getChildFile("KiTiK Music");
    #elif JUCE_MAC
        auto kitikFolder  = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getChildFile("KiTiK Music");
    #elif JUCE_LINUX
        auto kitikFolder = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getChildFile("KiTiK Music");
    #endif

    auto pluginFolder = kitikFolder.getChildFile("BYOI");

    if (!pluginFolder.exists())
        pluginFolder.createDirectory();

    return pluginFolder;
}
