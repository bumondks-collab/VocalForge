#include "PluginEditor.h"

// ============================================================
//  Module metadata
// ============================================================
namespace {
    struct ModInfo { const char* name; const char* unit; float minVal; float maxVal; juce::uint32 color; };
    static const ModInfo kMod[7] =
    {
        { "EQ",         "%",   0.f, 100.f, 0xff7B5BFF },   // purple
        { "COMPRESSOR", "%",   0.f, 100.f, 0xff00D4AA },   // teal
        { "DE-ESSER",   "%",   0.f, 100.f, 0xff00CCFF },   // cyan
        { "SATURATION", "%",   0.f, 100.f, 0xffFF9900 },   // amber
        { "DELAY",      "%",   0.f, 100.f, 0xffFFCC00 },   // yellow
        { "CHORUS",     "%",   0.f, 100.f, 0xff4499FF },   // blue
        { "REVERB",     "%",   0.f, 100.f, 0xff9944FF },   // violet
    };
}

static constexpr float kVuX1 = 16.f,  kVuX2 = 1336.f, kVuW = 24.f;
static constexpr float kVuY  = 110.f, kVuH  = 490.f;

static constexpr float kPPX = 52.f,  kPPY = 96.f,  kPPW = 1276.f, kPPH = 248.f;
static constexpr float kCPX = 52.f,  kCPY = 353.f, kCPW = 1276.f, kCPH = 264.f;
static constexpr float kBPX = 52.f,  kBPY = 626.f, kBPW = 1276.f, kBPH = 122.f;

static constexpr float kCardX0 = 104.f, kCardY0 = 112.f;
static constexpr float kCardW  = 182.f, kCardH  = 188.f, kCardGap = 16.f;

static constexpr float kModX0 = 56.f, kModY0 = 357.f;
static constexpr float kModW  = 164.f, kModH = 256.f, kArrowW = 20.f;

static constexpr int kKnobSz = 72;
static constexpr int kKnobOffY = 130;

juce::Rectangle<float> VocalForgeAudioProcessorEditor::cardBounds (int slot) const
{
    float x = kCardX0 + slot * (kCardW + kCardGap);
    return { x, kCardY0, kCardW, kCardH };
}

juce::Rectangle<float> VocalForgeAudioProcessorEditor::moduleBounds (int idx) const
{
    float x = kModX0 + idx * (kModW + kArrowW);
    return { x, kModY0, kModW, kModH };
}

juce::Rectangle<float> VocalForgeAudioProcessorEditor::modToggleBounds (int idx) const
{
    auto mb = moduleBounds (idx);
    return { mb.getRight() - 26.f, mb.getY() + 8.f, 18.f, 18.f };
}

juce::Rectangle<float> VocalForgeAudioProcessorEditor::leftNavBounds() const
{
    return { kPPX + 2.f, kPPY + 40.f, 42.f, kPPH - 60.f };
}

juce::Rectangle<float> VocalForgeAudioProcessorEditor::rightNavBounds() const
{
    return { kPPX + kPPW - 44.f, kPPY + 40.f, 42.f, kPPH - 60.f };
}

juce::Rectangle<int> VocalForgeAudioProcessorEditor::knobBounds (int idx) const
{
    auto mb = moduleBounds (idx);
    int kx = (int)(mb.getX() + (kModW - kKnobSz) * 0.5f);
    int ky = (int)(mb.getY()) + kKnobOffY;
    return { kx, ky, kKnobSz, kKnobSz };
}

juce::Rectangle<int> VocalForgeAudioProcessorEditor::inputKnobBounds() const
{
    return { 20, (int)(kBPY + 22.f), 50, 56 };
}

juce::Rectangle<int> VocalForgeAudioProcessorEditor::outputKnobBounds() const
{
    return { kW - 50, (int)(kBPY + 22.f), 42, 56 };
}

juce::Rectangle<float> VocalForgeAudioProcessorEditor::oversampBounds() const
{
    return { kBPX + kBPW - 228.f, kBPY + 14.f, 200.f, 94.f };
}

static constexpr float kPitchKeyX0 = 900.f;
static constexpr float kPitchBtnW  = 26.f;
static constexpr float kPitchBtnH  = 36.f;
static constexpr float kPitchGap   = 3.f;

juce::Rectangle<float> VocalForgeAudioProcessorEditor::pitchKeyBounds (int noteIdx) const
{
    return { kPitchKeyX0 + noteIdx * (kPitchBtnW + kPitchGap),
             16.f, kPitchBtnW, kPitchBtnH };
}

juce::Rectangle<float> VocalForgeAudioProcessorEditor::pitchScaleBounds (int scaleIdx) const
{
    static constexpr float kTotalW = 12.f * kPitchBtnW + 11.f * kPitchGap;
    static constexpr float kBtnW   = (kTotalW - 2.f * kPitchGap) / 3.f;
    return { kPitchKeyX0 + scaleIdx * (kBtnW + kPitchGap),
             54.f, kBtnW, 22.f };
}

juce::Rectangle<float> VocalForgeAudioProcessorEditor::delayDivBounds (int divIdx) const
{
    auto mb = moduleBounds (4);
    float iconX = mb.getX() + 8.f;
    float iconY = mb.getY() + 34.f + 18.f;
    float bw    = 46.f, bh = 20.f;
    return { iconX + divIdx * (bw + 5.f), iconY, bw, bh };
}

juce::Rectangle<float> VocalForgeAudioProcessorEditor::delayPingPongBounds() const
{
    auto mb = moduleBounds (4);
    float iconX = mb.getX() + 8.f;
    float iconY = mb.getY() + 34.f + 44.f;
    return { iconX, iconY, 148.f, 22.f };
}

juce::Rectangle<float> VocalForgeAudioProcessorEditor::pitchBypassBtnBounds() const
{
    return { 1248.f, 16.f, 28.f, 26.f };
}

juce::Rectangle<float> VocalForgeAudioProcessorEditor::pitchMenuBtnBounds() const
{
    return { 1248.f, 46.f, 28.f, 26.f };
}

juce::Rectangle<float> VocalForgeAudioProcessorEditor::gearBtnBounds() const
{
    return { (float)kW - 100.f, 24.f, 34.f, 34.f };
}

juce::Rectangle<float> VocalForgeAudioProcessorEditor::powerBtnBounds() const
{
    return { (float)kW - 58.f, 24.f, 34.f, 34.f };
}

VocalForgeAudioProcessorEditor::VocalForgeAudioProcessorEditor (VocalForgeAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (kW, kH);
    setResizable (false, false);

    for (int i = 0; i < 7; ++i)
    {
        auto& k = knob[i];
        k.setSliderStyle (juce::Slider::Rotary);
        k.setTextBoxStyle (juce::Slider::NoTextBox, true, 0, 0);
        k.setRange (kMod[i].minVal, kMod[i].maxVal, 0.01);
        k.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (kMod[i].color));
        k.setLookAndFeel (&vfLAF);
        addAndMakeVisible (k);

        const int idx = i;
        k.onValueChange = [this, idx]()
        {
            const float v = (float)knob[idx].getValue();
            switch (idx)
            {
                case 0: audioProcessor.setEqLowBellGain ((v - 50.f) * 0.24f); break;
                case 1: audioProcessor.setCompThreshold (v * -0.40f);          break;
                case 2: audioProcessor.setDeEsserThresh (v * -0.40f);          break;
                case 3: audioProcessor.setSatDrive      (v * 0.01f);           break;
                case 4: audioProcessor.setDelayDepth    (v * 0.01f);           break;
                case 5: audioProcessor.setChorusMix     (v * 0.01f);           break;
                case 6: audioProcessor.setReverbWet     (v * 0.01f);           break;
                default: break;
            }
            repaint();
        };
    }

    for (auto* k : { &inputKnob, &outputKnob })
    {
        k->setSliderStyle (juce::Slider::Rotary);
        k->setTextBoxStyle (juce::Slider::NoTextBox, true, 0, 0);
        k->setRange (-24.0, 24.0, 0.1);
        k->setValue (0.0, juce::dontSendNotification);
        k->setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xff7B5BFF));
        k->setLookAndFeel (&vfLAF);
        addAndMakeVisible (*k);
    }

    selectPreset (audioProcessor.getCurrentPresetIndex());
    startTimerHz (30);
}

VocalForgeAudioProcessorEditor::~VocalForgeAudioProcessorEditor()
{
    for (int i = 0; i < 7; ++i)
        knob[i].setLookAndFeel (nullptr);
    inputKnob.setLookAndFeel (nullptr);
    outputKnob.setLookAndFeel (nullptr);
    stopTimer();
}

void VocalForgeAudioProcessorEditor::timerCallback()
{
    float newOut = audioProcessor.getOutputLevelDb();
    float newIn  = audioProcessor.getInputLevelDb();
    float newGR  = audioProcessor.getGainReductionDb();
    float newBpm = audioProcessor.getCurrentBpm();
    static float lastBpm = 0.f;
    int newKey   = audioProcessor.getPitchKey();
    int newScale = audioProcessor.getPitchScale();

    if (std::abs (newOut - outputDb)   > 0.3f
     || std::abs (newIn  - inputDb)    > 0.3f
     || std::abs (newGR  - currentGR)  > 0.15f
     || std::abs (newBpm - lastBpm)    > 0.5f
     || newKey   != pitchKey
     || newScale != pitchScale)
    {
        outputDb   = newOut;
        inputDb    = newIn;
        currentGR  = newGR;
        lastBpm    = newBpm;
        pitchKey   = newKey;
        pitchScale = newScale;
        repaint();
    }
}

void VocalForgeAudioProcessorEditor::selectPreset (int idx)
{
    if (idx < 0 || idx >= audioProcessor.getPresets().size()) return;
    selectedPreset = idx;
    audioProcessor.loadPreset (idx);
    updateKnobs (audioProcessor.getPresets()[idx]);

    float speedMs   = audioProcessor.getPresets()[idx].pitchSpeedMs;
    pitchSpeedTier  = speedMs <= 2.f   ? 0 :
                      speedMs <= 20.f  ? 1 :
                      speedMs <= 100.f ? 2 :
                      speedMs <= 200.f ? 3 : 4;

    pitchBypassState = false;
    pitchHumanizeOn  = false;
    audioProcessor.setPitchBypass  (false);
    audioProcessor.setPitchHumanize (0.f);
    repaint();
}

void VocalForgeAudioProcessorEditor::updateKnobs (const VocalPreset& p)
{
    knob[0].setValue (juce::jlimit (0.0, 100.0, (double)(p.eqLowBellGain / 0.24f + 50.f)),
                      juce::dontSendNotification);
    knob[1].setValue (juce::jlimit (0.0, 100.0, (double)(p.compThresholdDb / -0.40f)),
                      juce::dontSendNotification);
    knob[2].setValue (juce::jlimit (0.0, 100.0, (double)(p.deEssHighThreshDb / -0.40f)),
                      juce::dontSendNotification);
    knob[3].setValue (p.satDrive  * 100.f, juce::dontSendNotification);
    knob[4].setValue (100.0,               juce::dontSendNotification);
    knob[5].setValue (p.chorusMix * 100.f, juce::dontSendNotification);
    knob[6].setValue (p.reverbWet * 100.f, juce::dontSendNotification);
    delayDivision = p.delayDivision;
    delayPingPong = p.delayPingPong;
}

void VocalForgeAudioProcessorEditor::mouseDown (const juce::MouseEvent& e)
{
    auto pos = e.position;

    for (int i = 0; i < audioProcessor.getPresets().size(); ++i)
        if (cardBounds (i).contains (pos))
            { selectPreset (i); return; }

    if (leftNavBounds().contains (pos))
    {
        int newIdx = (selectedPreset - 1 + audioProcessor.getPresets().size())
                     % audioProcessor.getPresets().size();
        selectPreset (newIdx); return;
    }
    if (rightNavBounds().contains (pos))
    {
        int newIdx = (selectedPreset + 1) % audioProcessor.getPresets().size();
        selectPreset (newIdx); return;
    }

    for (int i = 0; i < 7; ++i)
    {
        if (modToggleBounds (i).contains (pos))
        {
            modEnabled[i] = !modEnabled[i];
            audioProcessor.setModuleEnabled (i, modEnabled[i]);
            repaint(); return;
        }
    }

    for (int d = 0; d < 3; ++d)
    {
        if (delayDivBounds (d).contains (pos))
        {
            delayDivision = d;
            audioProcessor.setDelayDivision (d);
            repaint(); return;
        }
    }

    if (delayPingPongBounds().contains (pos))
    {
        delayPingPong = !delayPingPong;
        audioProcessor.setDelayPingPong (delayPingPong);
        repaint(); return;
    }

    if (oversampBounds().contains (pos))
    {
        oversamplingOn = !oversamplingOn;
        audioProcessor.setOversamplingEnabled (oversamplingOn);
        repaint(); return;
    }

    if (pitchBypassBtnBounds().contains (pos))
    {
        pitchBypassState = !pitchBypassState;
        audioProcessor.setPitchBypass (pitchBypassState);
        repaint(); return;
    }

    if (pitchMenuBtnBounds().contains (pos)) { showPitchMenu(); return; }

    if (gearBtnBounds().contains (pos))
    {
        oversamplingOn = !oversamplingOn;
        audioProcessor.setOversamplingEnabled (oversamplingOn);
        repaint(); return;
    }

    if (powerBtnBounds().contains (pos))
    {
        masterBypassState = !masterBypassState;
        audioProcessor.setMasterBypass (masterBypassState);
        repaint(); return;
    }

    for (int n = 0; n < 12; ++n)
        if (pitchKeyBounds (n).contains (pos))
            { pitchKey = n; audioProcessor.setPitchKey (n); repaint(); return; }

    for (int s = 0; s < 3; ++s)
        if (pitchScaleBounds (s).contains (pos))
            { pitchScale = s; audioProcessor.setPitchScale (s); repaint(); return; }
}

void VocalForgeAudioProcessorEditor::mouseMove (const juce::MouseEvent& e)
{
    auto pos = e.position;
    int  newHovCard  = -1;
    int  newHovMod   = -1;
    bool newHovLeft  = leftNavBounds().contains (pos);
    bool newHovRight = rightNavBounds().contains (pos);
    bool newHovOver  = oversampBounds().contains (pos);

    for (int i = 0; i < audioProcessor.getPresets().size(); ++i)
        if (cardBounds (i).contains (pos)) { newHovCard = i; break; }

    for (int i = 0; i < 7; ++i)
        if (moduleBounds (i).contains (pos)) { newHovMod = i; break; }

    if (newHovCard != hovCard || newHovMod != hovMod
     || newHovLeft != hovLeft || newHovRight != hovRight || newHovOver != hovOver)
    {
        hovCard  = newHovCard; hovMod   = newHovMod;
        hovLeft  = newHovLeft; hovRight = newHovRight; hovOver  = newHovOver;
        repaint();
    }
}

void VocalForgeAudioProcessorEditor::mouseExit (const juce::MouseEvent&)
{
    hovCard = -1; hovMod = -1;
    hovLeft = hovRight = hovOver = false;
    repaint();
}
void VocalForgeAudioProcessorEditor::paint (juce::Graphics& g)
{
    drawBackground (g);
    drawHeader (g);
    drawVU (g, { kVuX1, kVuY, kVuW, kVuH }, inputDb,  true);
    drawVU (g, { kVuX2, kVuY, kVuW, kVuH }, outputDb, false);
    drawPresetSection (g);
    drawChainSection  (g);
    drawBottomSection (g);
}

void VocalForgeAudioProcessorEditor::resized()
{
    for (int i = 0; i < 7; ++i)
        knob[i].setBounds (knobBounds (i));
    inputKnob.setBounds  (inputKnobBounds());
    outputKnob.setBounds (outputKnobBounds());
}

void VocalForgeAudioProcessorEditor::drawBackground (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff090915));

    const auto& presets = audioProcessor.getPresets();
    juce::Colour ac = presets.isEmpty() ? juce::Colour (0xff7B5BFF) : presets[selectedPreset].colour;

    juce::ColourGradient topGlow (ac.withAlpha (0.10f),
                                  (float)kW * 0.5f, 0.f,
                                  juce::Colours::transparentBlack,
                                  (float)kW * 0.5f, 300.f, true);
    g.setGradientFill (topGlow);
    g.fillRect (0, 0, kW, 300);

    auto panelCol = juce::Colour (0xff0f0f28);
    g.setColour (panelCol.withAlpha (0.92f));
    g.fillRoundedRectangle (kPPX, kPPY, kPPW, kPPH, 14.f);
    g.fillRoundedRectangle (kCPX, kCPY, kCPW, kCPH, 14.f);
    g.fillRoundedRectangle (kBPX + 28.f, kBPY, kBPW - 56.f, kBPH, 14.f);

    g.setColour (juce::Colour (0xff2a2a50));
    g.drawRoundedRectangle (kPPX, kPPY, kPPW, kPPH, 14.f, 0.8f);
    g.drawRoundedRectangle (kCPX, kCPY, kCPW, kCPH, 14.f, 0.8f);
    g.drawRoundedRectangle (kBPX + 28.f, kBPY, kBPW - 56.f, kBPH, 14.f, 0.8f);
}

void VocalForgeAudioProcessorEditor::drawHeader (juce::Graphics& g)
{
    g.setColour (juce::Colour (0xff0c0c22));
    g.fillRect (0, 0, kW, 88);

    const auto& presets = audioProcessor.getPresets();
    juce::Colour ac = presets.isEmpty() ? juce::Colour (0xff7B5BFF) : presets[selectedPreset].colour;
    g.setColour (ac.withAlpha (0.5f));
    g.fillRect (0, 86, kW, 2);

    {
        float cx = 42.f, cy = 44.f, r = 22.f;
        juce::ColourGradient grad (ac, cx - r, cy - r, ac.darker (0.4f), cx + r, cy + r, false);
        g.setGradientFill (grad);
        g.fillEllipse (cx - r, cy - r, r * 2.f, r * 2.f);
        g.setColour (juce::Colours::white);
        g.setFont (juce::FontOptions (14.f).withStyle ("Bold"));
        g.drawText ("VF", (int)(cx - 14), (int)(cy - 9), 28, 18, juce::Justification::centred);
    }

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (26.f).withStyle ("Bold"));
    g.drawText ("VOCAL FACTORY", 78, 12, 340, 34, juce::Justification::centredLeft);

    g.setColour (juce::Colour (0xff6666aa));
    g.setFont (juce::FontOptions (9.5f).withStyle ("Bold"));
    g.drawText ("BY BUMON.MIX", 82, 48, 200, 14, juce::Justification::centredLeft);

    g.setColour (juce::Colour (0xff2a2a50));
    g.drawVerticalLine (435, 12.f, 76.f);

    if (!presets.isEmpty())
    {
        const auto& p = presets[selectedPreset];

        g.setColour (juce::Colour (0xff7777aa));
        g.setFont (juce::FontOptions (8.5f).withStyle ("Bold"));
        g.drawText ("PRESET", 460, 10, 540, 14, juce::Justification::centred);

        g.setColour (juce::Colours::white);
        g.setFont (juce::FontOptions (24.f).withStyle ("Bold"));
        g.drawText (p.name, 460, 24, 540, 34, juce::Justification::centred);

        {
            float wfX = 560.f, wfY = 61.f, wfW = 240.f, wfH = 16.f;
            juce::Random rng (selectedPreset * 31337 + 1);
            for (float wx = wfX; wx < wfX + wfW; wx += 3.f)
            {
                float h = 2.f + rng.nextFloat() * (wfH - 2.f);
                float hy = wfY + (wfH - h) * 0.5f;
                g.setColour (ac.withAlpha (0.25f + rng.nextFloat() * 0.35f));
                g.fillRect (wx, hy, 1.5f, h);
            }
        }
    }

    {
        static const char* kNoteNames[] = { "C","C#","D","D#","E","F","F#","G","G#","A","A#","B" };
        static const bool  kIsSharp[]  = { false,true,false,true,false,false,true,false,true,false,true,false };

        g.setColour (juce::Colour (0xff6666aa));
        g.setFont   (juce::FontOptions (8.f).withStyle ("Bold"));
        g.drawText  ("KEY",
                     juce::Rectangle<float> (pitchKeyBounds(0).getX() - 28.f, 2.f, 26.f, 12.f).toNearestInt(),
                     juce::Justification::centredRight);

        for (int n = 0; n < 12; ++n)
        {
            auto  b     = pitchKeyBounds (n);
            bool  sel   = (n == pitchKey);
            bool  sharp = kIsSharp[n];
            juce::Colour baseCol = sharp ? juce::Colour (0xff0e0e24) : juce::Colour (0xff181830);

            if (sel)
            {
                g.setColour (ac.withAlpha (0.30f));
                g.fillRoundedRectangle (b, 3.f);
                g.setColour (ac);
                g.drawRoundedRectangle (b, 3.f, 1.5f);
            }
            else
            {
                g.setColour (baseCol);
                g.fillRoundedRectangle (b, 3.f);
                g.setColour (juce::Colour (0xff2a2a50));
                g.drawRoundedRectangle (b, 3.f, 0.8f);
            }

            g.setColour (sel ? ac.brighter (0.2f) : juce::Colour (0xff6666aa));
            g.setFont   (juce::FontOptions (7.f).withStyle (sel ? "Bold" : "Regular"));
            g.drawText  (kNoteNames[n], b.toNearestInt(), juce::Justification::centred);
        }

        static const char* kScaleLabels[] = { "CHR", "MAJ", "MIN" };
        for (int s = 0; s < 3; ++s)
        {
            auto  b   = pitchScaleBounds (s);
            bool  sel = (s == pitchScale);

            if (sel)
            {
                g.setColour (ac.withAlpha (0.25f));
                g.fillRoundedRectangle (b, 4.f);
                g.setColour (ac);
                g.drawRoundedRectangle (b, 4.f, 1.5f);
            }
            else
            {
                g.setColour (juce::Colour (0xff181830));
                g.fillRoundedRectangle (b, 4.f);
                g.setColour (juce::Colour (0xff2a2a50));
                g.drawRoundedRectangle (b, 4.f, 0.8f);
            }

            g.setColour (sel ? ac.brighter (0.2f) : juce::Colour (0xff6666aa));
            g.setFont   (juce::FontOptions (8.f).withStyle (sel ? "Bold" : "Regular"));
            g.drawText  (kScaleLabels[s], b.toNearestInt(), juce::Justification::centred);
        }
    }

    {
        auto  bb   = pitchBypassBtnBounds();
        bool  bypOn = pitchBypassState;
        g.setColour (bypOn ? juce::Colour (0xff882222) : juce::Colour (0xff181830));
        g.fillRoundedRectangle (bb, 4.f);
        g.setColour (bypOn ? juce::Colour (0xffff5555) : juce::Colour (0xff2a2a50));
        g.drawRoundedRectangle (bb, 4.f, 1.2f);
        g.setColour (bypOn ? juce::Colours::white : juce::Colour (0xff555588));
        g.setFont   (juce::FontOptions (7.5f).withStyle ("Bold"));
        g.drawText  ("BYP", bb.toNearestInt(), juce::Justification::centred);
    }

    {
        auto mb   = pitchMenuBtnBounds();
        bool humOn = pitchHumanizeOn;
        static const char* kTierLabels[] = { "BOT", "FAST", "MED", "SLOW", "NAT" };
        juce::String label = kTierLabels[juce::jlimit (0, 4, pitchSpeedTier)];
        g.setColour (humOn ? ac.withAlpha (0.20f) : juce::Colour (0xff181830));
        g.fillRoundedRectangle (mb, 4.f);
        g.setColour (humOn ? ac.withAlpha (0.70f) : juce::Colour (0xff2a2a50));
        g.drawRoundedRectangle (mb, 4.f, 1.2f);
        g.setColour (humOn ? ac.brighter (0.2f) : juce::Colour (0xff8888bb));
        g.setFont   (juce::FontOptions (7.f).withStyle ("Bold"));
        g.drawText  (label, mb.toNearestInt(), juce::Justification::centred);
    }

    auto drawIconBtn = [&](float bx, float by, float bs,
                           bool active, juce::Colour accentCol) -> juce::Colour
    {
        juce::Colour bg     = active ? accentCol.darker  (1.4f)  : juce::Colour (0xff1e1e40);
        juce::Colour border = active ? accentCol                  : juce::Colour (0xff5555aa);
        juce::Colour icon   = active ? accentCol.brighter (0.4f) : juce::Colour (0xff6666aa);
        if (active)
        {
            g.setColour (accentCol.withAlpha (0.16f));
            g.fillRoundedRectangle (bx - 3.f, by - 3.f, bs + 6.f, bs + 6.f, 11.f);
        }
        g.setColour (bg);
        g.fillRoundedRectangle (bx, by, bs, bs, 8.f);
        g.setColour (border);
        g.drawRoundedRectangle (bx, by, bs, bs, 8.f, 0.8f);
        return icon;
    };

    {
        auto  b   = gearBtnBounds();
        float bx  = b.getX(), by = b.getY(), bs = b.getWidth();
        auto iconCol = drawIconBtn (bx, by, bs, oversamplingOn, juce::Colour (0xff44aaff));
        float cx = bx + bs * 0.5f, cy = by + bs * 0.5f;
        g.setColour (iconCol);
        g.fillEllipse (cx - 2.f, cy - 2.f, 4.f, 4.f);
        for (int a = 0; a < 4; ++a)
        {
            float ang = a * juce::MathConstants<float>::pi * 0.5f;
            g.fillEllipse (cx + 7.f * std::sin (ang) - 2.f,
                           cy - 7.f * std::cos (ang) - 2.f, 4.f, 4.f);
        }
        if (oversamplingOn)
        {
            g.setColour (juce::Colour (0xff44aaff).brighter (0.3f));
            g.setFont (juce::FontOptions (7.f).withStyle ("Bold"));
            g.drawText ("2x", b.toNearestInt(), juce::Justification::centredBottom);
        }
    }

    {
        auto  b      = powerBtnBounds();
        float bx     = b.getX(), by = b.getY(), bs = b.getWidth();
        bool  active = !masterBypassState;
        auto accentCol  = active ? juce::Colour (0xff44cc88) : juce::Colour (0xffcc4444);
        auto iconCol    = drawIconBtn (bx, by, bs, true, accentCol);
        float cx = bx + bs * 0.5f, cy = by + bs * 0.5f;
        juce::Path pw;
        pw.addCentredArc (cx, cy + 1.f, 7.f, 7.f, 0.f, 0.7f,
                          juce::MathConstants<float>::twoPi - 0.7f, true);
        pw.startNewSubPath (cx, cy - 2.f);
        pw.lineTo (cx, cy - 9.f);
        g.setColour (iconCol);
        g.strokePath (pw, juce::PathStrokeType (2.0f));
        if (masterBypassState)
        {
            g.setColour (juce::Colour (0xffff5555).brighter (0.2f));
            g.setFont (juce::FontOptions (7.f).withStyle ("Bold"));
            g.drawText ("BYP", b.toNearestInt(), juce::Justification::centredBottom);
        }
    }
}

void VocalForgeAudioProcessorEditor::drawVU (juce::Graphics& g,
                                              juce::Rectangle<float> r,
                                              float db, bool isInput)
{
    juce::String label = isInput ? "INPUT" : "OUTPUT";
    g.setColour (juce::Colour (0xff7777aa));
    g.setFont (juce::FontOptions (9.f).withStyle ("Bold"));
    g.drawText (label,
                juce::Rectangle<float> (r.getX() - 10.f, r.getY() - 18.f, r.getWidth() + 20.f, 14.f)
                .toNearestInt(), juce::Justification::centred);

    g.setColour (juce::Colour (0xff0d0d1e));
    g.fillRoundedRectangle (r, 4.f);
    g.setColour (juce::Colour (0xff1a1a38));
    g.drawRoundedRectangle (r, 4.f, 0.8f);

    float normDb = juce::jmap (juce::jlimit (-60.f, 0.f, db), -60.f, 0.f, 0.f, 1.f);
    float fillH  = r.getHeight() * normDb;
    if (fillH > 1.f)
    {
        juce::Rectangle<float> fill (r.getX(), r.getBottom() - fillH, r.getWidth(), fillH);
        juce::Colour col = db < -12.f ? juce::Colour (0xff00e0a0)
                         : db < -3.f  ? juce::Colour (0xffffc107)
                                      : juce::Colour (0xffff4444);
        juce::ColourGradient grad (col.brighter (0.2f), 0.f, fill.getY(),
                                   col.darker  (0.1f), 0.f, fill.getBottom(), false);
        g.setGradientFill (grad);
        g.fillRoundedRectangle (fill, 3.f);
    }

    static const float ticks[] = { 0.f, -6.f, -12.f, -18.f, -24.f, -36.f, -48.f };
    g.setColour (juce::Colour (0xff444466));
    g.setFont (juce::FontOptions (7.f));
    for (float t : ticks)
    {
        float ny   = juce::jmap (t, -60.f, 0.f, 0.f, 1.f);
        float tickY = r.getBottom() - ny * r.getHeight();
        g.drawHorizontalLine ((int)tickY, r.getX() - 2.f, r.getRight() + 2.f);
    }

    g.setColour (juce::Colour (0xff6666aa));
    g.setFont (juce::FontOptions (8.f));
    juce::String dbStr = db > -99.f ? juce::String (db, 1) + "dB" : "-inf";
    g.drawText (dbStr,
                juce::Rectangle<float> (r.getX() - 10.f, r.getBottom() + 2.f, r.getWidth() + 20.f, 12.f)
                .toNearestInt(), juce::Justification::centred);
}
void VocalForgeAudioProcessorEditor::drawPresetSection (juce::Graphics& g)
{
    g.setColour (juce::Colour (0xff6666aa));
    g.setFont (juce::FontOptions (10.f).withStyle ("Bold"));
    g.drawText ("PRESETS",
                juce::Rectangle<float> (kPPX + 8.f, kPPY + 6.f, 100.f, 14.f).toNearestInt(),
                juce::Justification::centredLeft);

    const auto& presets = audioProcessor.getPresets();
    drawNavArrow (g, leftNavBounds(),  true);
    drawNavArrow (g, rightNavBounds(), false);

    for (int i = 0; i < presets.size(); ++i)
        drawPresetCard (g, cardBounds (i), i, i == selectedPreset, i == hovCard);

    int n = presets.size();
    float dotR = 3.5f, dotGap = 12.f;
    float totalW = n * dotGap - (dotGap - dotR * 2.f);
    float dotX   = kPPX + kPPW * 0.5f - totalW * 0.5f;
    float dotY   = kPPY + kPPH - 12.f;
    for (int i = 0; i < n; ++i)
    {
        bool active = (i == selectedPreset);
        g.setColour (active ? presets[selectedPreset].colour.withAlpha (0.9f)
                            : juce::Colour (0xff333355));
        float r2 = active ? dotR : dotR * 0.7f;
        g.fillEllipse (dotX - r2, dotY - r2, r2 * 2.f, r2 * 2.f);
        dotX += dotGap;
    }
}

void VocalForgeAudioProcessorEditor::drawNavArrow (juce::Graphics& g,
                                                    juce::Rectangle<float> r, bool left)
{
    bool hov = left ? hovLeft : hovRight;
    g.setColour (hov ? juce::Colour (0xff3a3a60) : juce::Colour (0xff1e1e38));
    g.fillRoundedRectangle (r, 8.f);
    g.setColour (hov ? juce::Colour (0xffaaaaff) : juce::Colour (0xff666699));
    g.drawRoundedRectangle (r, 8.f, 0.8f);

    float cx = r.getCentreX(), cy = r.getCentreY();
    juce::Path p;
    if (left)
    {
        p.startNewSubPath (cx + 6.f, cy - 8.f);
        p.lineTo          (cx - 5.f, cy);
        p.lineTo          (cx + 6.f, cy + 8.f);
    }
    else
    {
        p.startNewSubPath (cx - 6.f, cy - 8.f);
        p.lineTo          (cx + 5.f, cy);
        p.lineTo          (cx - 6.f, cy + 8.f);
    }
    g.setColour (hov ? juce::Colours::white : juce::Colour (0xff9999cc));
    g.strokePath (p, juce::PathStrokeType (2.2f, juce::PathStrokeType::curved,
                                           juce::PathStrokeType::rounded));
}

void VocalForgeAudioProcessorEditor::drawPresetCard (juce::Graphics& g,
                                                      juce::Rectangle<float> r,
                                                      int idx, bool sel, bool hov)
{
    const auto& p = audioProcessor.getPresets()[idx];
    float cr = 12.f;

    juce::Colour bgCol = sel  ? p.colour.withAlpha (0.18f)
                        : hov ? juce::Colour (0xff1e1e40)
                              : juce::Colour (0xff14142c);
    g.setColour (bgCol);
    g.fillRoundedRectangle (r, cr);

    float borderAlpha = sel ? 0.9f : (hov ? 0.4f : 0.18f);
    g.setColour (p.colour.withAlpha (borderAlpha));
    g.drawRoundedRectangle (r, cr, sel ? 2.0f : 1.2f);

    juce::Rectangle<float> artR = r.withHeight (120.f).reduced (4.f, 4.f);
    artR = artR.withTop (r.getY() + 4.f);
    drawPresetArtwork (g, artR, idx);

    if (sel)
    {
        juce::ColourGradient selGlow (p.colour.withAlpha (0.08f),
                                      r.getCentreX(), r.getY(),
                                      juce::Colours::transparentBlack,
                                      r.getCentreX(), r.getBottom(), false);
        g.setGradientFill (selGlow);
        g.fillRoundedRectangle (r, cr);
    }

    juce::Rectangle<float> pill (r.getX() + 8.f, r.getY() + artR.getHeight() + 10.f, 56.f, 16.f);
    g.setColour (p.colour.withAlpha (0.25f));
    g.fillRoundedRectangle (pill, 8.f);
    g.setColour (p.colour);
    g.setFont (juce::FontOptions (8.f).withStyle ("Bold"));
    g.drawText (p.category.toUpperCase(), pill.toNearestInt(), juce::Justification::centred);

    g.setColour (sel ? juce::Colours::white : juce::Colours::white.withAlpha (0.85f));
    g.setFont (juce::FontOptions (13.f).withStyle ("Bold"));
    juce::Rectangle<float> nameR (r.getX() + 8.f,
                                   r.getY() + artR.getHeight() + 30.f,
                                   r.getWidth() - 16.f, 20.f);
    g.drawText (p.name, nameR.toNearestInt(), juce::Justification::centredLeft, true);

    if (sel)
    {
        float tx = r.getRight() - 22.f, ty = r.getBottom() - 22.f;
        g.setColour (p.colour);
        g.fillEllipse (tx, ty, 14.f, 14.f);
        juce::Path tick;
        tick.startNewSubPath (tx + 3.f, ty + 7.f);
        tick.lineTo          (tx + 6.f, ty + 10.f);
        tick.lineTo          (tx + 11.f, ty + 4.f);
        g.setColour (juce::Colours::white);
        g.strokePath (tick, juce::PathStrokeType (2.0f));
    }
}

void VocalForgeAudioProcessorEditor::drawPresetArtwork (juce::Graphics& g,
                                                          juce::Rectangle<float> r, int idx)
{
    const juce::Colour presetColors[6] =
    {
        juce::Colour (0xff7B5BFF),
        juce::Colour (0xffFF3C78),
        juce::Colour (0xff00D2B4),
        juce::Colour (0xffFFA000),
        juce::Colour (0xff5050C8),
        juce::Colour (0xffFF643C),
    };

    juce::Colour col = (idx < 6) ? presetColors[idx] : juce::Colour (0xff7B5BFF);
    float cx = r.getCentreX(), cy = r.getCentreY();

    g.saveState();
    juce::Path clip;
    clip.addRoundedRectangle (r, 8.f);
    g.reduceClipRegion (clip);

    juce::ColourGradient bg (col.darker (0.6f), cx, r.getBottom(),
                             col.darker (0.1f), cx, r.getY(), false);
    g.setGradientFill (bg);
    g.fillRect (r);

    g.setColour (col.withAlpha (0.08f));
    switch (idx % 6)
    {
        case 0:
            for (int i = 1; i <= 4; ++i)
                g.drawEllipse (cx - i * 18.f, cy - i * 14.f, i * 36.f, i * 28.f, 0.8f);
            break;
        case 1:
            g.setColour (col.withAlpha (0.12f));
            for (int i = -3; i <= 3; ++i)
                g.drawLine (r.getX() + i * 24.f, r.getY(),
                            r.getX() + i * 24.f + r.getHeight(), r.getBottom(), 1.5f);
            break;
        case 2:
            g.setColour (col.withAlpha (0.10f));
            for (int i = 0; i < 5; ++i)
            {
                float wy = r.getY() + i * r.getHeight() * 0.2f;
                juce::Path wave;
                wave.startNewSubPath (r.getX(), wy + r.getHeight() * 0.1f);
                for (float wx = r.getX(); wx < r.getRight(); wx += 4.f)
                {
                    float amp = r.getHeight() * 0.04f;
                    wave.lineTo (wx, wy + r.getHeight() * 0.1f
                                     + amp * std::sin ((wx - r.getX()) * 0.12f));
                }
                g.strokePath (wave, juce::PathStrokeType (0.8f));
            }
            break;
        case 3:
            g.setColour (col.withAlpha (0.10f));
            for (int i = 1; i <= 5; ++i)
                g.drawEllipse (r.getX() + 10.f - i * 14.f, cy - i * 12.f, i * 28.f, i * 24.f, 0.9f);
            break;
        case 4:
            g.setColour (col.withAlpha (0.07f));
            for (float gx = r.getX(); gx < r.getRight(); gx += 14.f)
                g.drawVerticalLine ((int)gx, r.getY(), r.getBottom());
            for (float gy = r.getY(); gy < r.getBottom(); gy += 14.f)
                g.drawHorizontalLine ((int)gy, r.getX(), r.getRight());
            break;
        case 5:
            g.setColour (col.withAlpha (0.11f));
            for (int i = -3; i <= 3; ++i)
            {
                float angle = i * 0.25f;
                g.drawLine (cx, r.getY(),
                            cx + std::sin (angle) * r.getHeight() * 2.f,
                            r.getBottom(), 2.5f);
            }
            break;
        default: break;
    }

    {
        juce::Path mic;
        float mx = cx, my = cy - 4.f;
        float bw = 14.f, bh = 20.f, capR = 10.f;
        mic.addRoundedRectangle (mx - bw * 0.5f, my - bh * 0.5f, bw, bh, 4.f);
        mic.addEllipse (mx - capR, my - bh * 0.5f - capR * 1.4f, capR * 2.f, capR * 2.5f);
        mic.startNewSubPath (mx, my + bh * 0.5f);
        mic.lineTo          (mx, my + bh * 0.5f + 8.f);
        mic.startNewSubPath (mx - 10.f, my + bh * 0.5f + 8.f);
        mic.lineTo          (mx + 10.f, my + bh * 0.5f + 8.f);
        g.setColour (juce::Colours::white.withAlpha (0.25f));
        g.fillPath (mic);
        g.setColour (juce::Colours::white.withAlpha (0.55f));
        g.strokePath (mic, juce::PathStrokeType (1.2f));
    }

    g.restoreState();

    juce::ColourGradient vignette (juce::Colours::transparentBlack, cx, cy,
                                   juce::Colour (0x55000010), r.getRight(), r.getBottom(), true);
    g.setGradientFill (vignette);
    g.fillRect (r);
}

void VocalForgeAudioProcessorEditor::drawChainSection (juce::Graphics& g)
{
    g.setColour (juce::Colour (0xff6666aa));
    g.setFont (juce::FontOptions (10.f).withStyle ("Bold"));
    g.drawText ("PROCESSING CHAIN",
                juce::Rectangle<float> (kCPX + 8.f, kCPY + 6.f, 200.f, 14.f).toNearestInt(),
                juce::Justification::centredLeft);

    if (audioProcessor.getPresets().isEmpty()) return;
    const auto& preset = audioProcessor.getPresets()[selectedPreset];

    for (int i = 0; i < 7; ++i)
    {
        drawModule (g, moduleBounds (i), i, preset);

        if (i < 6)
        {
            auto mb   = moduleBounds (i);
            float ax  = mb.getRight();
            float acy = mb.getCentreY();
            juce::Colour ac = modEnabled[i] && modEnabled[i + 1]
                              ? juce::Colour (kMod[i].color).withAlpha (0.5f)
                              : juce::Colour (0xff333355);
            drawArrowConn (g, ax, acy, ac);
        }
    }
}

void VocalForgeAudioProcessorEditor::drawArrowConn (juce::Graphics& g,
                                                     float x, float cy, juce::Colour col)
{
    float aw = kArrowW;
    float lx1 = x + 2.f, lx2 = x + aw - 4.f;
    g.setColour (col);
    g.drawLine (lx1, cy, lx2, cy, 1.5f);

    juce::Path head;
    head.startNewSubPath (lx2 - 5.f, cy - 4.f);
    head.lineTo          (lx2,        cy);
    head.lineTo          (lx2 - 5.f, cy + 4.f);
    g.strokePath (head, juce::PathStrokeType (1.5f));
}
void VocalForgeAudioProcessorEditor::drawModule (juce::Graphics& g,
                                                  juce::Rectangle<float> r,
                                                  int idx, const VocalPreset& preset)
{
    juce::Colour ac = juce::Colour (kMod[idx].color);
    bool enabled    = modEnabled[idx];
    bool hov        = (idx == hovMod);
    float cr        = 10.f;

    juce::Colour bgBase = enabled ? juce::Colour (0xff131328) : juce::Colour (0xff0e0e20);
    juce::Colour bgHov  = hov     ? bgBase.brighter (0.12f)   : bgBase;
    g.setColour (bgHov);
    g.fillRoundedRectangle (r, cr);

    juce::Colour borderCol = enabled ? ac.withAlpha (hov ? 0.7f : 0.35f)
                                     : juce::Colour (0xff2a2a44);
    g.setColour (borderCol);
    g.drawRoundedRectangle (r, cr, 1.2f);

    if (enabled)
    {
        juce::Rectangle<float> topBar (r.getX(), r.getY(), r.getWidth(), 3.f);
        juce::ColourGradient topGrad (ac.withAlpha (0.7f), r.getX(), r.getY(),
                                      ac.withAlpha (0.0f), r.getRight(), r.getY(), false);
        g.setGradientFill (topGrad);
        g.fillRect (topBar);
    }

    juce::Colour nameCol = enabled ? juce::Colours::white : juce::Colour (0xff555588);
    g.setColour (nameCol);
    g.setFont (juce::FontOptions (9.5f).withStyle ("Bold"));
    float nameX = r.getX() + 10.f;
    g.drawText (kMod[idx].name,
                juce::Rectangle<float> (nameX, r.getY() + 10.f, r.getWidth() - 32.f, 16.f)
                .toNearestInt(), juce::Justification::centredLeft);

    {
        auto tb   = modToggleBounds (idx);
        float tcx = tb.getCentreX(), tcy = tb.getCentreY();
        float br  = tb.getWidth() * 0.5f;

        juce::Colour toggleFill = enabled ? ac.withAlpha (0.18f) : juce::Colour (0xff0e0e1e);
        g.setColour (toggleFill);
        g.fillEllipse (tb);
        g.setColour (enabled ? ac.withAlpha (0.85f) : juce::Colour (0xff333355));
        g.drawEllipse (tb, 1.4f);

        float pr = br * 0.52f;
        juce::Path pw;
        float gapAng = juce::MathConstants<float>::pi * 0.44f;
        pw.addCentredArc (tcx, tcy + 0.5f, pr, pr, 0.f,
                          gapAng, juce::MathConstants<float>::twoPi - gapAng, true);
        pw.startNewSubPath (tcx, tcy - pr + 0.5f);
        pw.lineTo          (tcx, tcy - br * 0.85f);
        g.setColour (enabled ? juce::Colours::white.withAlpha (0.9f) : juce::Colour (0xff444466));
        g.strokePath (pw, juce::PathStrokeType (1.4f, juce::PathStrokeType::curved,
                                                juce::PathStrokeType::rounded));
    }

    juce::Rectangle<float> iconR (r.getX() + 8.f, r.getY() + 34.f,
                                   r.getWidth() - 16.f, 82.f);
    drawModuleIcon (g, iconR, idx, ac, enabled);

    float dispVal = (float)knob[idx].getValue();
    juce::String valStr, unitLabel, secLabel;

    switch (idx)
    {
        case 0: { float db = (dispVal - 50.f) * 0.24f;
                  valStr = (db >= 0.f ? "+" : "") + juce::String (db, 1) + " dB";
                  unitLabel = "GAIN"; break; }
        case 1: { float db = dispVal * -0.40f;
                  valStr = juce::String (db, 1) + " dB"; unitLabel = "THRESHOLD"; break; }
        case 2: { float db = dispVal * -0.40f;
                  valStr = juce::String (db, 1) + " dB"; unitLabel = "REDUCTION"; break; }
        case 3: valStr = juce::String ((int)std::round (dispVal)) + " %"; unitLabel = "DRIVE"; break;
        case 4: { valStr = juce::String ((int)std::round (dispVal)) + " %"; unitLabel = "MIX";
                  static const char* kDivStr[] = { "1/2", "1/4", "1/8" };
                  secLabel = juce::String (kDivStr[juce::jlimit (0, 2, delayDivision)]) + " SYNC";
                  break; }
        case 5: valStr = juce::String ((int)std::round (dispVal)) + " %"; unitLabel = "MIX"; break;
        case 6: valStr = juce::String ((int)std::round (dispVal)) + " %"; unitLabel = "MIX"; break;
        default: valStr = juce::String ((int)std::round (dispVal)) + " %"; unitLabel = kMod[idx].name; break;
    }

    float valY = r.getY() + kKnobOffY + kKnobSz + 3.f;
    juce::Colour valCol = enabled ? ac : juce::Colour (0xff444466);
    g.setColour (valCol);
    g.setFont (juce::FontOptions (12.f).withStyle ("Bold"));
    g.drawText (valStr,
                juce::Rectangle<float> (r.getX(), valY, r.getWidth(), 17.f).toNearestInt(),
                juce::Justification::centred);

    g.setColour (enabled ? juce::Colour (0xff7777aa) : juce::Colour (0xff333355));
    g.setFont (juce::FontOptions (8.f).withStyle ("Bold"));
    g.drawText (unitLabel,
                juce::Rectangle<float> (r.getX(), valY + 18.f, r.getWidth(), 11.f).toNearestInt(),
                juce::Justification::centred);

    if (secLabel.isNotEmpty())
    {
        g.setColour (ac.withAlpha (enabled ? 0.60f : 0.20f));
        g.setFont (juce::FontOptions (7.5f).withStyle ("Bold"));
        g.drawText (secLabel,
                    juce::Rectangle<float> (r.getX(), valY + 29.f, r.getWidth(), 11.f).toNearestInt(),
                    juce::Justification::centred);
    }
}

void VocalForgeAudioProcessorEditor::drawModuleIcon (juce::Graphics& g,
                                                       juce::Rectangle<float> r,
                                                       int idx, juce::Colour col, bool enabled)
{
    juce::Colour drawCol = col.withAlpha (enabled ? 0.85f : 0.25f);
    juce::Colour dimCol  = col.withAlpha (enabled ? 0.20f : 0.08f);
    float cx = r.getCentreX(), cy = r.getCentreY();
    float w2 = r.getWidth() * 0.42f, h2 = r.getHeight() * 0.42f;

    g.setColour (dimCol);
    g.drawHorizontalLine ((int)cy, r.getX() + 4.f, r.getRight() - 4.f);

    switch (idx)
    {
        case 0: // EQ — live frequency response curve
        {
            double sr = audioProcessor.getSampleRate();
            if (sr < 8000.0) sr = 44100.0;
            const auto& preset = audioProcessor.getPresets()[selectedPreset];
            float kv         = (float)knob[0].getValue();
            float bellGainDb = (kv - 50.f) * 0.24f;

            auto hpC    = *juce::dsp::IIR::Coefficients<float>::makeHighPass (
                              sr, (double)preset.eqLowCutHz, 0.707);
            auto bellC  = *juce::dsp::IIR::Coefficients<float>::makePeakFilter (
                              sr, (double)preset.eqLowBellHz, (double)preset.eqLowBellQ,
                              (double)juce::Decibels::decibelsToGain (bellGainDb));
            auto shelfC = *juce::dsp::IIR::Coefficients<float>::makeHighShelf (
                              sr, (double)preset.eqAirHz, 0.707,
                              (double)juce::Decibels::decibelsToGain (preset.eqAirGain));

            constexpr int kN    = 120;
            constexpr float kDbMin = -18.f, kDbMax = 18.f;
            float logMin = std::log10 (20.f), logMax = std::log10 (20000.f);
            float zeroY = r.getY() + r.getHeight() * 0.5f;
            g.setColour (dimCol);
            g.drawHorizontalLine ((int)zeroY, r.getX() + 2.f, r.getRight() - 2.f);

            juce::Path curve;
            for (int i = 0; i < kN; ++i)
            {
                double f  = std::pow (10.0, logMin + (double)i / (kN - 1) * (logMax - logMin));
                double mag = hpC.getMagnitudeForFrequency (f, sr)
                           * bellC.getMagnitudeForFrequency (f, sr)
                           * shelfC.getMagnitudeForFrequency (f, sr);
                float magDb = (float)juce::Decibels::gainToDecibels ((float)mag);
                float yn = juce::jmap (juce::jlimit (kDbMin, kDbMax, magDb), kDbMin, kDbMax, 1.f, 0.f);
                float px = r.getX() + (float)i / (kN - 1) * r.getWidth();
                float py = r.getY() + yn * r.getHeight();
                if (i == 0) curve.startNewSubPath (px, py);
                else        curve.lineTo           (px, py);
            }
            g.setColour (drawCol);
            g.strokePath (curve, juce::PathStrokeType (1.8f, juce::PathStrokeType::curved));

            juce::Path fill = curve;
            fill.lineTo (r.getRight(), r.getBottom());
            fill.lineTo (r.getX(),     r.getBottom());
            fill.closeSubPath();
            g.setColour (col.withAlpha (enabled ? 0.10f : 0.03f));
            g.fillPath (fill);

            auto drawFreqDot = [&](double f, float gainDb)
            {
                double lf = (std::log10 (f) - logMin) / (logMax - logMin);
                float px  = r.getX() + (float)lf * r.getWidth();
                float yn  = juce::jmap (juce::jlimit (kDbMin, kDbMax, gainDb), kDbMin, kDbMax, 1.f, 0.f);
                float py  = r.getY() + yn * r.getHeight();
                g.setColour (col.withAlpha (enabled ? 0.7f : 0.25f));
                g.fillEllipse (px - 2.5f, py - 2.5f, 5.f, 5.f);
            };
            drawFreqDot ((double)preset.eqLowCutHz,  -3.f);
            drawFreqDot ((double)preset.eqLowBellHz,  bellGainDb);
            drawFreqDot ((double)preset.eqAirHz,       preset.eqAirGain);
            break;
        }
        case 1: // Compressor
        {
            const float grDb     = currentGR;
            const float maxGrDisp = 20.f;
            juce::Rectangle<float> curveR (r.getX(), r.getY(), r.getWidth(), 46.f);
            float kv    = (float)knob[1].getValue();
            float thrDb = kv * -0.40f;
            float thrNorm = juce::jmap (thrDb, -40.f, 0.f, 0.f, 1.f);
            float thrX   = curveR.getX() + thrNorm * curveR.getWidth();

            g.setColour (dimCol);
            for (float x2 = curveR.getX(); x2 < curveR.getRight(); x2 += 5.f)
                g.drawLine (x2, curveR.getY() + (x2 - curveR.getX()) / curveR.getWidth() * curveR.getHeight(),
                            std::min (x2 + 3.f, curveR.getRight()),
                            curveR.getY() + (std::min (x2 + 3.f, curveR.getRight()) - curveR.getX()) / curveR.getWidth() * curveR.getHeight(), 0.7f);

            {
                constexpr int kN2 = 80;
                constexpr float kCompKneeHalf = 3.f;
                juce::Path tc;
                for (int i = 0; i < kN2; ++i)
                {
                    float inDb  = -40.f + (float)i / (kN2 - 1) * 40.f;
                    float outDb = inDb;
                    if (inDb > thrDb - kCompKneeHalf)
                    {
                        if (inDb < thrDb + kCompKneeHalf)
                        {
                            float x2 = inDb - (thrDb - kCompKneeHalf);
                            outDb = inDb + (1.f / 4.f - 1.f) * (x2 * x2) / (2.f * 6.f);
                        }
                        else outDb = thrDb + (inDb - thrDb) / 4.f;
                    }
                    float px2 = curveR.getX() + (inDb + 40.f) / 40.f * curveR.getWidth();
                    float py2 = curveR.getBottom() - (outDb + 40.f) / 40.f * curveR.getHeight();
                    if (i == 0) tc.startNewSubPath (px2, py2);
                    else        tc.lineTo           (px2, py2);
                }
                g.setColour (drawCol);
                g.strokePath (tc, juce::PathStrokeType (1.8f, juce::PathStrokeType::curved));
                g.setColour (col.withAlpha (enabled ? 0.45f : 0.12f));
                g.drawVerticalLine ((int)thrX, curveR.getY() + 2.f, curveR.getBottom() - 2.f);
            }

            juce::Rectangle<float> meterR (r.getX() + 2.f, r.getBottom() - 30.f,
                                            r.getWidth() - 4.f, 22.f);
            g.setColour (juce::Colour (0xff0d0d1e));
            g.fillRoundedRectangle (meterR, 3.f);
            g.setColour (dimCol);
            g.drawRoundedRectangle (meterR, 3.f, 0.7f);

            float grNorm = juce::jlimit (0.f, 1.f, grDb / maxGrDisp);
            if (grNorm > 0.005f)
            {
                float fillW  = grNorm * meterR.getWidth();
                juce::Rectangle<float> fill (meterR.getRight() - fillW,
                                              meterR.getY(), fillW, meterR.getHeight());
                juce::Colour grCol = grDb < 4.f  ? juce::Colour (0xff00e0a0)
                                   : grDb < 10.f ? juce::Colour (0xffffc107)
                                                 : juce::Colour (0xffff5555);
                juce::ColourGradient grGrad (grCol.brighter (0.15f), fill.getX(), 0.f,
                                              grCol.darker  (0.1f),  fill.getRight(), 0.f, false);
                g.setGradientFill (grGrad);
                g.fillRoundedRectangle (fill, 3.f);
            }

            static const float kTicks[] = { 0.f, 3.f, 6.f, 12.f };
            g.setFont (juce::FontOptions (7.f));
            for (float t : kTicks)
            {
                float tx = meterR.getRight() - (t / maxGrDisp) * meterR.getWidth();
                g.setColour (dimCol);
                g.drawVerticalLine ((int)tx, meterR.getY() - 2.f, meterR.getBottom() + 1.f);
                g.setColour (col.withAlpha (enabled ? 0.40f : 0.12f));
                g.drawText ("-" + juce::String ((int)t),
                            juce::Rectangle<float> (tx - 8.f, meterR.getBottom() + 1.f, 16.f, 9.f)
                            .toNearestInt(), juce::Justification::centred);
            }
            break;
        }
        case 2: // De-esser
        {
            juce::Path hpCurve;
            hpCurve.startNewSubPath (r.getX() + 4.f, cy + h2 * 0.5f);
            hpCurve.lineTo          (cx - w2 * 0.2f, cy + h2 * 0.4f);
            hpCurve.cubicTo         (cx + w2 * 0.2f, cy + h2 * 0.4f,
                                     cx + w2 * 0.4f, cy - h2 * 0.8f,
                                     r.getRight() - 4.f, cy - h2 * 1.0f);
            g.setColour (drawCol);
            g.strokePath (hpCurve, juce::PathStrokeType (2.0f));
            g.setColour (col.withAlpha (enabled ? 0.5f : 0.15f));
            g.drawHorizontalLine ((int)(cy - h2 * 0.4f), r.getX() + 4.f, r.getRight() - 4.f);
            break;
        }
        case 3: // Saturation — live waveshaper curve
        {
            int   sType = audioProcessor.getSatType();
            float drive = (float)knob[3].getValue() * 0.01f;

            {
                static const char* kNames[] = { "TUBE", "TAPE", "TRANS" };
                juce::String modeStr = kNames[juce::jlimit (0, 2, sType)];
                juce::Rectangle<float> pill (r.getRight() - 36.f, r.getY() + 2.f, 34.f, 13.f);
                g.setColour (col.withAlpha (enabled ? 0.22f : 0.07f));
                g.fillRoundedRectangle (pill, 4.f);
                g.setColour (col.withAlpha (enabled ? 0.80f : 0.22f));
                g.setFont (juce::FontOptions (7.5f).withStyle ("Bold"));
                g.drawText (modeStr, pill.toNearestInt(), juce::Justification::centred);
            }

            g.setColour (dimCol);
            g.drawHorizontalLine ((int)cy, r.getX() + 2.f, r.getRight() - 2.f);

            constexpr int kNs = 100;
            juce::Path curve;
            for (int i = 0; i < kNs; ++i)
            {
                float xIn = -1.f + 2.f * (float)i / (kNs - 1);
                float yOut = 0.f;
                float d    = drive < 0.01f ? 0.01f : drive;

                switch (sType)
                {
                    case 1: { float xd = xIn * (1.f + d * 4.5f); yOut = xd / std::sqrt (1.f + xd * xd); break; }
                    case 2: { float xd = juce::jlimit (-1.f, 1.f, xIn * (1.f + d * 7.f));
                              yOut = xd - xd * xd * xd * 0.333f; break; }
                    default: { float dd = 1.f + d * 5.f;
                               if (xIn >= 0.f) yOut = std::tanh (xIn * dd * 0.85f) / (dd * 0.85f) + xIn * xIn * d * 0.05f;
                               else            yOut = std::tanh (xIn * dd * 1.15f) / (dd * 1.15f) - xIn * xIn * d * 0.05f;
                               break; }
                }

                float px = r.getX() + (float)i / (kNs - 1) * r.getWidth();
                float py = cy - juce::jlimit (-1.f, 1.f, yOut) * h2 * 0.95f;
                if (i == 0) curve.startNewSubPath (px, py);
                else        curve.lineTo           (px, py);
            }
            g.setColour (drawCol);
            g.strokePath (curve, juce::PathStrokeType (2.0f, juce::PathStrokeType::curved));
            g.setColour (col.withAlpha (enabled ? 0.18f : 0.06f));
            g.drawLine (r.getX() + 2.f, r.getBottom() - 2.f, r.getRight() - 2.f, r.getY() + 2.f, 0.8f);
            break;
        }
        case 4: // Delay
        {
            float bpm = audioProcessor.getCurrentBpm();
            g.setColour (col.withAlpha (enabled ? 0.55f : 0.18f));
            g.setFont (juce::FontOptions (8.5f).withStyle ("Bold"));
            g.drawText (juce::String ((int)bpm) + " BPM",
                        juce::Rectangle<float> (r.getX(), r.getY(), r.getWidth(), 14.f).toNearestInt(),
                        juce::Justification::centred);

            static const char* kDivNames[] = { "1/2", "1/4", "1/8" };
            for (int d = 0; d < 3; ++d)
            {
                auto db = delayDivBounds (d);
                bool active = (d == delayDivision);
                g.setColour (active ? col.withAlpha (enabled ? 0.28f : 0.10f) : juce::Colour (0xff151525));
                g.fillRoundedRectangle (db, 4.f);
                g.setColour (active ? col.withAlpha (enabled ? 0.85f : 0.28f) : col.withAlpha (enabled ? 0.22f : 0.08f));
                g.drawRoundedRectangle (db, 4.f, 1.0f);
                g.setColour (active ? juce::Colours::white.withAlpha (enabled ? 1.0f : 0.35f) : col.withAlpha (enabled ? 0.50f : 0.18f));
                g.setFont (juce::FontOptions (9.f).withStyle ("Bold"));
                g.drawText (kDivNames[d], db.toNearestInt(), juce::Justification::centred);
            }

            auto ppb = delayPingPongBounds();
            bool ppOn = delayPingPong;
            g.setColour (ppOn ? col.withAlpha (enabled ? 0.22f : 0.07f) : juce::Colour (0xff121220));
            g.fillRoundedRectangle (ppb, 4.f);
            g.setColour (ppOn ? col.withAlpha (enabled ? 0.75f : 0.22f) : col.withAlpha (enabled ? 0.20f : 0.07f));
            g.drawRoundedRectangle (ppb, 4.f, 1.0f);

            if (ppOn && enabled)
            {
                float ax = ppb.getX() + 6.f, ay = ppb.getCentreY();
                juce::Path lArr, rArr;
                lArr.startNewSubPath (ax + 6.f, ay - 4.f);
                lArr.lineTo (ax, ay);
                lArr.lineTo (ax + 6.f, ay + 4.f);
                g.setColour (col.withAlpha (0.70f));
                g.strokePath (lArr, juce::PathStrokeType (1.4f));
                float rx = ppb.getRight() - 12.f;
                rArr.startNewSubPath (rx - 6.f, ay - 4.f);
                rArr.lineTo (rx, ay);
                rArr.lineTo (rx - 6.f, ay + 4.f);
                g.strokePath (rArr, juce::PathStrokeType (1.4f));
            }

            g.setColour (ppOn ? juce::Colours::white.withAlpha (enabled ? 0.90f : 0.30f) : col.withAlpha (enabled ? 0.42f : 0.14f));
            g.setFont (juce::FontOptions (8.f).withStyle ("Bold"));
            g.drawText ("PING-PONG", ppb.toNearestInt(), juce::Justification::centred);

            {
                juce::Rectangle<float> duckR (r.getX(), r.getBottom() - 12.f, r.getWidth(), 10.f);
                g.setColour (col.withAlpha (enabled ? 0.15f : 0.05f));
                g.fillRoundedRectangle (duckR, 3.f);
                g.setColour (col.withAlpha (enabled ? 0.50f : 0.15f));
                g.setFont (juce::FontOptions (7.f).withStyle ("Bold"));
                g.drawText ("DUCK  ●", duckR.toNearestInt(), juce::Justification::centred);
            }
            break;
        }
        case 5: // Chorus
        {
            static const float offsets[] = { -6.f, 0.f, 6.f };
            static const float alphas[]  = { 0.4f, 0.85f, 0.4f };
            for (int wi = 0; wi < 3; ++wi)
            {
                juce::Path wave;
                wave.startNewSubPath (r.getX() + 4.f, cy + offsets[wi]);
                for (float wx = r.getX() + 4.f; wx < r.getRight() - 4.f; wx += 1.5f)
                {
                    float norm = (wx - (r.getX() + 4.f)) / (r.getWidth() - 8.f);
                    float wy   = cy + offsets[wi]
                                 - std::sin (norm * juce::MathConstants<float>::twoPi * 2.0f
                                             + wi * 0.6f) * h2 * 0.55f;
                    wave.lineTo (wx, wy);
                }
                g.setColour (col.withAlpha (enabled ? alphas[wi] : alphas[wi] * 0.3f));
                g.strokePath (wave, juce::PathStrokeType (wi == 1 ? 2.0f : 1.2f));
            }
            break;
        }
        case 6: // Reverb
        {
            static const float scales[] = { 0.28f, 0.52f, 0.76f, 1.0f };
            static const float as[]     = { 0.85f, 0.60f, 0.35f, 0.15f };
            for (int ri = 0; ri < 4; ++ri)
            {
                float s  = scales[ri];
                float rw = w2 * 2.f * s, rh = h2 * 1.6f * s;
                juce::Path arc;
                arc.addCentredArc (cx, cy + h2 * 0.15f, rw * 0.5f, rh * 0.5f,
                                   0.f, -2.4f, 2.4f, true);
                g.setColour (col.withAlpha (enabled ? as[ri] : as[ri] * 0.3f));
                g.strokePath (arc, juce::PathStrokeType (1.6f));
            }
            g.setColour (drawCol);
            g.fillEllipse (cx - 3.f, cy + h2 * 0.15f - 3.f, 6.f, 6.f);
            break;
        }
        default: break;
    }
}

void VocalForgeAudioProcessorEditor::drawBottomSection (juce::Graphics& g)
{
    if (audioProcessor.getPresets().isEmpty()) return;
    const auto& preset = audioProcessor.getPresets()[selectedPreset];
    juce::Colour ac    = preset.colour;

    {
        float bx = kBPX + 32.f, by = kBPY + 12.f, bs = 76.f;
        juce::ColourGradient grad (ac.darker (0.2f), bx, by + bs,
                                   ac.brighter (0.1f), bx + bs, by, false);
        g.setGradientFill (grad);
        g.fillEllipse (bx, by, bs, bs);
        g.setColour (ac.withAlpha (0.6f));
        g.drawEllipse (bx, by, bs, bs, 1.5f);

        float mx = bx + bs * 0.5f, my = by + bs * 0.5f - 2.f;
        juce::Path mic;
        mic.addRoundedRectangle (mx - 6.f, my - 8.f, 12.f, 14.f, 3.f);
        mic.addEllipse           (mx - 8.f, my - 18.f, 16.f, 16.f);
        mic.startNewSubPath      (mx, my + 6.f);
        mic.lineTo               (mx, my + 12.f);
        mic.startNewSubPath      (mx - 8.f, my + 12.f);
        mic.lineTo               (mx + 8.f, my + 12.f);
        g.setColour (juce::Colours::white.withAlpha (0.9f));
        g.strokePath (mic, juce::PathStrokeType (1.5f));
    }

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (16.f).withStyle ("Bold"));
    g.drawText (preset.name,
                juce::Rectangle<float> (kBPX + 120.f, kBPY + 14.f, 340.f, 24.f).toNearestInt(),
                juce::Justification::centredLeft);

    g.setColour (juce::Colour (0xff8888bb));
    g.setFont (juce::FontOptions (10.f));
    juce::String catStr = preset.category + " Preset  ●  Tune: " + juce::String (preset.pitchSpeedMs, 0) + "ms";
    g.drawText (catStr,
                juce::Rectangle<float> (kBPX + 120.f, kBPY + 42.f, 340.f, 16.f).toNearestInt(),
                juce::Justification::centredLeft);

    {
        static const char* kNodeNames[] = { "IN", "EQ", "COMP", "DE-ESS", "SAT",
                                            "DELAY", "CHORUS", "REVERB", "OUT" };
        static const juce::uint32 kNodeColors[] = {
            0xff5555aa, kMod[0].color, kMod[1].color, kMod[2].color, kMod[3].color,
            kMod[4].color, kMod[5].color, kMod[6].color, 0xff5555aa,
        };
        static const bool kIsModule[] = { false, true, true, true, true, true, true, true, false };

        constexpr int kN = 9;
        constexpr float kNodeR = 9.f;
        constexpr float kNodeGap = 72.f;

        float totalW = (kN - 1) * kNodeGap;
        float sfCX   = (kBPX + 470.f + kBPX + 770.f) * 0.5f;
        float nodeX0 = sfCX - totalW * 0.5f;
        float nodeY  = kBPY + 52.f;

        g.setColour (juce::Colour (0xff6666aa));
        g.setFont (juce::FontOptions (9.f).withStyle ("Bold"));
        g.drawText ("SIGNAL FLOW",
                    juce::Rectangle<float> (nodeX0 - 10.f, kBPY + 8.f, totalW + 20.f, 12.f)
                    .toNearestInt(), juce::Justification::centred);

        for (int i = 0; i < kN; ++i)
        {
            float nx  = nodeX0 + i * kNodeGap;
            bool  isM = kIsModule[i];
            int   mi  = i - 1;
            bool  on  = (!isM) || modEnabled[mi];
            juce::Colour nc = juce::Colour (kNodeColors[i]);
            juce::Colour drawC = on ? nc : nc.withAlpha (0.28f);

            if (i > 0)
            {
                float prevX = nodeX0 + (i - 1) * kNodeGap;
                int   prevMi = i - 2;
                bool  lineOn = on && ((!kIsModule[i-1]) || (prevMi >= 0 && modEnabled[prevMi]));
                juce::Colour lc = lineOn ? nc.withAlpha (0.35f) : juce::Colour (0xff1e1e38);
                g.setColour (lc);
                g.drawLine (prevX + kNodeR + 1.f, nodeY, nx - kNodeR - 1.f, nodeY, 1.2f);

                float mx2 = (prevX + kNodeR + nx - kNodeR) * 0.5f;
                juce::Path arh;
                arh.startNewSubPath (mx2 - 4.f, nodeY - 3.f);
                arh.lineTo          (mx2,        nodeY);
                arh.lineTo          (mx2 - 4.f, nodeY + 3.f);
                g.strokePath (arh, juce::PathStrokeType (1.2f));
            }

            g.setColour (drawC.withAlpha (on ? 0.18f : 0.06f));
            g.fillEllipse (nx - kNodeR, nodeY - kNodeR, kNodeR * 2.f, kNodeR * 2.f);
            g.setColour (drawC.withAlpha (on ? 0.85f : 0.30f));
            g.drawEllipse (nx - kNodeR, nodeY - kNodeR, kNodeR * 2.f, kNodeR * 2.f, on ? 1.8f : 1.0f);

            if (on) { g.setColour (drawC); g.fillEllipse (nx - 3.f, nodeY - 3.f, 6.f, 6.f); }

            g.setColour (drawC.withAlpha (on ? 0.80f : 0.35f));
            g.setFont (juce::FontOptions (7.5f).withStyle ("Bold"));
            g.drawText (kNodeNames[i],
                        juce::Rectangle<float> (nx - 20.f, nodeY + kNodeR + 3.f, 40.f, 12.f)
                        .toNearestInt(), juce::Justification::centred);
        }
    }

    {
        auto ob = oversampBounds();
        g.setColour (juce::Colour (0xff0e0e24));
        g.fillRoundedRectangle (ob, 10.f);
        g.setColour (juce::Colour (0xff2a2a50));
        g.drawRoundedRectangle (ob, 10.f, 0.8f);

        g.setColour (juce::Colour (0xff8888bb));
        g.setFont (juce::FontOptions (9.f).withStyle ("Bold"));
        g.drawText ("OVERSAMPLING",
                    juce::Rectangle<float> (ob.getX() + 10.f, ob.getY() + 10.f, 130.f, 14.f)
                    .toNearestInt(), juce::Justification::centredLeft);

        float tx = ob.getX() + 140.f, ty = ob.getY() + 8.f, tw = 44.f, th = 22.f;
        juce::Colour trackCol = oversamplingOn ? juce::Colour (0xff7B5BFF) : juce::Colour (0xff1e1e38);
        g.setColour (trackCol);
        g.fillRoundedRectangle (tx, ty, tw, th, th * 0.5f);
        g.setColour (juce::Colours::white.withAlpha (0.3f));
        g.drawRoundedRectangle (tx, ty, tw, th, th * 0.5f, 0.8f);
        float knobPos = oversamplingOn ? tx + tw - th + 2.f : tx + 2.f;
        g.setColour (juce::Colours::white);
        g.fillEllipse (knobPos, ty + 2.f, th - 4.f, th - 4.f);
        g.setColour (oversamplingOn ? juce::Colour (0xff9980ff) : juce::Colour (0xff555577));
        g.setFont (juce::FontOptions (9.f).withStyle ("Bold"));
        g.drawText (oversamplingOn ? "ON" : "OFF",
                    juce::Rectangle<float> (tx + tw + 8.f, ty, 30.f, th).toNearestInt(),
                    juce::Justification::centredLeft);

        int   latSamples = audioProcessor.calcLatencySamples();
        float latMs      = latSamples / (float)juce::jmax (1.0, audioProcessor.getSampleRate()) * 1000.f;
        g.setColour (juce::Colour (0xff6666aa));
        g.setFont (juce::FontOptions (9.f).withStyle ("Bold"));
        g.drawText ("LATENCY",
                    juce::Rectangle<float> (ob.getX() + 10.f, ob.getY() + 52.f, 80.f, 14.f)
                    .toNearestInt(), juce::Justification::centredLeft);
        g.setColour (juce::Colours::white.withAlpha (0.8f));
        g.setFont (juce::FontOptions (11.f).withStyle ("Bold"));
        g.drawText (juce::String (latSamples) + " smp  (" + juce::String (latMs, 1) + " ms)",
                    juce::Rectangle<float> (ob.getX() + 10.f, ob.getY() + 68.f, 190.f, 16.f)
                    .toNearestInt(), juce::Justification::centredLeft);
    }

    auto ib  = inputKnobBounds().toFloat();
    auto ob2 = outputKnobBounds().toFloat();
    g.setColour (juce::Colour (0xff6666aa));
    g.setFont (juce::FontOptions (8.f).withStyle ("Bold"));
    g.drawText ("IN GAIN",
                juce::Rectangle<float> (ib.getX() - 4.f, ib.getY() - 14.f, ib.getWidth() + 8.f, 12.f)
                .toNearestInt(), juce::Justification::centred);
    g.drawText ("OUT GAIN",
                juce::Rectangle<float> (ob2.getX() - 4.f, ob2.getY() - 14.f, ob2.getWidth() + 8.f, 12.f)
                .toNearestInt(), juce::Justification::centred);

    float inGainVal  = (float)inputKnob.getValue();
    float outGainVal = (float)outputKnob.getValue();
    g.setColour (juce::Colour (0xff9999cc));
    g.setFont (juce::FontOptions (8.f));
    juce::String inStr  = (inGainVal  >= 0.f ? "+" : "") + juce::String (inGainVal,  1) + "dB";
    juce::String outStr = (outGainVal >= 0.f ? "+" : "") + juce::String (outGainVal, 1) + "dB";
    g.drawText (inStr,
                juce::Rectangle<float> (ib.getX() - 4.f, ib.getBottom() + 2.f, ib.getWidth() + 8.f, 12.f)
                .toNearestInt(), juce::Justification::centred);
    g.drawText (outStr,
                juce::Rectangle<float> (ob2.getX() - 4.f, ob2.getBottom() + 2.f, ob2.getWidth() + 8.f, 12.f)
                .toNearestInt(), juce::Justification::centred);
}

void VocalForgeAudioProcessorEditor::showPitchMenu()
{
    static const float      kSpeedMs[]    = { 1.f, 8.f, 50.f, 150.f, 300.f };
    static const char*      kSpeedNames[] = { "Robot (1 ms)",   "Fast (8 ms)",
                                              "Medium (50 ms)", "Slow (150 ms)",
                                              "Natural (300 ms)" };

    juce::PopupMenu menu;
    menu.addSectionHeader ("RETUNE SPEED");
    for (int i = 0; i < 5; ++i)
        menu.addItem (i + 1, kSpeedNames[i], true, (pitchSpeedTier == i));

    menu.addSeparator();
    menu.addItem (10, "Humanize", true, pitchHumanizeOn);

    auto screenBounds = localAreaToGlobal (pitchMenuBtnBounds().toNearestInt());

    menu.showMenuAsync (
        juce::PopupMenu::Options().withTargetScreenArea (screenBounds),
        [this] (int result)
        {
            if (result >= 1 && result <= 5)
            {
                static const float kMs[] = { 1.f, 8.f, 50.f, 150.f, 300.f };
                pitchSpeedTier = result - 1;
                audioProcessor.setPitchRetuneSpeed (kMs[pitchSpeedTier]);
                repaint();
            }
            else if (result == 10)
            {
                pitchHumanizeOn = !pitchHumanizeOn;
                audioProcessor.setPitchHumanize (pitchHumanizeOn ? 1.f : 0.f);
                repaint();
            }
        });
}
