#pragma once

#include <JuceHeader.h>

using namespace juce;

class Filter
{
	public:
		const static enum FilterType { LPF, HPF, LSF, HSF, BPF, PF };

		Filter(AudioParameterFloat* cutoff, AudioParameterFloat* q, FilterType fType);
		~Filter();

		AudioParameterFloat* cutoff;
		AudioParameterFloat* q;
		FilterType fType;
};

