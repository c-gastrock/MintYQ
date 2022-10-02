/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace juce;

//==============================================================================
ChaseGP02MintYQAudioProcessorEditor::ChaseGP02MintYQAudioProcessorEditor (ChaseGP02MintYQAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

    // Setup your sliders and other gui components - - - -
    auto& params = processor.getParameters();

    // Cutoff Freq Slider
    AudioParameterFloat* audioParam = (AudioParameterFloat*)params.getUnchecked(0);
    mFilterFcSlider.setBounds(0, 0, 100, 160);
    mFilterFcSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    mFilterFcSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 70, 20);
    mFilterFcSlider.setRange(audioParam->range.start, audioParam->range.end);
    mFilterFcSlider.setValue(*audioParam);
    mFilterFcSlider.addListener(this);
    addAndMakeVisible(mFilterFcSlider);

    // Q slider
    audioParam = (AudioParameterFloat*)params.getUnchecked(1);
    mFilterQSlider.setBounds(0, 190, 100, 80);
    mFilterQSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    //mFilterQSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    mFilterQSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 70, 20);
    mFilterQSlider.setRange(audioParam->range.start, audioParam->range.end);
    mFilterQSlider.setValue(*audioParam);
    mFilterQSlider.addListener(this);
    addAndMakeVisible(mFilterQSlider);
}

ChaseGP02MintYQAudioProcessorEditor::~ChaseGP02MintYQAudioProcessorEditor()
{
}

//==============================================================================
void ChaseGP02MintYQAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void ChaseGP02MintYQAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void ChaseGP02MintYQAudioProcessorEditor::sliderValueChanged(Slider* slider) {
    auto& params = processor.getParameters();

    if (slider == &mFilterFcSlider) {
        AudioParameterFloat* audioParam = (AudioParameterFloat*)params.getUnchecked(0);
        *audioParam = mFilterFcSlider.getValue(); // set the AudioParameter
    }
    else if (slider == &mFilterQSlider) {
        AudioParameterFloat* audioParam = (AudioParameterFloat*)params.getUnchecked(1);
        *audioParam = mFilterQSlider.getValue(); // set the param
    };
}
