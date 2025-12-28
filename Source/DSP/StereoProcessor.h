#pragma once

#include <cmath>
#include <algorithm>

namespace Chronos {

enum class StereoMode
{
    Mono,       // Both channels get same delay
    Stereo,     // Independent L/R with offset
    PingPong,   // Alternating L/R bounces
    Wide        // M/S processing for width
};

class StereoProcessor
{
public:
    StereoProcessor() = default;

    void setMode(StereoMode mode)
    {
        this->mode = mode;
    }

    void setWidth(float width)
    {
        // Width: 0 = mono, 1 = stereo, 2 = extra wide
        this->width = std::clamp(width, 0.0f, 2.0f);
    }

    void setOffset(float offsetMs)
    {
        // Right channel time offset (for stereo mode)
        this->offsetMs = offsetMs;
    }

    // Process stereo pair - modifies L and R in place
    void process(float& left, float& right)
    {
        switch (mode)
        {
            case StereoMode::Mono:
            {
                float mono = (left + right) * 0.5f;
                left = mono;
                right = mono;
                break;
            }

            case StereoMode::Stereo:
                // L/R stay independent, offset handled by delay engine
                break;

            case StereoMode::PingPong:
                // Ping-pong is handled in delay engine feedback routing
                break;

            case StereoMode::Wide:
            {
                // M/S processing
                float mid = (left + right) * 0.5f;
                float side = (left - right) * 0.5f;

                // Adjust side level based on width
                side *= width;

                // Convert back to L/R
                left = mid + side;
                right = mid - side;
                break;
            }
        }
    }

    // For ping-pong mode: swap feedback signals
    void processPingPongFeedback(float& leftFeedback, float& rightFeedback)
    {
        if (mode == StereoMode::PingPong)
        {
            // Swap: left output feeds right delay, right feeds left
            float temp = leftFeedback;
            leftFeedback = rightFeedback;
            rightFeedback = temp;
        }
    }

    StereoMode getMode() const { return mode; }
    float getWidth() const { return width; }
    float getOffset() const { return offsetMs; }

private:
    StereoMode mode = StereoMode::Stereo;
    float width = 1.0f;
    float offsetMs = 0.0f;
};

} // namespace Chronos
