/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

using namespace juce;

//==============================================================================
/**
*/
class ChaseGP02MintYQAudioProcessorEditor  : public AudioProcessorEditor, public Slider::Listener, public ComboBox::Listener, public Timer
{
public:
    ChaseGP02MintYQAudioProcessorEditor (ChaseGP02MintYQAudioProcessor&);
    ~ChaseGP02MintYQAudioProcessorEditor() override;

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    void sliderValueChanged(Slider* slider) override;
    void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;
    void timerCallback() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ChaseGP02MintYQAudioProcessor& audioProcessor;

    const static int numBands = 3; // in case wanted to add more bands as a feature
    Grid mainGrid;

    Slider filterFcSliders[numBands]; // Frequency Cutoff Knobs
    Slider filterQSliders[numBands]; // Resonance Knobs
    Slider filterGainSliders[numBands]; // Gain Knobs
    ComboBox filterTypeCBoxes[numBands]; // Filter Type ComboBoxes
    
    Label fcLabels[numBands];
    Label qLabels[numBands];
    Label gainLabels[numBands];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChaseGP02MintYQAudioProcessorEditor)
};
