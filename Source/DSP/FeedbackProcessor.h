#pragma once

#include <cmath>
#include <algorithm>

namespace Chronos {

enum class FilterMode
{
    LowPass,
    HighPass,
    BandPass
};

class FeedbackProcessor
{
public:
    FeedbackProcessor() = default;

    void prepare(float sampleRate)
    {
        this->sampleRate = sampleRate;
        reset();
    }

    void reset()
    {
        // SVF state
        ic1eq = 0.0f;
        ic2eq = 0.0f;

        // Damping filter state
        dampState = 0.0f;
    }

    void setFilterParams(float frequencyHz, float resonance, FilterMode mode)
    {
        cutoffHz = std::clamp(frequencyHz, 20.0f, 20000.0f);
        this->resonance = std::clamp(resonance, 0.0f, 1.0f);
        filterMode = mode;
        updateCoefficients();
    }

    void setDamping(float damping)
    {
        // Damping: 0 = no HF rolloff, 1 = heavy rolloff
        // Maps to a simple 1-pole lowpass coefficient
        dampingAmount = std::clamp(damping, 0.0f, 1.0f);
        // Higher damping = lower cutoff
        float dampFreq = 20000.0f * (1.0f - dampingAmount * 0.95f);
        float x = std::exp(-2.0f * 3.14159265359f * dampFreq / sampleRate);
        dampCoeff = 1.0f - x;
    }

    float process(float input, float drive)
    {
        // Apply drive/saturation first
        float driven = applySaturation(input, drive);

        // Apply SVF filter
        float filtered = processFilter(driven);

        // Apply damping (simple 1-pole LP)
        dampState += dampCoeff * (filtered - dampState);

        return dampState;
    }

private:
    void updateCoefficients()
    {
        // SVF coefficients (Cytomic/Andrew Simper method)
        float g = std::tan(3.14159265359f * cutoffHz / sampleRate);
        float k = 2.0f - 2.0f * resonance;  // Q = 1/(2-2*res), so k = 2-2*res

        a1 = 1.0f / (1.0f + g * (g + k));
        a2 = g * a1;
        a3 = g * a2;
    }

    float processFilter(float input)
    {
        float v3 = input - ic2eq;
        float v1 = a1 * ic1eq + a2 * v3;
        float v2 = ic2eq + a2 * ic1eq + a3 * v3;

        ic1eq = 2.0f * v1 - ic1eq;
        ic2eq = 2.0f * v2 - ic2eq;

        switch (filterMode)
        {
            case FilterMode::LowPass:
                return v2;
            case FilterMode::HighPass:
                return input - (2.0f - 2.0f * resonance) * v1 - v2;
            case FilterMode::BandPass:
                return v1;
            default:
                return v2;
        }
    }

    float applySaturation(float input, float drive)
    {
        if (drive <= 0.0f)
            return input;

        // Soft saturation using tanh
        float driveAmount = 1.0f + drive * 4.0f;  // 1x to 5x gain into saturation
        float saturated = std::tanh(input * driveAmount);

        // Mix dry and saturated based on drive amount
        return input * (1.0f - drive) + saturated * drive;
    }

    float sampleRate = 44100.0f;

    // SVF state
    float ic1eq = 0.0f;
    float ic2eq = 0.0f;
    float a1 = 0.0f, a2 = 0.0f, a3 = 0.0f;

    // Filter params
    float cutoffHz = 8000.0f;
    float resonance = 0.0f;
    FilterMode filterMode = FilterMode::LowPass;

    // Damping (1-pole LP)
    float dampState = 0.0f;
    float dampCoeff = 0.1f;
    float dampingAmount = 0.0f;
};

} // namespace Chronos
