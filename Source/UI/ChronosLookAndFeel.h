#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace Chronos {

struct Colors
{
    // Background
    static inline const juce::Colour background      = juce::Colour(0xff0a0f14);
    static inline const juce::Colour panelDark       = juce::Colour(0xff121a24);
    static inline const juce::Colour panelLight      = juce::Colour(0xff1e2a38);

    // Accents
    static inline const juce::Colour timeBlue        = juce::Colour(0xff00aaff);
    static inline const juce::Colour modCyan         = juce::Colour(0xff00ffcc);
    static inline const juce::Colour feedbackOrange  = juce::Colour(0xffff8844);
    static inline const juce::Colour freezePurple    = juce::Colour(0xffaa44ff);

    // Standard
    static inline const juce::Colour textPrimary     = juce::Colour(0xffeef4ff);
    static inline const juce::Colour textSecondary   = juce::Colour(0xff8899aa);
    static inline const juce::Colour meterGreen      = juce::Colour(0xff00cc66);
    static inline const juce::Colour meterYellow     = juce::Colour(0xffffcc00);
    static inline const juce::Colour meterRed        = juce::Colour(0xffff4444);
};

class ChronosLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ChronosLookAndFeel()
    {
        setColour(juce::Slider::backgroundColourId, Colors::panelDark);
        setColour(juce::Slider::trackColourId, Colors::timeBlue.withAlpha(0.4f));
        setColour(juce::Slider::thumbColourId, Colors::timeBlue);
        setColour(juce::Slider::rotarySliderFillColourId, Colors::timeBlue);
        setColour(juce::Slider::rotarySliderOutlineColourId, Colors::panelDark);

        setColour(juce::Label::textColourId, Colors::textPrimary);

        setColour(juce::ComboBox::backgroundColourId, Colors::panelDark);
        setColour(juce::ComboBox::textColourId, Colors::textPrimary);
        setColour(juce::ComboBox::outlineColourId, Colors::timeBlue.withAlpha(0.3f));
        setColour(juce::ComboBox::arrowColourId, Colors::timeBlue);

        setColour(juce::PopupMenu::backgroundColourId, Colors::panelDark);
        setColour(juce::PopupMenu::textColourId, Colors::textPrimary);
        setColour(juce::PopupMenu::highlightedBackgroundColourId, Colors::timeBlue.withAlpha(0.3f));
        setColour(juce::PopupMenu::highlightedTextColourId, Colors::textPrimary);

        setColour(juce::ToggleButton::textColourId, Colors::textPrimary);
        setColour(juce::ToggleButton::tickColourId, Colors::timeBlue);
        setColour(juce::ToggleButton::tickDisabledColourId, Colors::textSecondary);

        setColour(juce::TextButton::buttonColourId, Colors::panelLight);
        setColour(juce::TextButton::buttonOnColourId, Colors::timeBlue);
        setColour(juce::TextButton::textColourOffId, Colors::textPrimary);
        setColour(juce::TextButton::textColourOnId, Colors::background);
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider& slider) override
    {
        auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto centreX = bounds.getCentreX();
        auto centreY = bounds.getCentreY();
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // Background arc
        juce::Path backgroundArc;
        backgroundArc.addCentredArc(centreX, centreY, radius - 2.0f, radius - 2.0f,
                                    0.0f, rotaryStartAngle, rotaryEndAngle, true);
        g.setColour(Colors::panelDark);
        g.strokePath(backgroundArc, juce::PathStrokeType(4.0f, juce::PathStrokeType::curved,
                                                          juce::PathStrokeType::rounded));

        // Value arc
        juce::Path valueArc;
        valueArc.addCentredArc(centreX, centreY, radius - 2.0f, radius - 2.0f,
                               0.0f, rotaryStartAngle, angle, true);
        g.setColour(Colors::timeBlue);
        g.strokePath(valueArc, juce::PathStrokeType(4.0f, juce::PathStrokeType::curved,
                                                     juce::PathStrokeType::rounded));

        // Thumb/pointer
        juce::Path pointer;
        auto pointerLength = radius * 0.6f;
        auto pointerThickness = 3.0f;
        pointer.addRoundedRectangle(-pointerThickness * 0.5f, -radius + 6.0f,
                                    pointerThickness, pointerLength, 1.0f);
        pointer.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

        g.setColour(Colors::textPrimary);
        g.fillPath(pointer);

        // Center dot
        g.setColour(Colors::panelLight);
        g.fillEllipse(centreX - 6.0f, centreY - 6.0f, 12.0f, 12.0f);
    }

    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                              const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);
        auto cornerSize = 4.0f;

        juce::Colour baseColour = button.getToggleState() ? Colors::timeBlue : Colors::panelLight;

        if (shouldDrawButtonAsDown)
            baseColour = baseColour.darker(0.2f);
        else if (shouldDrawButtonAsHighlighted)
            baseColour = baseColour.brighter(0.1f);

        g.setColour(baseColour);
        g.fillRoundedRectangle(bounds, cornerSize);

        g.setColour(Colors::timeBlue.withAlpha(button.getToggleState() ? 0.8f : 0.3f));
        g.drawRoundedRectangle(bounds, cornerSize, 1.0f);
    }

    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                      int buttonX, int buttonY, int buttonW, int buttonH,
                      juce::ComboBox& box) override
    {
        auto bounds = juce::Rectangle<float>(0, 0, static_cast<float>(width), static_cast<float>(height));
        auto cornerSize = 4.0f;

        g.setColour(Colors::panelDark);
        g.fillRoundedRectangle(bounds, cornerSize);

        g.setColour(Colors::timeBlue.withAlpha(0.3f));
        g.drawRoundedRectangle(bounds.reduced(0.5f), cornerSize, 1.0f);

        // Arrow
        auto arrowZone = juce::Rectangle<float>(static_cast<float>(buttonX), static_cast<float>(buttonY),
                                                 static_cast<float>(buttonW), static_cast<float>(buttonH));
        juce::Path arrow;
        arrow.addTriangle(arrowZone.getCentreX() - 4.0f, arrowZone.getCentreY() - 2.0f,
                          arrowZone.getCentreX() + 4.0f, arrowZone.getCentreY() - 2.0f,
                          arrowZone.getCentreX(), arrowZone.getCentreY() + 3.0f);
        g.setColour(Colors::timeBlue);
        g.fillPath(arrow);
    }

    juce::Font getLabelFont(juce::Label& label) override
    {
        return juce::Font(12.0f);
    }

    void drawLabel(juce::Graphics& g, juce::Label& label) override
    {
        g.setColour(label.findColour(juce::Label::textColourId));
        g.setFont(getLabelFont(label));

        auto textArea = label.getBorderSize().subtractedFrom(label.getLocalBounds());
        g.drawFittedText(label.getText(), textArea, label.getJustificationType(),
                         juce::jmax(1, static_cast<int>(static_cast<float>(textArea.getHeight()) / 12.0f)),
                         label.getMinimumHorizontalScale());
    }
};

} // namespace Chronos
