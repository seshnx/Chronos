#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "DSP/DelayEngine.h"
#include "Utils/Parameters.h"
#include "Utils/TempoSync.h"

class ChronosAudioProcessor : public juce::AudioProcessor
{
public:
    ChronosAudioProcessor();
    ~ChronosAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

    // For UI metering
    float getFeedbackLevel() const { return delayEngine.getFeedbackLevel(); }
    float getLFOPhase() const { return delayEngine.getLFOValue(); }
    float getCurrentBPM() const { return currentBPM; }

private:
    juce::AudioProcessorValueTreeState apvts;
    Chronos::Parameters params;
    Chronos::DelayEngine delayEngine;

    float currentBPM = 120.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChronosAudioProcessor)
};
