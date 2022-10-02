#include "Filter.h"

using namespace juce;

Filter::Filter(AudioParameterFloat* cutoff, AudioParameterFloat* q, FilterType fType) {
	this->cutoff = cutoff;
	this->q = q;
	this->fType = fType;
}

Filter::~Filter() {
	// TODO
}