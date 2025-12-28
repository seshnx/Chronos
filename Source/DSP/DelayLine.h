#pragma once

#include <vector>
#include <cmath>
#include <algorithm>

namespace Chronos {

class DelayLine
{
public:
    DelayLine() = default;

    void prepare(float sampleRate, float maxDelayMs)
    {
        this->sampleRate = sampleRate;
        this->maxDelayMs = maxDelayMs;

        // Calculate buffer size with some headroom
        int maxSamples = static_cast<int>(std::ceil(maxDelayMs * sampleRate / 1000.0f)) + 4;
        buffer.resize(static_cast<size_t>(maxSamples), 0.0f);
        writeIndex = 0;
    }

    void clear()
    {
        std::fill(buffer.begin(), buffer.end(), 0.0f);
        writeIndex = 0;
    }

    void write(float sample)
    {
        buffer[static_cast<size_t>(writeIndex)] = sample;
        writeIndex = (writeIndex + 1) % static_cast<int>(buffer.size());
    }

    // Linear interpolation - efficient for non-modulated delay
    float readLinear(float delayInSamples) const
    {
        float readPos = static_cast<float>(writeIndex) - delayInSamples;
        while (readPos < 0.0f)
            readPos += static_cast<float>(buffer.size());

        int index0 = static_cast<int>(readPos);
        int index1 = (index0 + 1) % static_cast<int>(buffer.size());
        float frac = readPos - static_cast<float>(index0);

        return buffer[static_cast<size_t>(index0)] * (1.0f - frac) +
               buffer[static_cast<size_t>(index1)] * frac;
    }

    // Cubic interpolation - smooth for modulated delay (prevents aliasing)
    float read(float delayInSamples) const
    {
        float readPos = static_cast<float>(writeIndex) - delayInSamples;
        while (readPos < 0.0f)
            readPos += static_cast<float>(buffer.size());

        int index1 = static_cast<int>(readPos);
        int index0 = (index1 - 1 + static_cast<int>(buffer.size())) % static_cast<int>(buffer.size());
        int index2 = (index1 + 1) % static_cast<int>(buffer.size());
        int index3 = (index1 + 2) % static_cast<int>(buffer.size());

        float frac = readPos - static_cast<float>(index1);

        float y0 = buffer[static_cast<size_t>(index0)];
        float y1 = buffer[static_cast<size_t>(index1)];
        float y2 = buffer[static_cast<size_t>(index2)];
        float y3 = buffer[static_cast<size_t>(index3)];

        // Cubic Hermite interpolation
        float c0 = y1;
        float c1 = 0.5f * (y2 - y0);
        float c2 = y0 - 2.5f * y1 + 2.0f * y2 - 0.5f * y3;
        float c3 = 0.5f * (y3 - y0) + 1.5f * (y1 - y2);

        return ((c3 * frac + c2) * frac + c1) * frac + c0;
    }

    float msToSamples(float ms) const
    {
        return ms * sampleRate / 1000.0f;
    }

    float getMaxDelayMs() const { return maxDelayMs; }
    float getSampleRate() const { return sampleRate; }

private:
    std::vector<float> buffer;
    int writeIndex = 0;
    float sampleRate = 44100.0f;
    float maxDelayMs = 2000.0f;
};

} // namespace Chronos
