#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "ChronosLookAndFeel.h"
#include <cmath>

namespace Chronos {

class DelayMeter : public juce::Component, private juce::Timer
{
public:
    DelayMeter()
    {
        startTimerHz(30);
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat().reduced(2.0f);

        // Background
        g.setColour(Colors::panelDark);
        g.fillRoundedRectangle(bounds, 4.0f);

        // Meter area
        auto meterBounds = bounds.reduced(3.0f);
        float meterHeight = meterBounds.getHeight() * smoothedLevel;

        // Gradient fill based on level
        juce::Colour meterColour;
        if (smoothedLevel < 0.6f)
            meterColour = Colors::meterGreen;
        else if (smoothedLevel < 0.85f)
            meterColour = Colors::meterYellow;
        else
            meterColour = Colors::meterRed;

        // Draw meter from bottom up
        auto fillBounds = meterBounds;
        fillBounds.removeFromTop(meterBounds.getHeight() - meterHeight);

        g.setColour(meterColour);
        g.fillRoundedRectangle(fillBounds, 2.0f);

        // Peak indicator
        if (peakLevel > 0.01f)
        {
            float peakY = meterBounds.getBottom() - meterBounds.getHeight() * peakLevel;
            g.setColour(Colors::textPrimary);
            g.fillRect(meterBounds.getX(), peakY - 1.0f, meterBounds.getWidth(), 2.0f);
        }

        // Border
        g.setColour(Colors::timeBlue.withAlpha(0.3f));
        g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
    }

    void setLevel(float level)
    {
        currentLevel = std::clamp(level, 0.0f, 1.0f);

        if (currentLevel > peakLevel)
        {
            peakLevel = currentLevel;
            peakHoldCounter = peakHoldTime;
        }
    }

private:
    void timerCallback() override
    {
        // Smooth the level display
        float targetLevel = currentLevel;
        if (targetLevel > smoothedLevel)
            smoothedLevel = smoothedLevel * 0.7f + targetLevel * 0.3f;
        else
            smoothedLevel = smoothedLevel * 0.9f + targetLevel * 0.1f;

        // Peak hold and decay
        if (peakHoldCounter > 0)
        {
            peakHoldCounter--;
        }
        else
        {
            peakLevel *= 0.95f;
        }

        repaint();
    }

    float currentLevel = 0.0f;
    float smoothedLevel = 0.0f;
    float peakLevel = 0.0f;
    int peakHoldCounter = 0;
    static constexpr int peakHoldTime = 30;  // ~1 second at 30Hz
};

} // namespace Chronos
