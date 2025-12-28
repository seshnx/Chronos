#pragma once

#include <cmath>
#include <algorithm>

namespace Chronos {

class DuckingEnvelope
{
public:
    DuckingEnvelope() = default;

    void prepare(float sampleRate)
    {
        this->sampleRate = sampleRate;
        setAttackRelease(10.0f, 150.0f);  // Fast attack, medium release
        reset();
    }

    void reset()
    {
        envelope = 0.0f;
    }

    void setAttackRelease(float attackMs, float releaseMs)
    {
        attackCoeff = std::exp(-1.0f / (sampleRate * attackMs / 1000.0f));
        releaseCoeff = std::exp(-1.0f / (sampleRate * releaseMs / 1000.0f));
    }

    // Process input level and return duck amount (0 = no duck, 1 = full duck)
    float process(float inputLevel)
    {
        float absLevel = std::abs(inputLevel);

        // Envelope follower with asymmetric attack/release
        if (absLevel > envelope)
            envelope = attackCoeff * envelope + (1.0f - attackCoeff) * absLevel;
        else
            envelope = releaseCoeff * envelope + (1.0f - releaseCoeff) * absLevel;

        return envelope;
    }

    // Apply ducking to wet signal based on envelope and duck amount
    float applyDucking(float wetSignal, float duckAmount)
    {
        // duckAmount: 0 = no ducking, 1 = full ducking
        // When envelope is high, reduce wet signal
        float duckGain = 1.0f - (envelope * duckAmount);
        duckGain = std::clamp(duckGain, 0.0f, 1.0f);
        return wetSignal * duckGain;
    }

    float getEnvelope() const { return envelope; }

private:
    float sampleRate = 44100.0f;
    float envelope = 0.0f;
    float attackCoeff = 0.0f;
    float releaseCoeff = 0.0f;
};

} // namespace Chronos
