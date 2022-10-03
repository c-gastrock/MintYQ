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
    setSize (900, 190);

    // Setup your sliders and other gui components - - - -
    auto& params = processor.getParameters();
    AudioParameterFloat* audioParam;

    // Colors
    getLookAndFeel().setColour(Slider::thumbColourId, Colours::mintcream);
    getLookAndFeel().setColour(Slider::rotarySliderFillColourId, Colour::fromRGB(47, 91, 97));
    getLookAndFeel().setColour(Slider::rotarySliderOutlineColourId, Colour::fromRGB(83, 189, 125));
    getLookAndFeel().setColour(Slider::textBoxTextColourId, Colours::mintcream);
    getLookAndFeel().setColour(Slider::textBoxOutlineColourId, Colour::fromRGB(64, 128, 64));
    getLookAndFeel().setColour(Slider::textBoxBackgroundColourId, Colour::fromRGB(87, 173, 87));
    getLookAndFeel().setColour(Label::textColourId, Colour::fromRGB(32, 64, 32));
    getLookAndFeel().setColour(ComboBox::backgroundColourId, Colour::fromRGB(87, 173, 87));
    getLookAndFeel().setColour(ComboBox::outlineColourId, Colour::fromRGB(64, 128, 64));
    getLookAndFeel().setColour(PopupMenu::backgroundColourId, Colour::fromRGB(87, 173, 87));
    getLookAndFeel().setColour(PopupMenu::highlightedBackgroundColourId, Colour::fromRGB(47, 91, 97));

    for (int i = 0; i < numBands; i++) {

        // Cutoff knob gui
        audioParam = (AudioParameterFloat*)params.getUnchecked(i * 4);
        filterFcSliders[i].setRotaryParameters((5 * Mu45FilterCalc::myPI) / 4, (11 * Mu45FilterCalc::myPI) / 4, true);
        filterFcSliders[i].setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
        filterFcSliders[i].setTextBoxStyle(Slider::TextBoxBelow, false, 70, 20);
        filterFcSliders[i].setRange(audioParam->range.start, audioParam->range.end);
        filterFcSliders[i].setValue(audioParam->get(), dontSendNotification);
        filterFcSliders[i].setDoubleClickReturnValue(true, 0.0f);
        filterFcSliders[i].setNumDecimalPlacesToDisplay(0);
        filterFcSliders[i].setSkewFactorFromMidPoint(800.0f); // freq should be logarithmic, 800 roughly in center
        addAndMakeVisible(filterFcSliders[i]);
        filterFcSliders[i].addListener(this);

        fcLabels[i].attachToComponent(&filterFcSliders[i], false);
        fcLabels[i].setText("Cutoff (Hz)", dontSendNotification);
        fcLabels[i].setJustificationType(Justification::centred);
        addAndMakeVisible(fcLabels[i]);

        // Resonance knob gui
        audioParam = (AudioParameterFloat*)params.getUnchecked(i * 4 + 1);
        filterQSliders[i].setRotaryParameters((5 * Mu45FilterCalc::myPI) / 4, (11 * Mu45FilterCalc::myPI) / 4, true);
        filterQSliders[i].setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
        filterQSliders[i].setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);
        filterQSliders[i].setRange(audioParam->range.start, audioParam->range.end);
        filterQSliders[i].setValue(audioParam->get(), dontSendNotification);
        filterQSliders[i].setDoubleClickReturnValue(true, 0.0f);
        filterQSliders[i].setNumDecimalPlacesToDisplay(2);
        addAndMakeVisible(filterQSliders[i]);
        filterQSliders[i].addListener(this);

        qLabels[i].attachToComponent(&filterQSliders[i], false);
        qLabels[i].setText("Resonance", dontSendNotification);
        qLabels[i].setJustificationType(Justification::centred);
        addAndMakeVisible(qLabels[i]);
        
        // Gain knob gui
        audioParam = (AudioParameterFloat*)params.getUnchecked(i * 4 + 2);
        filterGainSliders[i].setRotaryParameters((5 * Mu45FilterCalc::myPI) / 4, (11 * Mu45FilterCalc::myPI) / 4, true);
        filterGainSliders[i].setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
        filterGainSliders[i].setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);
        filterGainSliders[i].setRange(audioParam->range.start, audioParam->range.end);
        filterGainSliders[i].setValue(audioParam->get(), dontSendNotification);
        filterGainSliders[i].setDoubleClickReturnValue(true, 0.0f);
        filterGainSliders[i].setNumDecimalPlacesToDisplay(2);
        filterGainSliders[i].setSkewFactorFromMidPoint(0.0f); // dB is logarithmic
        addAndMakeVisible(filterGainSliders[i]);
        filterGainSliders[i].addListener(this);

        gainLabels[i].attachToComponent(&filterGainSliders[i], false);
        gainLabels[i].setText("Gain (dB)", dontSendNotification);
        gainLabels[i].setJustificationType(Justification::centred);
        addAndMakeVisible(gainLabels[i]);

        // Loop through choices from parameter, more extensible than hardcoding
        AudioParameterChoice* choiceParam = (AudioParameterChoice*)params.getUnchecked(i * 4 + 3);
        int index = 1;
        for (auto s : choiceParam->choices) {
            filterTypeCBoxes[i].addItem(s, index++);
        }
        filterTypeCBoxes[i].setSelectedItemIndex(choiceParam->getIndex(), dontSendNotification);
        addAndMakeVisible(filterTypeCBoxes[i]);
        filterTypeCBoxes[i].addListener(this);

        startTimer(33); // For automation animation, roughly 30 FPS
    }
}

ChaseGP02MintYQAudioProcessorEditor::~ChaseGP02MintYQAudioProcessorEditor()
{
}

//==============================================================================
void ChaseGP02MintYQAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (Colours::lightgreen);

    g.setColour (Colours::black);
    g.setFont (15.0f);
}

void ChaseGP02MintYQAudioProcessorEditor::resized()
{
    mainGrid.justifyItems = Grid::JustifyItems::center;
    mainGrid.alignItems = Grid::AlignItems::center;

    using Track = Grid::TrackInfo;
    using Fr = Grid::Fr;

    mainGrid.templateRows = { Track(Fr(1)), Track(Fr(2)), Track(Fr(1))};
    mainGrid.templateColumns = { Track(Fr(2)), Track(Fr(2)), Track(Fr(2)), Track(Fr(1)),
        Track(Fr(2)), Track(Fr(2)), Track(Fr(2)), Track(Fr(1)),
        Track(Fr(2)), Track(Fr(2)), Track(Fr(2)) };

    // Spacers
    for (int i = 0; i < numBands; i++) {
        mainGrid.items.add(GridItem());
        mainGrid.items.add(GridItem(filterTypeCBoxes[i]).withWidth(100.0f).withHeight(25.0f));
        mainGrid.items.add(GridItem());
        mainGrid.items.add(GridItem());
    }
    mainGrid.items.removeLast();

    // Loop through to set up eq subcomponents
    for (int i = 0; i < numBands; i++) {
        mainGrid.items.add(GridItem(filterQSliders[i]).withWidth(66.0f).withHeight(66.0f));
        mainGrid.items.add(GridItem(filterFcSliders[i]).withWidth(100.0f).withHeight(100.0f));
        mainGrid.items.add(GridItem(filterGainSliders[i]).withWidth(66.0f).withHeight(66.0f));
        mainGrid.items.add(GridItem());
    }
    mainGrid.items.removeLast();

    // Spacers
    for (int i = 0; i < numBands; i++) {
        mainGrid.items.add(GridItem());
        mainGrid.items.add(GridItem(filterTypeCBoxes[i]).withWidth(100.0f).withHeight(25.0f));
        mainGrid.items.add(GridItem());
        mainGrid.items.add(GridItem());
    }

    mainGrid.performLayout(getLocalBounds());
}

void ChaseGP02MintYQAudioProcessorEditor::sliderValueChanged(Slider* slider) {
    auto& params = processor.getParameters();
    
    for (int i = 0; i < numBands; i ++) {
        // Check if slider is an FC Slider
        if (&filterFcSliders[i] == slider) { // If slider has same memory address as filterFcSliders[i], they are the same slider
            AudioParameterFloat* audioParam = (AudioParameterFloat*)params.getUnchecked(i*4); // i*4 b/c 4 params per band
            *audioParam = filterFcSliders[i].getValue();
        }
        // Check if slider is an Resonance Slider
        if (&filterQSliders[i] == slider) {
            AudioParameterFloat* audioParam = (AudioParameterFloat*)params.getUnchecked(i * 4 + 1); // Q is the 2nd param
            *audioParam = filterQSliders[i].getValue();
        }
        // Check if slider is an Gain Slider
        if (&filterGainSliders[i] == slider) {
            AudioParameterFloat* audioParam = (AudioParameterFloat*)params.getUnchecked(i * 4 + 2); // Gain is the 3rd param
            *audioParam = filterGainSliders[i].getValue();
        }
    }
}

void ChaseGP02MintYQAudioProcessorEditor::comboBoxChanged(ComboBox* comboBoxThatHasChanged) {
    auto& params = processor.getParameters();

    for (int i = 0; i < numBands; i++) {
        if (&filterTypeCBoxes[i] == comboBoxThatHasChanged) { // If CBox has same memory address as filterTypeCBoxes[i], they are the same CBox
            AudioParameterChoice* choiceParam = (AudioParameterChoice*)params.getUnchecked(i * 4 + 3); // Filter type is 4th param
            *choiceParam = filterTypeCBoxes[i].getSelectedItemIndex();
        }
    }
}

void ChaseGP02MintYQAudioProcessorEditor::timerCallback() {
    // Animated knobs and sliders for parameter automation

    auto& params = processor.getParameters();
    AudioParameterFloat* fc;
    AudioParameterFloat* q;
    AudioParameterFloat* gain;
    AudioParameterChoice* fType;

    for (int i = 0; i < numBands; i++) {
        fc = (AudioParameterFloat*)params.getUnchecked(i * 4);
        filterFcSliders[i].setValue(fc->get(), dontSendNotification);

        q = (AudioParameterFloat*)params.getUnchecked(i * 4 + 1);
        filterQSliders[i].setValue(q->get(), dontSendNotification);

        gain = (AudioParameterFloat*)params.getUnchecked(i * 4 + 2);
        filterGainSliders[i].setValue(gain->get(), dontSendNotification);

        fType = (AudioParameterChoice*)params.getUnchecked(i * 4 + 3);
        filterTypeCBoxes[i].setSelectedItemIndex(fType->getIndex(), dontSendNotification);
    }
}
