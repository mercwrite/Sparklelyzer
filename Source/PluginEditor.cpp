/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SparklelyzerAudioProcessorEditor::SparklelyzerAudioProcessorEditor (SparklelyzerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    addAndMakeVisible(mixLabel);
    mixLabel.setText("Mix", juce::dontSendNotification);
    mixLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    mixLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    mixLabel.setJustificationType(juce::Justification::centred);

    // Mix slider
    addAndMakeVisible(mixSlider);
    mixSlider.setSliderStyle(juce::Slider::Rotary);
    mixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    mixSlider.setTextValueSuffix("%");
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "mix", mixSlider);

    addAndMakeVisible(frequencyLabel);
    frequencyLabel.setText("Frequency", juce::dontSendNotification);
    frequencyLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    frequencyLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    frequencyLabel.setJustificationType(juce::Justification::centred);

 
    // Frequency slider
    addAndMakeVisible(freqSlider);
    freqSlider.setSliderStyle(juce::Slider::Rotary);
    freqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    freqSlider.setRange(20.0f, 20000.0f);
    freqSlider.setSkewFactorFromMidPoint(1000.0f);
    freqSlider.setTextValueSuffix(" Hz");
    freqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "frequency", freqSlider);

    addAndMakeVisible(saturationLabel);
    saturationLabel.setText("Drive", juce::dontSendNotification);
    saturationLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    saturationLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    saturationLabel.setJustificationType(juce::Justification::centred);

    // Harmonics slider
    addAndMakeVisible(harmSlider);
    harmSlider.setSliderStyle(juce::Slider::Rotary);
    harmSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    harmAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "saturation", harmSlider);

    setSize(500, 400);
}

SparklelyzerAudioProcessorEditor::~SparklelyzerAudioProcessorEditor()
{
}

//==============================================================================
void SparklelyzerAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black); // Background color
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    //g.drawFittedText("High-End Sparkle Plugin", getLocalBounds(), juce::Justification::centredTop, 1);
}

void SparklelyzerAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    auto knobWidth = 100;
    auto knobHeight = 100;

    mixSlider.setBounds(350, 100, knobWidth, knobHeight);
    mixLabel.setBounds(350, 200, knobWidth, 20);
    // Position Saturation Knob
    harmSlider.setBounds(50, 100, knobWidth, knobHeight);
    saturationLabel.setBounds(50, 200, knobWidth, 20); // Below the knob

    // Position Frequency Knob
    freqSlider.setBounds(200, 100, knobWidth, knobHeight);
    frequencyLabel.setBounds(200, 200, knobWidth, 20); // Below the knob
}
