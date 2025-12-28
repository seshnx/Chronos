#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "ChronosLookAndFeel.h"
#include <array>
#include <cmath>

namespace Chronos {

class TimeDisplay : public juce::Component, private juce::Timer
{
public:
    TimeDisplay()
    {
        startTimerHz(30);
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat().reduced(2.0f);

        // Background
        g.setColour(Colors::panelDark);
        g.fillRoundedRectangle(bounds, 6.0f);

        // Border
        g.setColour(Colors::timeBlue.withAlpha(0.3f));
        g.drawRoundedRectangle(bounds, 6.0f, 1.0f);

        // Draw timeline
        auto timelineArea = bounds.reduced(20.0f, 30.0f);

        // Timeline base line
        float lineY = timelineArea.getCentreY();
        g.setColour(Colors::textSecondary.withAlpha(0.3f));
        g.drawLine(timelineArea.getX(), lineY, timelineArea.getRight(), lineY, 1.0f);

        // Draw delay taps (visualize feedback decay)
        float tapX = timelineArea.getX() + 20.0f;
        float spacing = (timelineArea.getWidth() - 40.0f) / 6.0f;

        for (int i = 0; i < 6; ++i)
        {
            float decay = std::pow(feedbackLevel, static_cast<float>(i + 1));
            float alpha = 0.2f + decay * 0.8f;
            float height = 10.0f + decay * 30.0f;

            // Tap marker
            g.setColour(Colors::timeBlue.withAlpha(alpha));
            g.fillRoundedRectangle(tapX + i * spacing - 3.0f, lineY - height / 2.0f,
                                   6.0f, height, 2.0f);

            // Glow effect on first tap
            if (i == 0)
            {
                for (int glow = 3; glow >= 0; --glow)
                {
                    g.setColour(Colors::timeBlue.withAlpha(0.05f * (4 - glow)));
                    g.fillEllipse(tapX - 8.0f - glow * 2.0f, lineY - 8.0f - glow * 2.0f,
                                  16.0f + glow * 4.0f, 16.0f + glow * 4.0f);
                }
            }
        }

        // Draw LFO modulation wave overlay
        if (modDepth > 0.01f)
        {
            juce::Path lfoPath;
            float waveHeight = timelineArea.getHeight() * 0.3f * modDepth;

            for (float x = 0.0f; x < timelineArea.getWidth(); x += 2.0f)
            {
                float phase = (x / timelineArea.getWidth()) * 2.0f * 3.14159265359f + lfoPhase * 2.0f * 3.14159265359f;
                float y = lineY + std::sin(phase) * waveHeight;

                if (x == 0.0f)
                    lfoPath.startNewSubPath(timelineArea.getX() + x, y);
                else
                    lfoPath.lineTo(timelineArea.getX() + x, y);
            }

            g.setColour(Colors::modCyan.withAlpha(0.4f));
            g.strokePath(lfoPath, juce::PathStrokeType(2.0f));
        }

        // Delay time readout
        g.setFont(juce::Font(16.0f).boldened());
        g.setColour(Colors::textPrimary);

        juce::String timeText;
        if (tempoSync && !syncDivisionName.isEmpty())
        {
            timeText = syncDivisionName + " (" + juce::String(static_cast<int>(delayTimeMs)) + " ms)";
        }
        else
        {
            timeText = juce::String(delayTimeMs, 1) + " ms";
        }

        g.drawText(timeText, bounds.reduced(10.0f).removeFromTop(24.0f),
                   juce::Justification::centred);

        // BPM display if tempo sync
        if (tempoSync)
        {
            g.setFont(juce::Font(11.0f));
            g.setColour(Colors::textSecondary);
            g.drawText(juce::String(bpm, 1) + " BPM",
                       bounds.reduced(10.0f).removeFromBottom(18.0f),
                       juce::Justification::centred);
        }
    }

    void setDelayTime(float ms) { delayTimeMs = ms; }
    void setFeedbackLevel(float level) { feedbackLevel = level; }
    void setLFOPhase(float phase) { lfoPhase = phase; }
    void setModDepth(float depth) { modDepth = depth; }
    void setTempoSync(bool sync) { tempoSync = sync; }
    void setSyncDivisionName(const juce::String& name) { syncDivisionName = name; }
    void setBPM(float bpmValue) { bpm = bpmValue; }

private:
    void timerCallback() override
    {
        repaint();
    }

    float delayTimeMs = 250.0f;
    float feedbackLevel = 0.3f;
    float lfoPhase = 0.0f;
    float modDepth = 0.0f;
    bool tempoSync = false;
    juce::String syncDivisionName;
    float bpm = 120.0f;
};

} // namespace Chronos
