/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SparklelyzerAudioProcessor::SparklelyzerAudioProcessor() 
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ) , parameters(*this, nullptr, "PARAMETERS",
                           {
                             std::make_unique<juce::AudioParameterFloat>("frequency", "Frequency", juce::NormalisableRange<float>(20.0f, 20000.f, 1.f, 0.25f), 750.f),
                             std::make_unique<juce::AudioParameterFloat>("saturation", "Saturation", 0.f, 10.f, 5.f),
                             std::make_unique<juce::AudioParameterFloat>("mix", "Mix", 0.f, 100.f, 100.f)})
#endif
{
}

SparklelyzerAudioProcessor::~SparklelyzerAudioProcessor()
{
}

//==============================================================================
const juce::String SparklelyzerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SparklelyzerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SparklelyzerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SparklelyzerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SparklelyzerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SparklelyzerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SparklelyzerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SparklelyzerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SparklelyzerAudioProcessor::getProgramName (int index)
{
    return {};
}

void SparklelyzerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}



//==============================================================================
void SparklelyzerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    highShelf.prepare(spec);
}

void SparklelyzerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SparklelyzerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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
#endif

void SparklelyzerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{

    auto frequency = parameters.getRawParameterValue("frequency")->load();
    auto saturationLevel = parameters.getRawParameterValue("saturation")->load();
    auto mix = parameters.getRawParameterValue("mix")->load();

    //Get mix as a percentage
    mix *= 0.01f;

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    //Prepare buffers for filtering
    juce::AudioBuffer<float> filteredBuffer;
    filteredBuffer.makeCopyOf(buffer);

    juce::dsp::AudioBlock<float> audioBlock(filteredBuffer);
    juce::dsp::ProcessContextReplacing<float> context(audioBlock);

    // Update high-shelf filter parameters dynamically
    auto coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(
        getSampleRate(), frequency, 0.707f
    );

    *highShelf.state = *coefficients;

    highShelf.process(context);


    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    for (int channel = 0; channel < filteredBuffer.getNumChannels(); ++channel) {
        auto* originalChannelData = buffer.getWritePointer(channel);
        auto* channelData = filteredBuffer.getWritePointer(channel);
        for (int sample = 0; sample < filteredBuffer.getNumSamples(); ++sample) {
            // Apply saturation transfer function
            originalChannelData[sample] -= channelData[sample];
            channelData[sample] = std::tanh(channelData[sample] * saturationLevel);
        }
    }

    // Mix the saturated signal back with the original signal
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        auto* originalChannelData = buffer.getWritePointer(channel);
        auto* filteredChannelData = filteredBuffer.getWritePointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
            //Add highshelf saturation to original signal
            filteredChannelData[sample] = originalChannelData[sample] + filteredChannelData[sample];
            // Blend original and saturated signals
            originalChannelData[sample] = originalChannelData[sample] * (1.0f - mix) + filteredChannelData[sample] * mix;
        }
    }
}

//==============================================================================
bool SparklelyzerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SparklelyzerAudioProcessor::createEditor()
{
    return new SparklelyzerAudioProcessorEditor (*this);
}

//==============================================================================
void SparklelyzerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SparklelyzerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SparklelyzerAudioProcessor();
}
