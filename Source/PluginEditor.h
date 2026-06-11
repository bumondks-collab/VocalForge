#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class VFLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider (juce::Graphics& g,
                           int x, int y, int width, int height,
                           float sliderPos,
                           float startAngle, float endAngle,
                           juce::Slider& slider) override
    {
        auto radius  = (float)juce::jmin (width / 2, height / 2) - 5.0f;
        auto centreX = (float)x + (float)width  * 0.5f;
        auto centreY = (float)y + (float)height * 0.5f;
        auto angle   = startAngle + sliderPos * (endAngle - startAngle);

        juce::Colour accent = slider.findColour (juce::Slider::rotarySliderFillColourId);

        { juce::Path arc; arc.addCentredArc (centreX, centreY, radius, radius, 0.f, startAngle, endAngle, true);
          g.setColour (juce::Colour (0xff181830)); g.strokePath (arc, juce::PathStrokeType (3.5f)); }
        { juce::Path arc; arc.addCentredArc (centreX, centreY, radius, radius, 0.f, startAngle, angle, true);
          g.setColour (accent); g.strokePath (arc, juce::PathStrokeType (3.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded)); }

        float innerR = radius * 0.62f;
        g.setColour (juce::Colour (0xff14142e));
        g.fillEllipse (centreX - innerR, centreY - innerR, innerR * 2.f, innerR * 2.f);
        g.setColour (accent.withAlpha (0.28f));
        g.drawEllipse (centreX - innerR, centreY - innerR, innerR * 2.f, innerR * 2.f, 1.2f);

        float dotDist = innerR * 0.60f;
        float pdx = centreX + dotDist * std::sin (angle);
        float pdy = centreY - dotDist * std::cos (angle);
        g.setColour (accent);
        g.fillEllipse (pdx - 2.8f, pdy - 2.8f, 5.6f, 5.6f);
    }

    void drawLinearSlider (juce::Graphics&, int, int, int, int, float, float, float,
                           juce::Slider::SliderStyle, juce::Slider&) override {}
};

class VocalForgeAudioProcessorEditor : public juce::AudioProcessorEditor,
                                        private juce::Timer
{
public:
    VocalForgeAudioProcessorEditor (VocalForgeAudioProcessor&);
    ~VocalForgeAudioProcessorEditor() override;

    void paint   (juce::Graphics&) override;
    void resized ()                override;
    void mouseDown (const juce::MouseEvent&) override;
    void mouseMove (const juce::MouseEvent&) override;
    void mouseExit (const juce::MouseEvent&) override;

private:
    void timerCallback() override;

    void drawBackground    (juce::Graphics&);
    void drawHeader        (juce::Graphics&);
    void drawVU            (juce::Graphics&, juce::Rectangle<float>, float db, bool isInput);
    void drawPresetSection (juce::Graphics&);
    void drawPresetCard    (juce::Graphics&, juce::Rectangle<float>, int idx, bool sel, bool hov);
    void drawPresetArtwork (juce::Graphics&, juce::Rectangle<float>, int idx);
    void drawNavArrow      (juce::Graphics&, juce::Rectangle<float>, bool pointLeft);
    void drawChainSection  (juce::Graphics&);
    void drawModule        (juce::Graphics&, juce::Rectangle<float>, int modIdx, const VocalPreset&);
    void drawModuleIcon    (juce::Graphics&, juce::Rectangle<float>, int modIdx, juce::Colour col, bool enabled);
    void drawArrowConn     (juce::Graphics&, float midX, float midY, juce::Colour col);
    void drawBottomSection (juce::Graphics&);

    juce::Rectangle<float> cardBounds         (int slot)    const;
    juce::Rectangle<float> moduleBounds       (int modIdx)  const;
    juce::Rectangle<float> modToggleBounds    (int modIdx)  const;
    juce::Rectangle<float> leftNavBounds      ()            const;
    juce::Rectangle<float> rightNavBounds     ()            const;
    juce::Rectangle<int>   knobBounds         (int modIdx)  const;
    juce::Rectangle<int>   inputKnobBounds    ()            const;
    juce::Rectangle<int>   outputKnobBounds   ()            const;
    juce::Rectangle<float> oversampBounds     ()            const;
    juce::Rectangle<float> delayDivBounds     (int divIdx)  const;
    juce::Rectangle<float> delayPingPongBounds()            const;
    juce::Rectangle<float> pitchKeyBounds     (int noteIdx)   const;
    juce::Rectangle<float> pitchScaleBounds   (int scaleIdx)  const;
    juce::Rectangle<float> pitchBypassBtnBounds () const;
    juce::Rectangle<float> pitchMenuBtnBounds   () const;
    juce::Rectangle<float> gearBtnBounds  () const;
    juce::Rectangle<float> powerBtnBounds () const;

    void selectPreset  (int idx);
    void updateKnobs   (const VocalPreset& p);
    void showPitchMenu ();

    VocalForgeAudioProcessor& audioProcessor;
    VFLookAndFeel              vfLAF;

    juce::Slider knob[7];
    juce::Slider inputKnob, outputKnob;

    int   selectedPreset = 0;
    bool  modEnabled[7]  { true, true, true, true, true, true, true };
    bool  oversamplingOn = true;
    float inputDb        = -100.f;
    float outputDb       = -100.f;
    float currentGR      =    0.f;

    int  delayDivision  = 1;
    bool delayPingPong  = false;

    int  pitchKey   = 0;
    int  pitchScale = 0;

    bool pitchBypassState = false;
    int  pitchSpeedTier   = 2;
    bool pitchHumanizeOn  = false;

    bool masterBypassState = false;

    int  hovCard  = -1, hovMod = -1;
    bool hovLeft  = false, hovRight = false, hovOver = false;

    static constexpr int kW = 1380;
    static constexpr int kH = 760;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VocalForgeAudioProcessorEditor)
};
