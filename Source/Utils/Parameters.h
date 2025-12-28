#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "TempoSync.h"

namespace Chronos {

namespace ParamIDs {
    // Time
    inline const juce::String delayTime       = "delayTime";
    inline const juce::String tempoSync       = "tempoSync";
    inline const juce::String syncDivision    = "syncDivision";
    inline const juce::String delayTimeR      = "delayTimeR";
    inline const juce::String linkLR          = "linkLR";

    // Feedback
    inline const juce::String feedback        = "feedback";
    inline const juce::String fbFilterFreq    = "fbFilterFreq";
    inline const juce::String fbFilterRes     = "fbFilterRes";
    inline const juce::String fbFilterMode    = "fbFilterMode";
    inline const juce::String damping         = "damping";
    inline const juce::String drive           = "drive";

    // Modulation
    inline const juce::String modRate         = "modRate";
    inline const juce::String modDepth        = "modDepth";
    inline const juce::String modShape        = "modShape";
    inline const juce::String modSync         = "modSync";
    inline const juce::String modSyncDiv      = "modSyncDiv";

    // Stereo
    inline const juce::String stereoMode      = "stereoMode";
    inline const juce::String width           = "width";

    // Features
    inline const juce::String ducking         = "ducking";
    inline const juce::String duckAmount      = "duckAmount";
    inline const juce::String freeze          = "freeze";

    // I/O
    inline const juce::String inputGain       = "inputGain";
    inline const juce::String outputGain      = "outputGain";
    inline const juce::String mix             = "mix";
}

class Parameters
{
public:
    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();

    // Attach to APVTS
    explicit Parameters(juce::AudioProcessorValueTreeState& apvts);

    // Raw parameter pointers for fast access
    std::atomic<float>* delayTime       = nullptr;
    std::atomic<float>* tempoSync       = nullptr;
    std::atomic<float>* syncDivision    = nullptr;
    std::atomic<float>* delayTimeR      = nullptr;
    std::atomic<float>* linkLR          = nullptr;

    std::atomic<float>* feedback        = nullptr;
    std::atomic<float>* fbFilterFreq    = nullptr;
    std::atomic<float>* fbFilterRes     = nullptr;
    std::atomic<float>* fbFilterMode    = nullptr;
    std::atomic<float>* damping         = nullptr;
    std::atomic<float>* drive           = nullptr;

    std::atomic<float>* modRate         = nullptr;
    std::atomic<float>* modDepth        = nullptr;
    std::atomic<float>* modShape        = nullptr;
    std::atomic<float>* modSync         = nullptr;
    std::atomic<float>* modSyncDiv      = nullptr;

    std::atomic<float>* stereoMode      = nullptr;
    std::atomic<float>* width           = nullptr;

    std::atomic<float>* ducking         = nullptr;
    std::atomic<float>* duckAmount      = nullptr;
    std::atomic<float>* freeze          = nullptr;

    std::atomic<float>* inputGain       = nullptr;
    std::atomic<float>* outputGain      = nullptr;
    std::atomic<float>* mix             = nullptr;
};

} // namespace Chronos
