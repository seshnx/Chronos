#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "UI/ChronosLookAndFeel.h"
#include "UI/TimeDisplay.h"
#include "UI/DelayMeter.h"
#include "Utils/Parameters.h"
#include "BinaryData.h"

class ChronosAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit ChronosAudioProcessorEditor(ChronosAudioProcessor&);
    ~ChronosAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    ChronosAudioProcessor& processorRef;
    Chronos::ChronosLookAndFeel lookAndFeel;

    // Header
    juce::Image logoImage;

    // Visualizers
    Chronos::TimeDisplay timeDisplay;
    Chronos::DelayMeter feedbackMeter;

    // Time controls
    juce::Slider delayTimeSlider;
    juce::ToggleButton tempoSyncButton;
    juce::ComboBox syncDivisionCombo;
    juce::ToggleButton linkLRButton;

    // Feedback controls
    juce::Slider feedbackSlider;
    juce::Slider dampingSlider;
    juce::Slider driveSlider;
    juce::Slider filterFreqSlider;
    juce::Slider filterResSlider;
    juce::ComboBox filterModeCombo;

    // Modulation controls
    juce::Slider modRateSlider;
    juce::Slider modDepthSlider;
    juce::ComboBox modShapeCombo;
    juce::ToggleButton modSyncButton;

    // Stereo controls
    juce::ComboBox stereoModeCombo;
    juce::Slider widthSlider;

    // Feature controls
    juce::ToggleButton duckingButton;
    juce::Slider duckAmountSlider;
    juce::ToggleButton freezeButton;

    // I/O controls
    juce::Slider inputGainSlider;
    juce::Slider outputGainSlider;
    juce::Slider mixSlider;

    // Labels
    juce::Label timeLabel{"", "TIME"};
    juce::Label feedbackLabel{"", "FEEDBACK"};
    juce::Label modulationLabel{"", "MODULATION"};
    juce::Label outputLabel{"", "OUTPUT"};

    // Attachments
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using ComboAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    std::unique_ptr<SliderAttachment> delayTimeAttachment;
    std::unique_ptr<ButtonAttachment> tempoSyncAttachment;
    std::unique_ptr<ComboAttachment> syncDivisionAttachment;
    std::unique_ptr<ButtonAttachment> linkLRAttachment;

    std::unique_ptr<SliderAttachment> feedbackAttachment;
    std::unique_ptr<SliderAttachment> dampingAttachment;
    std::unique_ptr<SliderAttachment> driveAttachment;
    std::unique_ptr<SliderAttachment> filterFreqAttachment;
    std::unique_ptr<SliderAttachment> filterResAttachment;
    std::unique_ptr<ComboAttachment> filterModeAttachment;

    std::unique_ptr<SliderAttachment> modRateAttachment;
    std::unique_ptr<SliderAttachment> modDepthAttachment;
    std::unique_ptr<ComboAttachment> modShapeAttachment;
    std::unique_ptr<ButtonAttachment> modSyncAttachment;

    std::unique_ptr<ComboAttachment> stereoModeAttachment;
    std::unique_ptr<SliderAttachment> widthAttachment;

    std::unique_ptr<ButtonAttachment> duckingAttachment;
    std::unique_ptr<SliderAttachment> duckAmountAttachment;
    std::unique_ptr<ButtonAttachment> freezeAttachment;

    std::unique_ptr<SliderAttachment> inputGainAttachment;
    std::unique_ptr<SliderAttachment> outputGainAttachment;
    std::unique_ptr<SliderAttachment> mixAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChronosAudioProcessorEditor)
};
