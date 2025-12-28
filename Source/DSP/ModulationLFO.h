#pragma once

#include <cmath>
#include <random>

namespace Chronos {

enum class LFOShape
{
    Sine,
    Triangle,
    Random  // Sample & Hold
};

class ModulationLFO
{
public:
    ModulationLFO()
        : randomEngine(std::random_device{}()),
          randomDist(-1.0f, 1.0f)
    {
    }

    void prepare(float sampleRate)
    {
        this->sampleRate = sampleRate;
        reset();
    }

    void reset()
    {
        phase = 0.0f;
        randomCurrent = 0.0f;
        randomTarget = randomDist(randomEngine);
    }

    void setShape(LFOShape newShape)
    {
        shape = newShape;
    }

    // Returns value in range [-1, 1]
    float process(float rateHz)
    {
        float output = 0.0f;

        switch (shape)
        {
            case LFOShape::Sine:
                output = std::sin(phase * 2.0f * 3.14159265359f);
                break;

            case LFOShape::Triangle:
                // Triangle wave from phase [0, 1]
                if (phase < 0.25f)
                    output = phase * 4.0f;
                else if (phase < 0.75f)
                    output = 1.0f - (phase - 0.25f) * 4.0f;
                else
                    output = -1.0f + (phase - 0.75f) * 4.0f;
                break;

            case LFOShape::Random:
                // Sample & Hold - update on phase reset
                output = randomCurrent;
                break;
        }

        // Advance phase
        float phaseIncrement = rateHz / sampleRate;
        float oldPhase = phase;
        phase += phaseIncrement;

        // Handle phase wraparound
        if (phase >= 1.0f)
        {
            phase -= 1.0f;

            // For S&H, sample new random value on wrap
            if (shape == LFOShape::Random)
            {
                randomCurrent = randomTarget;
                randomTarget = randomDist(randomEngine);
            }
        }

        // Smooth S&H transition for less clicks
        if (shape == LFOShape::Random && oldPhase > phase)
        {
            // Just wrapped - output is now the new value
        }

        return output;
    }

    float getPhase() const { return phase; }
    LFOShape getShape() const { return shape; }

private:
    float phase = 0.0f;
    float sampleRate = 44100.0f;
    LFOShape shape = LFOShape::Sine;

    // Random S&H state
    std::mt19937 randomEngine;
    std::uniform_real_distribution<float> randomDist;
    float randomCurrent = 0.0f;
    float randomTarget = 0.0f;
};

} // namespace Chronos
