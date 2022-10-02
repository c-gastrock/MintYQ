/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Mu45FilterCalc/Mu45FilterCalc.h"
#include "StkLite-4.6.1/BiQuad.h"

using namespace juce;

extern const int numBands = 3; // in case wanted to add more bands as a feature

//==============================================================================
/**
*/
class ChaseGP02MintYQAudioProcessor  : public AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    ChaseGP02MintYQAudioProcessor();
    ~ChaseGP02MintYQAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:

    
    enum FilterType { LPF, HPF, LSF, HSF, BPF, PF };
    std::unordered_map<std::string, FilterType> stf; // string to filtertype

    // Mu45: User Parameters
    AudioParameterFloat* filterFcs[numBands];
    AudioParameterFloat* filterQs[numBands];
    AudioParameterFloat* filterGains[numBands];
    AudioParameterChoice* filterTypes[numBands];

    // Mu45: Objects we need
    stk::BiQuad filterLs[numBands], filterRs[numBands];

    float sampleRate;

    void calcAlgorithmParams();
    void getFilterCoeffs(float* coeffs, String fType, float fc, float gain, float q);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChaseGP02MintYQAudioProcessor)
};
