#pragma once

#include "DelayLine.h"
#include "ModulationLFO.h"
#include "FeedbackProcessor.h"
#include "DuckingEnvelope.h"
#include "StereoProcessor.h"
#include <array>
#include <vector>
#include <algorithm>

namespace Chronos {

static constexpr float MAX_DELAY_MS = 2000.0f;
static constexpr int FREEZE_BUFFER_SIZE = 88200;  // 2 seconds at 44.1kHz

class DelayEngine
{
public:
    DelayEngine() = default;

    void prepare(float sampleRate, int samplesPerBlock)
    {
        this->sampleRate = sampleRate;

        // Prepare delay lines
        for (auto& delay : delayLines)
            delay.prepare(sampleRate, MAX_DELAY_MS);

        // Prepare sub-processors
        lfo.prepare(sampleRate);
        for (auto& fb : feedbackProcessors)
            fb.prepare(sampleRate);
        ducker.prepare(sampleRate);

        // Prepare freeze buffers
        for (auto& buf : freezeBuffers)
        {
            buf.resize(FREEZE_BUFFER_SIZE, 0.0f);
            std::fill(buf.begin(), buf.end(), 0.0f);
        }
        freezeReadPos = 0;

        // Feedback state
        feedbackSamples[0] = 0.0f;
        feedbackSamples[1] = 0.0f;
    }

    void reset()
    {
        for (auto& delay : delayLines)
            delay.clear();

        lfo.reset();
        for (auto& fb : feedbackProcessors)
            fb.reset();
        ducker.reset();

        feedbackSamples[0] = 0.0f;
        feedbackSamples[1] = 0.0f;

        for (auto& buf : freezeBuffers)
            std::fill(buf.begin(), buf.end(), 0.0f);
    }

    struct Parameters
    {
        float delayTimeMs = 250.0f;
        float delayTimeRightMs = 250.0f;  // For stereo offset
        float feedback = 0.3f;
        float mix = 0.5f;

        // Modulation
        float modRateHz = 0.5f;
        float modDepth = 0.0f;

        // Feedback processing
        float filterFreq = 8000.0f;
        float filterRes = 0.0f;
        FilterMode filterMode = FilterMode::LowPass;
        float damping = 0.3f;
        float drive = 0.0f;

        // Stereo
        StereoMode stereoMode = StereoMode::Stereo;
        float width = 1.0f;

        // Features
        bool freeze = false;
        bool duckingEnabled = false;
        float duckAmount = 0.5f;

        // I/O
        float inputGain = 1.0f;
        float outputGain = 1.0f;
    };

    void setParameters(const Parameters& params)
    {
        currentParams = params;

        lfo.setShape(LFOShape::Sine);  // Will be controlled by param later
        stereoProc.setMode(params.stereoMode);
        stereoProc.setWidth(params.width);

        for (auto& fb : feedbackProcessors)
        {
            fb.setFilterParams(params.filterFreq, params.filterRes, params.filterMode);
            fb.setDamping(params.damping);
        }
    }

    void setLFOShape(LFOShape shape)
    {
        lfo.setShape(shape);
    }

    void process(float* leftChannel, float* rightChannel, int numSamples)
    {
        for (int i = 0; i < numSamples; ++i)
        {
            // Apply input gain
            float inL = leftChannel[i] * currentParams.inputGain;
            float inR = rightChannel[i] * currentParams.inputGain;

            // Get modulation
            float modValue = lfo.process(currentParams.modRateHz);
            float modOffset = modValue * currentParams.modDepth * 20.0f;  // +/- 20ms max

            // Calculate delay times
            float delayL = std::clamp(currentParams.delayTimeMs + modOffset, 1.0f, MAX_DELAY_MS);
            float delayR = std::clamp(currentParams.delayTimeRightMs + modOffset, 1.0f, MAX_DELAY_MS);

            float wetL, wetR;

            if (currentParams.freeze)
            {
                // Read from freeze buffer
                wetL = freezeBuffers[0][freezeReadPos];
                wetR = freezeBuffers[1][freezeReadPos];
                freezeReadPos = (freezeReadPos + 1) % static_cast<int>(freezeBuffers[0].size());
            }
            else
            {
                // Write input + feedback to delay lines
                float toWriteL = inL + feedbackSamples[0] * currentParams.feedback;
                float toWriteR = inR + feedbackSamples[1] * currentParams.feedback;

                // Soft limit feedback to prevent runaway
                toWriteL = softLimit(toWriteL);
                toWriteR = softLimit(toWriteR);

                delayLines[0].write(toWriteL);
                delayLines[1].write(toWriteR);

                // Read from delay lines with interpolation
                wetL = delayLines[0].read(delayLines[0].msToSamples(delayL));
                wetR = delayLines[1].read(delayLines[1].msToSamples(delayR));

                // Process feedback through filter/saturation
                float fbL = feedbackProcessors[0].process(wetL, currentParams.drive);
                float fbR = feedbackProcessors[1].process(wetR, currentParams.drive);

                // Apply ping-pong if needed
                stereoProc.processPingPongFeedback(fbL, fbR);

                feedbackSamples[0] = fbL;
                feedbackSamples[1] = fbR;

                // Update freeze buffer continuously
                updateFreezeBuffer(wetL, wetR);
            }

            // Apply ducking if enabled
            if (currentParams.duckingEnabled)
            {
                float inputLevel = std::max(std::abs(inL), std::abs(inR));
                ducker.process(inputLevel);
                wetL = ducker.applyDucking(wetL, currentParams.duckAmount);
                wetR = ducker.applyDucking(wetR, currentParams.duckAmount);
            }

            // Apply stereo width processing
            stereoProc.process(wetL, wetR);

            // Mix dry/wet
            float outL = inL * (1.0f - currentParams.mix) + wetL * currentParams.mix;
            float outR = inR * (1.0f - currentParams.mix) + wetR * currentParams.mix;

            // Apply output gain
            leftChannel[i] = outL * currentParams.outputGain;
            rightChannel[i] = outR * currentParams.outputGain;
        }
    }

    // For metering
    float getFeedbackLevel() const
    {
        return std::max(std::abs(feedbackSamples[0]), std::abs(feedbackSamples[1]));
    }

    float getLFOValue() const
    {
        return lfo.getPhase();
    }

private:
    void updateFreezeBuffer(float left, float right)
    {
        freezeBuffers[0][freezeWritePos] = left;
        freezeBuffers[1][freezeWritePos] = right;
        freezeWritePos = (freezeWritePos + 1) % static_cast<int>(freezeBuffers[0].size());
    }

    float softLimit(float x)
    {
        // Soft saturation to prevent feedback runaway
        if (x > 1.0f)
            return 1.0f + std::tanh(x - 1.0f);
        else if (x < -1.0f)
            return -1.0f + std::tanh(x + 1.0f);
        return x;
    }

    float sampleRate = 44100.0f;

    // Core delay lines (stereo)
    std::array<DelayLine, 2> delayLines;

    // Sub-processors
    ModulationLFO lfo;
    std::array<FeedbackProcessor, 2> feedbackProcessors;
    DuckingEnvelope ducker;
    StereoProcessor stereoProc;

    // Feedback state
    std::array<float, 2> feedbackSamples = {0.0f, 0.0f};

    // Freeze buffer
    std::array<std::vector<float>, 2> freezeBuffers;
    int freezeWritePos = 0;
    int freezeReadPos = 0;

    // Current parameters
    Parameters currentParams;
};

} // namespace Chronos
