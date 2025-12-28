#include "PluginEditor.h"

ChronosAudioProcessorEditor::ChronosAudioProcessorEditor(ChronosAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    setLookAndFeel(&lookAndFeel);

    // Load logo
    logoImage = juce::ImageCache::getFromMemory(BinaryData::company_logo_png,
                                                 BinaryData::company_logo_pngSize);

    // Add visualizers
    addAndMakeVisible(timeDisplay);
    addAndMakeVisible(feedbackMeter);

    // Configure sliders as rotary
    auto setupRotarySlider = [this](juce::Slider& slider) {
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 18);
        addAndMakeVisible(slider);
    };

    // Time controls
    setupRotarySlider(delayTimeSlider);
    tempoSyncButton.setButtonText("SYNC");
    addAndMakeVisible(tempoSyncButton);
    syncDivisionCombo.addItemList(Chronos::getDivisionNames(), 1);
    addAndMakeVisible(syncDivisionCombo);
    linkLRButton.setButtonText("LINK");
    addAndMakeVisible(linkLRButton);

    // Feedback controls
    setupRotarySlider(feedbackSlider);
    setupRotarySlider(dampingSlider);
    setupRotarySlider(driveSlider);
    setupRotarySlider(filterFreqSlider);
    setupRotarySlider(filterResSlider);
    filterModeCombo.addItemList({"LP", "HP", "BP"}, 1);
    addAndMakeVisible(filterModeCombo);

    // Modulation controls
    setupRotarySlider(modRateSlider);
    setupRotarySlider(modDepthSlider);
    modShapeCombo.addItemList({"Sine", "Triangle", "Random"}, 1);
    addAndMakeVisible(modShapeCombo);
    modSyncButton.setButtonText("SYNC");
    addAndMakeVisible(modSyncButton);

    // Stereo controls
    stereoModeCombo.addItemList({"Mono", "Stereo", "Ping-Pong", "Wide"}, 1);
    addAndMakeVisible(stereoModeCombo);
    setupRotarySlider(widthSlider);

    // Feature controls
    duckingButton.setButtonText("DUCK");
    addAndMakeVisible(duckingButton);
    setupRotarySlider(duckAmountSlider);
    freezeButton.setButtonText("FREEZE");
    addAndMakeVisible(freezeButton);

    // I/O controls
    setupRotarySlider(inputGainSlider);
    setupRotarySlider(outputGainSlider);
    setupRotarySlider(mixSlider);

    // Labels
    auto setupLabel = [this](juce::Label& label) {
        label.setFont(juce::Font(14.0f).boldened());
        label.setColour(juce::Label::textColourId, Chronos::Colors::textSecondary);
        label.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(label);
    };

    setupLabel(timeLabel);
    setupLabel(feedbackLabel);
    setupLabel(modulationLabel);
    setupLabel(outputLabel);

    // Attachments
    auto& apvts = processorRef.getAPVTS();

    delayTimeAttachment = std::make_unique<SliderAttachment>(apvts, Chronos::ParamIDs::delayTime, delayTimeSlider);
    tempoSyncAttachment = std::make_unique<ButtonAttachment>(apvts, Chronos::ParamIDs::tempoSync, tempoSyncButton);
    syncDivisionAttachment = std::make_unique<ComboAttachment>(apvts, Chronos::ParamIDs::syncDivision, syncDivisionCombo);
    linkLRAttachment = std::make_unique<ButtonAttachment>(apvts, Chronos::ParamIDs::linkLR, linkLRButton);

    feedbackAttachment = std::make_unique<SliderAttachment>(apvts, Chronos::ParamIDs::feedback, feedbackSlider);
    dampingAttachment = std::make_unique<SliderAttachment>(apvts, Chronos::ParamIDs::damping, dampingSlider);
    driveAttachment = std::make_unique<SliderAttachment>(apvts, Chronos::ParamIDs::drive, driveSlider);
    filterFreqAttachment = std::make_unique<SliderAttachment>(apvts, Chronos::ParamIDs::fbFilterFreq, filterFreqSlider);
    filterResAttachment = std::make_unique<SliderAttachment>(apvts, Chronos::ParamIDs::fbFilterRes, filterResSlider);
    filterModeAttachment = std::make_unique<ComboAttachment>(apvts, Chronos::ParamIDs::fbFilterMode, filterModeCombo);

    modRateAttachment = std::make_unique<SliderAttachment>(apvts, Chronos::ParamIDs::modRate, modRateSlider);
    modDepthAttachment = std::make_unique<SliderAttachment>(apvts, Chronos::ParamIDs::modDepth, modDepthSlider);
    modShapeAttachment = std::make_unique<ComboAttachment>(apvts, Chronos::ParamIDs::modShape, modShapeCombo);
    modSyncAttachment = std::make_unique<ButtonAttachment>(apvts, Chronos::ParamIDs::modSync, modSyncButton);

    stereoModeAttachment = std::make_unique<ComboAttachment>(apvts, Chronos::ParamIDs::stereoMode, stereoModeCombo);
    widthAttachment = std::make_unique<SliderAttachment>(apvts, Chronos::ParamIDs::width, widthSlider);

    duckingAttachment = std::make_unique<ButtonAttachment>(apvts, Chronos::ParamIDs::ducking, duckingButton);
    duckAmountAttachment = std::make_unique<SliderAttachment>(apvts, Chronos::ParamIDs::duckAmount, duckAmountSlider);
    freezeAttachment = std::make_unique<ButtonAttachment>(apvts, Chronos::ParamIDs::freeze, freezeButton);

    inputGainAttachment = std::make_unique<SliderAttachment>(apvts, Chronos::ParamIDs::inputGain, inputGainSlider);
    outputGainAttachment = std::make_unique<SliderAttachment>(apvts, Chronos::ParamIDs::outputGain, outputGainSlider);
    mixAttachment = std::make_unique<SliderAttachment>(apvts, Chronos::ParamIDs::mix, mixSlider);

    setSize(800, 560);
    startTimerHz(30);
}

ChronosAudioProcessorEditor::~ChronosAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void ChronosAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Background
    g.fillAll(Chronos::Colors::background);

    // Header area
    auto headerArea = getLocalBounds().removeFromTop(50);
    g.setColour(Chronos::Colors::panelDark);
    g.fillRect(headerArea);

    // Title
    g.setFont(juce::Font(24.0f).boldened());
    g.setColour(Chronos::Colors::timeBlue);
    g.drawText("CHRONOS", headerArea.reduced(20, 0), juce::Justification::centredLeft);

    g.setFont(juce::Font(12.0f));
    g.setColour(Chronos::Colors::textSecondary);
    g.drawText("DELAY", headerArea.reduced(20, 0).translated(0, 18), juce::Justification::centredLeft);

    // Logo
    if (logoImage.isValid())
    {
        auto logoArea = headerArea.removeFromRight(100).reduced(10);
        g.drawImage(logoImage, logoArea.toFloat(),
                    juce::RectanglePlacement::centred | juce::RectanglePlacement::onlyReduceInSize);
    }

    // Section dividers
    g.setColour(Chronos::Colors::timeBlue.withAlpha(0.2f));

    int dividerY = 180;
    g.drawHorizontalLine(dividerY, 10.0f, static_cast<float>(getWidth() - 10));

    int midX = getWidth() / 2;
    g.drawVerticalLine(midX, static_cast<float>(dividerY), static_cast<float>(getHeight() - 60));
}

void ChronosAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();

    // Header
    area.removeFromTop(50);

    // Time display
    auto visualizerArea = area.removeFromTop(120).reduced(10);
    timeDisplay.setBounds(visualizerArea.removeFromLeft(visualizerArea.getWidth() - 30));
    feedbackMeter.setBounds(visualizerArea.reduced(5, 10));

    // Main controls area
    auto controlsArea = area.reduced(15);
    int halfWidth = controlsArea.getWidth() / 2;
    int knobSize = 85;
    int rowHeight = 100;

    // Left column: TIME + MODULATION
    auto leftColumn = controlsArea.removeFromLeft(halfWidth - 10);

    // TIME section
    timeLabel.setBounds(leftColumn.removeFromTop(22));
    auto timeRow = leftColumn.removeFromTop(rowHeight);

    delayTimeSlider.setBounds(timeRow.removeFromLeft(knobSize));
    tempoSyncButton.setBounds(timeRow.removeFromLeft(70).reduced(5, 30));
    syncDivisionCombo.setBounds(timeRow.removeFromLeft(90).reduced(5, 35));
    linkLRButton.setBounds(timeRow.removeFromLeft(60).reduced(5, 30));

    leftColumn.removeFromTop(15);

    // MODULATION section
    modulationLabel.setBounds(leftColumn.removeFromTop(22));
    auto modRow = leftColumn.removeFromTop(rowHeight);

    modRateSlider.setBounds(modRow.removeFromLeft(knobSize));
    modDepthSlider.setBounds(modRow.removeFromLeft(knobSize));
    modShapeCombo.setBounds(modRow.removeFromLeft(90).reduced(5, 35));
    modSyncButton.setBounds(modRow.removeFromLeft(60).reduced(5, 30));

    // Right column: FEEDBACK + STEREO
    controlsArea.removeFromLeft(20);  // Gap
    auto rightColumn = controlsArea;

    // FEEDBACK section
    feedbackLabel.setBounds(rightColumn.removeFromTop(22));
    auto fbRow1 = rightColumn.removeFromTop(rowHeight);

    feedbackSlider.setBounds(fbRow1.removeFromLeft(knobSize));
    dampingSlider.setBounds(fbRow1.removeFromLeft(knobSize));
    driveSlider.setBounds(fbRow1.removeFromLeft(knobSize));
    filterModeCombo.setBounds(fbRow1.removeFromLeft(70).reduced(5, 35));

    auto fbRow2 = rightColumn.removeFromTop(rowHeight - 10);
    filterFreqSlider.setBounds(fbRow2.removeFromLeft(knobSize));
    filterResSlider.setBounds(fbRow2.removeFromLeft(knobSize));
    stereoModeCombo.setBounds(fbRow2.removeFromLeft(100).reduced(5, 30));
    widthSlider.setBounds(fbRow2.removeFromLeft(knobSize));

    // OUTPUT section (bottom)
    auto bottomArea = getLocalBounds().removeFromBottom(110).reduced(15);
    outputLabel.setBounds(bottomArea.removeFromTop(22));

    auto outputRow = bottomArea;
    inputGainSlider.setBounds(outputRow.removeFromLeft(knobSize));
    outputGainSlider.setBounds(outputRow.removeFromLeft(knobSize));
    mixSlider.setBounds(outputRow.removeFromLeft(knobSize));

    outputRow.removeFromLeft(30);
    duckingButton.setBounds(outputRow.removeFromLeft(70).reduced(5, 22));
    duckAmountSlider.setBounds(outputRow.removeFromLeft(knobSize));

    outputRow.removeFromLeft(30);
    freezeButton.setBounds(outputRow.removeFromLeft(90).reduced(5, 18));
}

void ChronosAudioProcessorEditor::timerCallback()
{
    // Update visualizers with current processor state
    timeDisplay.setDelayTime(delayTimeSlider.getValue());
    timeDisplay.setFeedbackLevel(processorRef.getFeedbackLevel());
    timeDisplay.setLFOPhase(processorRef.getLFOPhase());
    timeDisplay.setModDepth(static_cast<float>(modDepthSlider.getValue()) / 100.0f);
    timeDisplay.setTempoSync(tempoSyncButton.getToggleState());
    timeDisplay.setBPM(processorRef.getCurrentBPM());

    if (tempoSyncButton.getToggleState())
    {
        int divIndex = syncDivisionCombo.getSelectedItemIndex();
        if (divIndex >= 0)
            timeDisplay.setSyncDivisionName(Chronos::getDivisionName(static_cast<Chronos::SyncDivision>(divIndex)));
    }

    feedbackMeter.setLevel(processorRef.getFeedbackLevel());

    // Visual feedback for freeze state
    if (freezeButton.getToggleState())
    {
        freezeButton.setColour(juce::TextButton::buttonOnColourId, Chronos::Colors::freezePurple);
    }
}
