#include "Parameters.h"

namespace Chronos {

juce::AudioProcessorValueTreeState::ParameterLayout Parameters::createLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // === TIME ===
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParamIDs::delayTime, 1),
        "Delay Time",
        juce::NormalisableRange<float>(1.0f, 2000.0f, 0.1f, 0.4f),  // Skewed for ms
        250.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(value, 1) + " ms"; },
        nullptr
    ));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(ParamIDs::tempoSync, 1),
        "Tempo Sync",
        false
    ));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID(ParamIDs::syncDivision, 1),
        "Sync Division",
        getDivisionNames(),
        9  // Default: 1/4
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParamIDs::delayTimeR, 1),
        "Delay Time R",
        juce::NormalisableRange<float>(1.0f, 2000.0f, 0.1f, 0.4f),
        250.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(value, 1) + " ms"; },
        nullptr
    ));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(ParamIDs::linkLR, 1),
        "Link L/R",
        true
    ));

    // === FEEDBACK ===
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParamIDs::feedback, 1),
        "Feedback",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        30.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(static_cast<int>(value)) + "%"; },
        nullptr
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParamIDs::fbFilterFreq, 1),
        "Filter Freq",
        juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.3f),
        8000.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) {
            if (value >= 1000.0f)
                return juce::String(value / 1000.0f, 1) + " kHz";
            return juce::String(static_cast<int>(value)) + " Hz";
        },
        nullptr
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParamIDs::fbFilterRes, 1),
        "Filter Resonance",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        0.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(static_cast<int>(value)) + "%"; },
        nullptr
    ));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID(ParamIDs::fbFilterMode, 1),
        "Filter Mode",
        juce::StringArray{"Low Pass", "High Pass", "Band Pass"},
        0
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParamIDs::damping, 1),
        "Damping",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        30.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(static_cast<int>(value)) + "%"; },
        nullptr
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParamIDs::drive, 1),
        "Drive",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        0.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(static_cast<int>(value)) + "%"; },
        nullptr
    ));

    // === MODULATION ===
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParamIDs::modRate, 1),
        "Mod Rate",
        juce::NormalisableRange<float>(0.01f, 20.0f, 0.01f, 0.4f),
        0.5f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(value, 2) + " Hz"; },
        nullptr
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParamIDs::modDepth, 1),
        "Mod Depth",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        0.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(static_cast<int>(value)) + "%"; },
        nullptr
    ));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID(ParamIDs::modShape, 1),
        "Mod Shape",
        juce::StringArray{"Sine", "Triangle", "Random"},
        0
    ));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(ParamIDs::modSync, 1),
        "Mod Sync",
        false
    ));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID(ParamIDs::modSyncDiv, 1),
        "Mod Sync Division",
        getDivisionNames(),
        6  // Default: 1/8
    ));

    // === STEREO ===
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID(ParamIDs::stereoMode, 1),
        "Stereo Mode",
        juce::StringArray{"Mono", "Stereo", "Ping-Pong", "Wide"},
        1
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParamIDs::width, 1),
        "Width",
        juce::NormalisableRange<float>(0.0f, 200.0f, 0.1f),
        100.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(static_cast<int>(value)) + "%"; },
        nullptr
    ));

    // === FEATURES ===
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(ParamIDs::ducking, 1),
        "Ducking",
        false
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParamIDs::duckAmount, 1),
        "Duck Amount",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        50.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(static_cast<int>(value)) + "%"; },
        nullptr
    ));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(ParamIDs::freeze, 1),
        "Freeze",
        false
    ));

    // === I/O ===
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParamIDs::inputGain, 1),
        "Input Gain",
        juce::NormalisableRange<float>(-24.0f, 12.0f, 0.1f),
        0.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(value, 1) + " dB"; },
        nullptr
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParamIDs::outputGain, 1),
        "Output Gain",
        juce::NormalisableRange<float>(-24.0f, 12.0f, 0.1f),
        0.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(value, 1) + " dB"; },
        nullptr
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ParamIDs::mix, 1),
        "Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        50.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(static_cast<int>(value)) + "%"; },
        nullptr
    ));

    return { params.begin(), params.end() };
}

Parameters::Parameters(juce::AudioProcessorValueTreeState& apvts)
{
    delayTime       = apvts.getRawParameterValue(ParamIDs::delayTime);
    tempoSync       = apvts.getRawParameterValue(ParamIDs::tempoSync);
    syncDivision    = apvts.getRawParameterValue(ParamIDs::syncDivision);
    delayTimeR      = apvts.getRawParameterValue(ParamIDs::delayTimeR);
    linkLR          = apvts.getRawParameterValue(ParamIDs::linkLR);

    feedback        = apvts.getRawParameterValue(ParamIDs::feedback);
    fbFilterFreq    = apvts.getRawParameterValue(ParamIDs::fbFilterFreq);
    fbFilterRes     = apvts.getRawParameterValue(ParamIDs::fbFilterRes);
    fbFilterMode    = apvts.getRawParameterValue(ParamIDs::fbFilterMode);
    damping         = apvts.getRawParameterValue(ParamIDs::damping);
    drive           = apvts.getRawParameterValue(ParamIDs::drive);

    modRate         = apvts.getRawParameterValue(ParamIDs::modRate);
    modDepth        = apvts.getRawParameterValue(ParamIDs::modDepth);
    modShape        = apvts.getRawParameterValue(ParamIDs::modShape);
    modSync         = apvts.getRawParameterValue(ParamIDs::modSync);
    modSyncDiv      = apvts.getRawParameterValue(ParamIDs::modSyncDiv);

    stereoMode      = apvts.getRawParameterValue(ParamIDs::stereoMode);
    width           = apvts.getRawParameterValue(ParamIDs::width);

    ducking         = apvts.getRawParameterValue(ParamIDs::ducking);
    duckAmount      = apvts.getRawParameterValue(ParamIDs::duckAmount);
    freeze          = apvts.getRawParameterValue(ParamIDs::freeze);

    inputGain       = apvts.getRawParameterValue(ParamIDs::inputGain);
    outputGain      = apvts.getRawParameterValue(ParamIDs::outputGain);
    mix             = apvts.getRawParameterValue(ParamIDs::mix);
}

} // namespace Chronos
