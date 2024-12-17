/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class SparklelyzerAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    SparklelyzerAudioProcessorEditor (SparklelyzerAudioProcessor&);
    ~SparklelyzerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SparklelyzerAudioProcessor& audioProcessor;

    juce::Slider mixSlider;
    juce::Label mixLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;

    juce::Label saturationLabel;
    juce::Label frequencyLabel;

    juce::Slider freqSlider;
    juce::Slider harmSlider;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> freqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> harmAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SparklelyzerAudioProcessorEditor)
};
