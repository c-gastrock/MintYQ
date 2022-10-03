/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace juce;

//==============================================================================
ChaseGP02MintYQAudioProcessor::ChaseGP02MintYQAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    // Set up dict to map choice names to enum values
    stf["All Pass"] = APF;
    stf["Low Pass"] = LPF;
    stf["High Pass"] = HPF;
    stf["Low Shelf"] = LSF;
    stf["High Shelf"] = HSF;
    stf["Band Pass"] = BPF;
    stf["Peak"] = PF;

    for (int i = 0; i < numBands; i++) {
        addParameter(filterFcs[i] = new AudioParameterFloat("Filter " + std::to_string(i) + " Cutoff (Hz)", // parameterID,
            "Filt" + std::to_string(i) + "Fc", // parameterName,
            40.0f, // minValue,
            10000.0f, // maxValue,
            200.0f + 1000.0f * i)); // defaultValue

        addParameter(filterQs[i] = new AudioParameterFloat("Filter " + std::to_string(i) + " Q", // parameterID,
            "Filt" + std::to_string(i) + "Q", // parameterName,
            1.0f, // minValue,
            10.0f, // maxValue,
            3.0f)); // defaultValue

        addParameter(filterGains[i] = new AudioParameterFloat("Filter " + std::to_string(i) + " Gain (dB)", // parameterID,
            "Filt" + std::to_string(i) + "Gain", // parameterName,
            -60.0f, // minValue,
            10.0f, // maxValue,
            0.0f)); // defaultValue

        const StringArray choices = { "All Pass", "Low Pass", "High Pass", "Low Shelf", "High Shelf", "Band Pass", "Peak" };
        auto attributes = AudioParameterChoiceAttributes().withLabel("selected");

        addParameter(filterTypes[i] = new AudioParameterChoice("Filter " + std::to_string(i) + " Type", // parameterID,
            "Filt" + std::to_string(i) + "Type", // parameterName,
            choices, // Choice list,
            0, // default (APF),
            attributes));
    }
}

ChaseGP02MintYQAudioProcessor::~ChaseGP02MintYQAudioProcessor()
{
}

//==============================================================================
const String ChaseGP02MintYQAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ChaseGP02MintYQAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ChaseGP02MintYQAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ChaseGP02MintYQAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ChaseGP02MintYQAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ChaseGP02MintYQAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ChaseGP02MintYQAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ChaseGP02MintYQAudioProcessor::setCurrentProgram (int index)
{
}

const String ChaseGP02MintYQAudioProcessor::getProgramName (int index)
{
    return {};
}

void ChaseGP02MintYQAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void ChaseGP02MintYQAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    this->sampleRate = sampleRate;
}

void ChaseGP02MintYQAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ChaseGP02MintYQAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void ChaseGP02MintYQAudioProcessor::getFilterCoeffs(float* coeffs, String fType, float fc, float gain, float q) {

    switch (stf[fType.toStdString()]) {
        case APF:
            Mu45FilterCalc::calcCoeffsAPF(coeffs, fc, 0.0f, sampleRate);
            break;
        case LPF:
            Mu45FilterCalc::calcCoeffsLPF(coeffs, fc, q, sampleRate);
            break;
        case HPF:
            Mu45FilterCalc::calcCoeffsHPF(coeffs, fc, q, sampleRate);
            break;
        case LSF:
            Mu45FilterCalc::calcCoeffsLowShelf(coeffs, fc, gain, sampleRate);
            break;
        case HSF:
            Mu45FilterCalc::calcCoeffsHighShelf(coeffs, fc, gain, sampleRate);
            break;
        case BPF:
            Mu45FilterCalc::calcCoeffsBPF(coeffs, fc, q, sampleRate);
            break;
        case PF:
            Mu45FilterCalc::calcCoeffsPeak(coeffs, fc, gain, q, sampleRate);
    }
}

void ChaseGP02MintYQAudioProcessor::calcAlgorithmParams() {
    float fc;
    float q;
    float gain;
    String fType;

    for (int i = 0; i < numBands; i++) {
        fc = filterFcs[i]->get();
        q = filterQs[i]->get();
        gain = filterGains[i]->get();
        fType = filterTypes[i]->getCurrentChoiceName();

        // Calculate filter coefficients
        float coeffs[5]; // an array of 5 floats for filter coeffs: [b0, b1, b2, a1, a2]
        getFilterCoeffs(coeffs, fType, fc, gain, q);

        // Set the coefficients for each filter
        filterLs[i].setCoefficients(coeffs[0], coeffs[1], coeffs[2], coeffs[3], coeffs[4]);
        filterRs[i].setCoefficients(coeffs[0], coeffs[1], coeffs[2], coeffs[3], coeffs[4]);
    }
}

void ChaseGP02MintYQAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    calcAlgorithmParams();
    auto* channelL = buffer.getWritePointer(0);
    auto* channelR = buffer.getWritePointer(1);

    for (int samp = 0; samp < buffer.getNumSamples(); samp++)
    {
        // Apply each filter in sequence
        for (int i = 0; i < numBands; i++) {
            channelL[samp] = filterLs[i].tick(channelL[samp]);
            channelR[samp] = filterRs[i].tick(channelR[samp]);
        }
    }
}

//==============================================================================
bool ChaseGP02MintYQAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* ChaseGP02MintYQAudioProcessor::createEditor()
{
    return new ChaseGP02MintYQAudioProcessorEditor (*this);
}

//==============================================================================
void ChaseGP02MintYQAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ChaseGP02MintYQAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChaseGP02MintYQAudioProcessor();
}
