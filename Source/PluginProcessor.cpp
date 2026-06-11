#include "PluginProcessor.h"
#include "PluginEditor.h"

VocalForgeAudioProcessor::VocalForgeAudioProcessor()
    : AudioProcessor (BusesProperties()
                      .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
{
    logger = new juce::FileLogger (
        juce::File::getSpecialLocation (juce::File::tempDirectory).getChildFile ("VocalForge.log"),
        "VocalForge Log");
    logger->writeToLog ("=== VocalForge constructor ===");
    logger->writeToLog ("  Name: " + getName());
    logger->writeToLog ("  PluginCode: " + juce::String::toHexString ((int)JucePlugin_PluginCode));
    initPresets();
}

VocalForgeAudioProcessor::~VocalForgeAudioProcessor()
{
    if (logger) { logger->writeToLog ("=== VocalForge destructor ==="); delete logger; logger = nullptr; }
}

void VocalForgeAudioProcessor::initPresets()
{
    presets.clear();

    // ---- 1. Clean Pop ----
    { VocalPreset p; p.name="Clean Pop"; p.category="Pop"; p.colour=juce::Colour::fromRGB(120,80,255);
      p.eqLowCutHz=90.f; p.eqLowBellHz=400.f; p.eqLowBellGain=-2.f; p.eqLowBellQ=0.9f;
      p.eqAirHz=10000.f; p.eqAirGain=3.f;
      p.compThresholdDb=-20.f; p.compRatio=4.f; p.compAttackMs=22.f; p.compReleaseMs=90.f; p.compMakeupGain=2.f;
      p.deEssLowThreshDb=-22.f; p.deEssHighThreshDb=-18.f;
      p.satType=0; p.satDrive=0.08f; p.satMix=0.25f; p.pitchSpeedMs=35.f;
      p.reverbRoomSize=0.22f; p.reverbDamping=0.68f; p.reverbWet=0.15f; p.reverbWidth=0.88f;
      p.delayTimeMs=220.f; p.delayFeedback=0.16f; p.delayWet=0.07f; p.delayDivision=1; p.delayPingPong=false;
      p.chorusRate=1.0f; p.chorusDepth=0.04f; p.chorusMix=0.04f; presets.add(p); }

    // ---- 2. Trap Vibes ----
    { VocalPreset p; p.name="Trap Vibes"; p.category="Trap"; p.colour=juce::Colour::fromRGB(255,60,120);
      p.eqLowCutHz=90.f; p.eqLowBellHz=400.f; p.eqLowBellGain=-1.f; p.eqLowBellQ=0.8f;
      p.eqAirHz=10000.f; p.eqAirGain=4.f;
      p.compThresholdDb=-24.f; p.compRatio=4.f; p.compAttackMs=10.f; p.compReleaseMs=55.f; p.compMakeupGain=4.f;
      p.deEssLowThreshDb=-20.f; p.deEssHighThreshDb=-22.f;
      p.satType=1; p.satDrive=0.3f; p.satMix=0.5f; p.pitchSpeedMs=10.f;
      p.reverbRoomSize=0.72f; p.reverbDamping=0.15f; p.reverbWet=0.40f; p.reverbWidth=1.0f;
      p.delayTimeMs=125.f; p.delayFeedback=0.52f; p.delayWet=0.26f; p.delayDivision=2; p.delayPingPong=true;
      p.chorusRate=0.6f; p.chorusDepth=0.05f; p.chorusMix=0.06f; p.chorusMode=1; p.reverbDuckEnabled=true;
      presets.add(p); }

    // ---- 3. Smooth R&B ----
    { VocalPreset p; p.name="Smooth R&B"; p.category="R&B"; p.colour=juce::Colour::fromRGB(0,210,180);
      p.eqLowCutHz=90.f; p.eqLowBellHz=400.f; p.eqLowBellGain=-3.f; p.eqLowBellQ=0.6f;
      p.eqAirHz=10000.f; p.eqAirGain=1.5f;
      p.compThresholdDb=-16.f; p.compRatio=4.f; p.compAttackMs=32.f; p.compReleaseMs=130.f; p.compMakeupGain=1.5f;
      p.deEssLowThreshDb=-24.f; p.deEssHighThreshDb=-16.f;
      p.satType=0; p.satDrive=0.06f; p.satMix=0.20f; p.pitchSpeedMs=25.f;
      p.reverbRoomSize=0.65f; p.reverbDamping=0.55f; p.reverbWet=0.30f; p.reverbWidth=0.90f;
      p.delayTimeMs=300.f; p.delayFeedback=0.32f; p.delayWet=0.16f; p.delayDivision=1; p.delayPingPong=false;
      p.chorusRate=0.8f; p.chorusDepth=0.08f; p.chorusMix=0.06f; presets.add(p); }

    // ---- 4. Afrobeat ----
    { VocalPreset p; p.name="Afrobeat"; p.category="Afro"; p.colour=juce::Colour::fromRGB(255,160,0);
      p.eqLowCutHz=90.f; p.eqLowBellHz=400.f; p.eqLowBellGain=0.5f; p.eqLowBellQ=1.0f;
      p.eqAirHz=10000.f; p.eqAirGain=2.5f;
      p.compThresholdDb=-18.f; p.compRatio=4.f; p.compAttackMs=16.f; p.compReleaseMs=75.f; p.compMakeupGain=3.f;
      p.deEssLowThreshDb=-22.f; p.deEssHighThreshDb=-20.f;
      p.satType=2; p.satDrive=0.18f; p.satMix=0.36f; p.pitchSpeedMs=55.f;
      p.reverbRoomSize=0.28f; p.reverbDamping=0.55f; p.reverbWet=0.11f; p.reverbWidth=0.85f;
      p.delayTimeMs=187.f; p.delayFeedback=0.26f; p.delayWet=0.11f; p.delayDivision=1; p.delayPingPong=true;
      p.chorusRate=2.0f; p.chorusDepth=0.12f; p.chorusMix=0.06f; presets.add(p); }

    // ---- 5. Dark Drill ----
    { VocalPreset p; p.name="Dark Drill"; p.category="Drill"; p.colour=juce::Colour::fromRGB(80,80,200);
      p.eqLowCutHz=90.f; p.eqLowBellHz=400.f; p.eqLowBellGain=-2.f; p.eqLowBellQ=0.7f;
      p.eqAirHz=10000.f; p.eqAirGain=1.5f;
      p.compThresholdDb=-22.f; p.compRatio=4.f; p.compAttackMs=8.f; p.compReleaseMs=55.f; p.compMakeupGain=5.f;
      p.deEssLowThreshDb=-18.f; p.deEssHighThreshDb=-24.f;
      p.satType=1; p.satDrive=0.35f; p.satMix=0.55f; p.pitchSpeedMs=8.f;
      p.reverbRoomSize=0.75f; p.reverbDamping=0.45f; p.reverbWet=0.28f; p.reverbWidth=1.0f;
      p.delayTimeMs=500.f; p.delayFeedback=0.32f; p.delayWet=0.14f; p.delayDivision=2; p.delayPingPong=false;
      p.chorusRate=0.4f; p.chorusDepth=0.03f; p.chorusMix=0.03f; p.chorusMode=1; presets.add(p); }

    // ---- 6. Gospel Power ----
    { VocalPreset p; p.name="Gospel Power"; p.category="Gospel"; p.colour=juce::Colour::fromRGB(255,100,60);
      p.eqLowCutHz=90.f; p.eqLowBellHz=400.f; p.eqLowBellGain=2.f; p.eqLowBellQ=0.8f;
      p.eqAirHz=10000.f; p.eqAirGain=3.f;
      p.compThresholdDb=-14.f; p.compRatio=4.f; p.compAttackMs=35.f; p.compReleaseMs=150.f; p.compMakeupGain=1.f;
      p.deEssLowThreshDb=-26.f; p.deEssHighThreshDb=-14.f;
      p.satType=0; p.satDrive=0.08f; p.satMix=0.25f; p.pitchSpeedMs=100.f;
      p.reverbRoomSize=0.6f; p.reverbDamping=0.5f; p.reverbWet=0.35f; p.reverbWidth=1.0f;
      p.delayTimeMs=400.f; p.delayFeedback=0.32f; p.delayWet=0.20f; p.delayDivision=0; p.delayPingPong=false;
      p.chorusRate=0.7f; p.chorusDepth=0.18f; p.chorusMix=0.38f; p.chorusMode=2; presets.add(p); }
}

void VocalForgeAudioProcessor::loadPreset (int index)
{
    if (index < 0 || index >= presets.size()) return;
    currentPresetIndex = index;
    applyPresetToDSP (presets[index]);
}

void VocalForgeAudioProcessor::applyPresetToDSP (const VocalPreset& p)
{
    if (currentSampleRate <= 0.0) return;
    auto sr = currentSampleRate;

    eqChain.get<0>().coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass (sr, p.eqLowCutHz, 0.707f);
    eqChain.get<1>().coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter (
        sr, p.eqLowBellHz, p.eqLowBellQ, juce::Decibels::decibelsToGain (p.eqLowBellGain));
    eqChain.get<2>().coefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf (
        sr, p.eqAirHz, 0.707f, juce::Decibels::decibelsToGain (p.eqAirGain));

    compThreshDb = p.compThresholdDb;
    compRatio    = p.compRatio;
    compMakeupDb = p.compMakeupGain;
    auto msToCoeff = [&](float ms) -> float { return 1.f - std::exp (-1.f / (ms * 0.001f * (float)sr)); };
    optoAttCoeff  = msToCoeff (p.compAttackMs);
    optoFRelCoeff = msToCoeff (p.compReleaseMs);
    optoSRelCoeff = msToCoeff (p.compReleaseMs * 4.f);

    auto lpCoeffs   = juce::dsp::IIR::Coefficients<float>::makeLowPass  (sr, kDeEssCrossover, 0.707f);
    auto hpCoeffs2  = juce::dsp::IIR::Coefficients<float>::makeHighPass (sr, kDeEssCrossover, 0.707f);
    auto lpHiCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass  (sr, 12000.f, 0.707f);
    deEssLowDetect.coefficients  = lpCoeffs;
    deEssHighDetect.coefficients = hpCoeffs2;
    deEssHighLP.coefficients     = lpHiCoeffs;
    deEssLowSplit[0].coefficients = lpCoeffs;
    deEssLowSplit[1].coefficients = lpCoeffs;
    deEssLowThreshLin  = juce::Decibels::decibelsToGain (p.deEssLowThreshDb);
    deEssHighThreshLin = juce::Decibels::decibelsToGain (p.deEssHighThreshDb);

    satType  = p.satType;
    satDrive = p.satDrive;
    satMix   = p.satMix;

    reverbParams.roomSize   = p.reverbRoomSize;
    reverbParams.damping    = p.reverbDamping;
    reverbParams.wetLevel   = p.reverbWet;
    reverbParams.dryLevel   = 1.f - p.reverbWet;
    reverbParams.width      = p.reverbWidth;
    reverb.setParameters (reverbParams);
    reverbDuckEnabled = p.reverbDuckEnabled;
    reverbDuckEnv     = 0.f;

    { currentPitchSpeedMs = p.pitchSpeedMs;
      float speedSec = juce::jmax (0.001f, p.pitchSpeedMs * 0.001f);
      pitchSmoothCoeff = 1.f - std::exp (-1.f / (speedSec * (float)currentSampleRate));
      pitchRatioTarget = pitchRatioSmooth = 1.f; }

    delayFeedback  = p.delayFeedback;
    delayWet       = p.delayWet;
    delayDivision  = p.delayDivision;
    delayPingPong  = p.delayPingPong;
    delayFbLP_L = delayFbLP_R = 0.f;

    chRate     = p.chorusRate;
    chDepthSmp = p.chorusDepth * (float)sr * 0.004f;
    chMix      = p.chorusMix;
    chMode     = p.chorusMode;
}
void VocalForgeAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    logger->writeToLog ("prepareToPlay sr=" + juce::String (sampleRate) + " spb=" + juce::String (samplesPerBlock));

    juce::dsp::ProcessSpec spec;
    spec.sampleRate       = sampleRate;
    spec.maximumBlockSize = (juce::uint32) samplesPerBlock;
    spec.numChannels      = 2;

    eqChain.prepare (spec);
    deEssLowDetect .reset();
    deEssHighDetect.reset();
    deEssHighLP    .reset();
    deEssLowSplit[0].reset();
    deEssLowSplit[1].reset();

    oversampler = std::make_unique<juce::dsp::Oversampling<float>> (
        2, 1, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, false);
    oversampler->initProcessing ((size_t) samplesPerBlock);

    int maxDelaySamples = (int) (sampleRate * 2.0);
    delayLineL.setMaximumDelayInSamples (maxDelaySamples);
    delayLineR.setMaximumDelayInSamples (maxDelaySamples);
    delayLineL.reset();
    delayLineR.reset();

    reverbMidLP.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass  ((float)sampleRate, 800.f,  0.707f);
    reverbMidHP.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass ((float)sampleRate, 3000.f, 0.707f);
    reverb.reset();

    int pitchBufSamples = (int) (sampleRate * 0.5);
    pitchBuf.assign (pitchBufSamples, 0.f);
    pitchBufPos = 0;

    grainBuf.assign ((int)(sampleRate * 2.0), 0.f);
    grainBufPos = 0;
    grainOutBuf.assign (samplesPerBlock, 0.f);

    optoEnvDb = 0.f;
    deEssLowEnv = deEssHighEnv = 0.f;

    for (auto& g : grains)
        g = GrainPlayer();

    if (currentPresetIndex >= 0 && currentPresetIndex < presets.size())
        applyPresetToDSP (presets[currentPresetIndex]);
}

void VocalForgeAudioProcessor::releaseResources() {}

void VocalForgeAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& /*midiMessages*/)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // --- Input level meter ---
    {
        float peak = 0.f;
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            peak = std::max (peak, buffer.getMagnitude (ch, 0, buffer.getNumSamples()));
        inputLevelDb.store (peak > 1e-6f ? juce::Decibels::gainToDecibels (peak) : -100.f);
    }

    // --- Master bypass ---
    if (masterBypass.load()) {
        float peak = 0.f;
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            peak = std::max (peak, buffer.getMagnitude (ch, 0, buffer.getNumSamples()));
        outputLevelDb.store (peak > 1e-6f ? juce::Decibels::gainToDecibels (peak) : -100.f);
        return;
    }

    const int numSamples = buffer.getNumSamples();
    float* L = buffer.getWritePointer (0);
    float* R = (buffer.getNumChannels() > 1) ? buffer.getWritePointer (1) : L;

    // ── 1. EQ ────────────────────────────────────────────────────────────────
    {
        juce::dsp::AudioBlock<float> block (buffer);
        juce::dsp::ProcessContextReplacing<float> ctx (block);
        eqChain.process (ctx);
    }

    // ── 2. Opto Compressor ───────────────────────────────────────────────────
    {
        float thrLin   = juce::Decibels::decibelsToGain (compThreshDb);
        float makeupLin = juce::Decibels::decibelsToGain (compMakeupDb);
        for (int n = 0; n < numSamples; ++n)
        {
            float s = 0.5f * (std::abs(L[n]) + std::abs(R[n]));
            float sDb = (s > 1e-6f) ? juce::Decibels::gainToDecibels (s) : -100.f;
            float targDb = (sDb > compThreshDb)
                           ? compThreshDb + (sDb - compThreshDb) / compRatio
                           : sDb;
            float gainDb = targDb - sDb;
            float coeff  = (gainDb < optoEnvDb) ? optoAttCoeff : optoFRelCoeff;
            optoEnvDb += coeff * (gainDb - optoEnvDb);
            float g = juce::Decibels::decibelsToGain (optoEnvDb) * makeupLin;
            L[n] *= g; R[n] *= g;
        }
        float peakGR = std::abs (optoEnvDb);
        compGainReductionDb.store (peakGR);
    }

    // ── 3. De-esser ──────────────────────────────────────────────────────────
    {
        auto envAtt = 1.f - std::exp (-1.f / (0.5f  * (float)currentSampleRate * 0.001f));
        auto envRel = 1.f - std::exp (-1.f / (50.f  * (float)currentSampleRate * 0.001f));
        auto envAttH = 1.f - std::exp (-1.f / (0.3f * (float)currentSampleRate * 0.001f));
        auto envRelH = 1.f - std::exp (-1.f / (25.f * (float)currentSampleRate * 0.001f));

        for (int n = 0; n < numSamples; ++n)
        {
            float mid = 0.5f * (L[n] + R[n]);
            float lowDet  = deEssLowDetect .processSample (mid);
            float highDet = deEssHighDetect.processSample (mid);
            highDet       = deEssHighLP    .processSample (highDet);

            float lowAbs  = std::abs (lowDet);
            float highAbs = std::abs (highDet);
            if (lowAbs > deEssLowEnv)  deEssLowEnv  += envAtt  * (lowAbs  - deEssLowEnv);
            else                        deEssLowEnv  += envRel  * (lowAbs  - deEssLowEnv);
            if (highAbs > deEssHighEnv) deEssHighEnv += envAttH * (highAbs - deEssHighEnv);
            else                        deEssHighEnv += envRelH * (highAbs - deEssHighEnv);

            float lowGain  = (deEssLowEnv  > deEssLowThreshLin)
                             ? deEssLowThreshLin  / (deEssLowEnv  + 1e-9f) : 1.f;
            float highGain = (deEssHighEnv > deEssHighThreshLin)
                             ? deEssHighThreshLin / (deEssHighEnv + 1e-9f) : 1.f;

            float loL = deEssLowSplit[0].processSample (L[n]);
            float loR = deEssLowSplit[1].processSample (R[n]);
            float hiL = L[n] - loL;
            float hiR = R[n] - loR;
            L[n] = loL * lowGain + hiL * highGain;
            R[n] = loR * lowGain + hiR * highGain;
        }
    }

    // ── 4. Pitch Correction ──────────────────────────────────────────────────
    if (!pitchBypass)
    {
        // Write into ring buffer
        for (int n = 0; n < numSamples; ++n)
        {
            grainBuf[grainBufPos] = 0.5f * (L[n] + R[n]);
            grainBufPos = (grainBufPos + 1) % (int)grainBuf.size();
        }

        runPitchDetect();

        if (pitchRatioTarget > 0.f)
        {
            float coeff = pitchSmoothCoeff;
            pitchRatioSmooth += coeff * (pitchRatioTarget - pitchRatioSmooth);
        }

        float ratio = pitchRatioSmooth;
        if (std::abs (ratio - 1.f) > 0.0005f)
        {
            // Granular OLA pitch shift on mono mix, then apply to both channels
            int grainSz  = 2048;
            int hopOut   = 256;
            int hopIn    = (int)(hopOut * ratio + 0.5f);
            hopIn = juce::jmax (1, hopIn);

            std::vector<float> out (numSamples, 0.f);
            for (int n = 0; n < numSamples; n += hopOut)
            {
                int startRead = (grainBufPos - grainSz + n - numSamples + (int)grainBuf.size()) % (int)grainBuf.size();
                for (int k = 0; k < grainSz && (n + k) < numSamples; ++k)
                {
                    float win = 0.5f - 0.5f * std::cos (2.f * juce::MathConstants<float>::pi * k / (grainSz - 1));
                    int idx = (startRead + (int)(k * ratio)) % (int)grainBuf.size();
                    out[n + k] += grainBuf[idx] * win;
                }
            }
            // Apply pitch-corrected mono to L and R
            for (int n = 0; n < numSamples; ++n) { L[n] = out[n]; R[n] = out[n]; }
        }
    }
    // ── 5. Saturation ────────────────────────────────────────────────────────
    {
        auto processSat = [&](float* ch, int n)
        {
            if (oversamplingEnabled && oversampler)
            {
                juce::AudioBuffer<float> tmp (1, n);
                tmp.copyFrom (0, 0, ch, n);
                juce::dsp::AudioBlock<float> blk (tmp);
                auto upBlk = oversampler->processSamplesUp (blk);
                float* up = upBlk.getChannelPointer (0);
                int upN   = (int) upBlk.getNumSamples();
                for (int k = 0; k < upN; ++k)
                {
                    float dry = up[k];
                    float wet = (satType == 0) ? satTube (dry, satDrive)
                              : (satType == 1) ? satTape (dry, satDrive)
                              :                  satTransistor (dry, satDrive);
                    up[k] = dry + satMix * (wet - dry);
                }
                oversampler->processSamplesDown (blk);
                ch[0]; // already updated in-place via AudioBlock
                tmp.copyTo (ch, 0, 0, n);
            }
            else
            {
                for (int k = 0; k < n; ++k)
                {
                    float dry = ch[k];
                    float wet = (satType == 0) ? satTube (dry, satDrive)
                              : (satType == 1) ? satTape (dry, satDrive)
                              :                  satTransistor (dry, satDrive);
                    ch[k] = dry + satMix * (wet - dry);
                }
            }
        };
        processSat (L, numSamples);
        processSat (R, numSamples);
    }

    // ── 6. Delay ─────────────────────────────────────────────────────────────
    {
        float delayMs = 0.f;
        if (auto* ph = getPlayHead())
        {
            if (auto pos = ph->getPosition())
            {
                if (pos->getBpm().hasValue())
                {
                    float bpm = (float) *pos->getBpm();
                    if (bpm > 0.f)
                    {
                        float beatMs = 60000.f / bpm;
                        if      (delayDivision == 0) delayMs = beatMs * 2.f;
                        else if (delayDivision == 1) delayMs = beatMs;
                        else if (delayDivision == 2) delayMs = beatMs * 0.5f;
                        else if (delayDivision == 3) delayMs = beatMs * 0.25f;
                        else                          delayMs = beatMs * (2.f / 3.f);
                    }
                }
            }
        }
        if (delayMs <= 0.f) delayMs = 500.f;

        float delaySamples = delayMs * 0.001f * (float)currentSampleRate;
        delaySamples = juce::jlimit (1.f, (float)(delayLineL.getMaximumDelayInSamples() - 1), delaySamples);

        const float fbLPCoeff = 1.f - std::exp (-1.f / (20.f * (float)currentSampleRate * 0.001f));

        for (int n = 0; n < numSamples; ++n)
        {
            float dL = delayLineL.popSample (0, delaySamples, true);
            float dR = delayLineR.popSample (0, delaySamples, true);

            delayFbLP_L += fbLPCoeff * (dL - delayFbLP_L);
            delayFbLP_R += fbLPCoeff * (dR - delayFbLP_R);

            if (delayPingPong)
            {
                delayLineL.pushSample (0, L[n] + delayFbLP_R * delayFeedback);
                delayLineR.pushSample (0, R[n] + delayFbLP_L * delayFeedback);
            }
            else
            {
                delayLineL.pushSample (0, L[n] + delayFbLP_L * delayFeedback);
                delayLineR.pushSample (0, R[n] + delayFbLP_R * delayFeedback);
            }
            L[n] += delayWet * dL;
            R[n] += delayWet * dR;
        }
    }

    // ── 7. Chorus ────────────────────────────────────────────────────────────
    {
        float chPhaseInc = chRate * juce::MathConstants<float>::twoPi / (float)currentSampleRate;
        int   maxChDelay = (int)(chDepthSmp * 2.f) + 16;

        for (int n = 0; n < numSamples; ++n)
        {
            float lfoL, lfoR;
            if (chMode == 0) // stereo
            {
                lfoL =  std::sin (chPhase);
                lfoR = -std::sin (chPhase);
            }
            else if (chMode == 1) // mono flanger
            {
                lfoL = lfoR = std::sin (chPhase);
            }
            else // wide ensemble: 3 voices
            {
                lfoL =  0.5f * std::sin (chPhase) + 0.5f * std::sin (chPhase * 1.5f);
                lfoR = -0.5f * std::sin (chPhase) + 0.5f * std::sin (chPhase * 0.75f);
            }
            chPhase += chPhaseInc;
            if (chPhase > juce::MathConstants<float>::twoPi)
                chPhase -= juce::MathConstants<float>::twoPi;

            int dL = (int)(chDepthSmp * (1.f + lfoL)) + 1;
            int dR = (int)(chDepthSmp * (1.f + lfoR)) + 1;
            dL = juce::jlimit (1, maxChDelay, dL);
            dR = juce::jlimit (1, maxChDelay, dR);

            // Simple allpass chorus approximation using circular history
            int hLen = maxChDelay + 2;
            chHistL.resize (hLen, 0.f);
            chHistR.resize (hLen, 0.f);
            chHistL[chHistPos % hLen] = L[n];
            chHistR[chHistPos % hLen] = R[n];
            int idxL = (chHistPos - dL + hLen * 16) % hLen;
            int idxR = (chHistPos - dR + hLen * 16) % hLen;
            L[n] = (1.f - chMix) * L[n] + chMix * chHistL[idxL];
            R[n] = (1.f - chMix) * R[n] + chMix * chHistR[idxR];
            chHistPos = (chHistPos + 1) % hLen;
        }
    }

    // ── 8. Reverb (with mid EQ on wet + optional ducking) ────────────────────
    {
        if (reverbDuckEnabled)
        {
            float envAtt = 1.f - std::exp (-1.f / (5.f  * (float)currentSampleRate * 0.001f));
            float envRel = 1.f - std::exp (-1.f / (200.f * (float)currentSampleRate * 0.001f));
            for (int n = 0; n < numSamples; ++n)
            {
                float s = 0.5f * (std::abs(L[n]) + std::abs(R[n]));
                if (s > reverbDuckEnv) reverbDuckEnv += envAtt * (s - reverbDuckEnv);
                else                   reverbDuckEnv += envRel * (s - reverbDuckEnv);
            }
            float duckGain = 1.f / (1.f + reverbDuckEnv * 4.f);
            reverbParams.wetLevel = presets[currentPresetIndex].reverbWet * duckGain;
            reverbParams.dryLevel = 1.f - reverbParams.wetLevel;
            reverb.setParameters (reverbParams);
        }

        juce::AudioBuffer<float> wetBuf (2, numSamples);
        wetBuf.copyFrom (0, 0, buffer, 0, 0, numSamples);
        wetBuf.copyFrom (1, 0, buffer, 1, 0, numSamples);

        float* wL = wetBuf.getWritePointer (0);
        float* wR = wetBuf.getWritePointer (1);

        // Apply reverb mid EQ to wet signal only
        for (int n = 0; n < numSamples; ++n)
        {
            float mid  = 0.5f * (wL[n] + wR[n]);
            float side = 0.5f * (wL[n] - wR[n]);
            float lo   = reverbMidLP.processSample (mid);
            float hi   = reverbMidHP.processSample (mid);
            float midEQ = lo * 0.85f + (mid - lo - hi) + hi * 1.1f;
            wL[n] = midEQ + side;
            wR[n] = midEQ - side;
        }

        reverb.processStereo (wL, wR, numSamples);
        buffer.copyFrom (0, 0, wetBuf, 0, 0, numSamples);
        buffer.copyFrom (1, 0, wetBuf, 1, 0, numSamples);
    }

    // --- Output level meter ---
    {
        float peak = 0.f;
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            peak = std::max (peak, buffer.getMagnitude (ch, 0, buffer.getNumSamples()));
        outputLevelDb.store (peak > 1e-6f ? juce::Decibels::gainToDecibels (peak) : -100.f);
    }
}
// ── Saturation helpers ───────────────────────────────────────────────────────
float VocalForgeAudioProcessor::satTube (float x, float drive) noexcept
{
    float g = 1.f + drive * 8.f;
    float y = x * g;
    return y / (1.f + std::abs (y)) * (1.f / (1.f + drive * 0.5f));
}

float VocalForgeAudioProcessor::satTape (float x, float drive) noexcept
{
    float g = 1.f + drive * 12.f;
    float y = x * g;
    float k = 2.f + drive * 4.f;
    return (float)std::tanh ((double)y * k) / (float)std::tanh ((double)k);
}

float VocalForgeAudioProcessor::satTransistor (float x, float drive) noexcept
{
    float g  = 1.f + drive * 15.f;
    float y  = x * g;
    float th = 0.3f + (1.f - drive) * 0.4f;
    if (y >  th) return  th + (y - th) / (1.f + ((y - th) / (1.f - th)) * 3.f);
    if (y < -th) return -th + (y + th) / (1.f + ((-y - th) / (1.f - th)) * 3.f);
    return y;
}

// ── Pitch detection (NCC) ────────────────────────────────────────────────────
void VocalForgeAudioProcessor::runPitchDetect()
{
    if (currentSampleRate <= 0.0) return;

    const int analysisLen = 2048;
    const int minLag = (int)(currentSampleRate / 1200.0);
    const int maxLag = (int)(currentSampleRate / 50.0);

    if ((int)pitchBuf.size() < analysisLen) return;
    int startPos = (pitchBufPos - analysisLen + (int)pitchBuf.size()) % (int)pitchBuf.size();

    std::vector<float> frame (analysisLen);
    for (int i = 0; i < analysisLen; ++i)
        frame[i] = pitchBuf[(startPos + i) % (int)pitchBuf.size()];

    // NCC
    float bestCorr = 0.f;
    int   bestLag  = 0;
    float energy0  = 0.f;
    for (int i = 0; i < analysisLen; ++i) energy0 += frame[i] * frame[i];
    if (energy0 < 1e-10f) { pitchRatioTarget = 1.f; return; }

    for (int lag = minLag; lag <= maxLag && lag < analysisLen; ++lag)
    {
        float corr = 0.f, energyLag = 0.f;
        for (int i = 0; i < analysisLen - lag; ++i)
        {
            corr      += frame[i] * frame[i + lag];
            energyLag += frame[i + lag] * frame[i + lag];
        }
        float normCorr = corr / (std::sqrt (energy0 * energyLag) + 1e-9f);
        if (normCorr > bestCorr) { bestCorr = normCorr; bestLag = lag; }
    }

    if (bestLag <= 0 || bestCorr < 0.3f) { pitchRatioTarget = 1.f; return; }

    float detectedHz = (float)currentSampleRate / (float)bestLag;
    float targetHz   = nearestNoteHz (detectedHz);
    pitchRatioTarget = targetHz / detectedHz;
}

float VocalForgeAudioProcessor::nearestNoteHz (float hz) const noexcept
{
    if (hz < 50.f || hz > 1200.f) return hz;
    // 12 semitone intervals in current key+scale (simplified: chromatic snap)
    static const float noteFreqs[128] = {
        8.18f,8.66f,9.18f,9.72f,10.30f,10.91f,11.56f,12.25f,12.98f,13.75f,
        14.57f,15.43f,16.35f,17.32f,18.35f,19.45f,20.60f,21.83f,23.12f,24.50f,
        25.96f,27.50f,29.14f,30.87f,32.70f,34.65f,36.71f,38.89f,41.20f,43.65f,
        46.25f,49.00f,51.91f,55.00f,58.27f,61.74f,65.41f,69.30f,73.42f,77.78f,
        82.41f,87.31f,92.50f,98.00f,103.83f,110.00f,116.54f,123.47f,130.81f,138.59f,
        146.83f,155.56f,164.81f,174.61f,185.00f,196.00f,207.65f,220.00f,233.08f,246.94f,
        261.63f,277.18f,293.66f,311.13f,329.63f,349.23f,369.99f,392.00f,415.30f,440.00f,
        466.16f,493.88f,523.25f,554.37f,587.33f,622.25f,659.26f,698.46f,739.99f,783.99f,
        830.61f,880.00f,932.33f,987.77f,1046.50f,1108.73f,1174.66f,1244.51f,1318.51f,1396.91f,
        1479.98f,1567.98f,1661.22f,1760.00f,1864.66f,1975.53f,2093.00f,2217.46f,2349.32f,2489.02f,
        2637.02f,2793.83f,2959.96f,3135.96f,3322.44f,3520.00f,3729.31f,3951.07f,4186.01f,4434.92f,
        4698.64f,4978.03f,5274.04f,5587.65f,5919.91f,6271.93f,6644.88f,7040.00f,7458.62f,7902.13f,
        8372.02f,8869.84f,9397.27f,9956.06f,10548.08f,11175.30f,11839.82f,12543.85f
    };
    float bestDist = 1e9f;
    float bestFreq = hz;
    for (int i = 0; i < 128; ++i)
    {
        float d = std::abs (noteFreqs[i] - hz);
        if (d < bestDist) { bestDist = d; bestFreq = noteFreqs[i]; }
    }
    return bestFreq;
}

// ── Standard JUCE boilerplate ────────────────────────────────────────────────
bool  VocalForgeAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    return layouts.getMainInputChannelSet() == layouts.getMainOutputChannelSet();
}

bool  VocalForgeAudioProcessor::hasEditor()        const { return true; }
const juce::String VocalForgeAudioProcessor::getName() const { return JucePlugin_Name; }
bool  VocalForgeAudioProcessor::acceptsMidi()       const { return false; }
bool  VocalForgeAudioProcessor::producesMidi()      const { return false; }
bool  VocalForgeAudioProcessor::isMidiEffect()      const { return false; }
double VocalForgeAudioProcessor::getTailLengthSeconds() const { return 2.0; }
int   VocalForgeAudioProcessor::getNumPrograms()    { return 1; }
int   VocalForgeAudioProcessor::getCurrentProgram() { return 0; }
void  VocalForgeAudioProcessor::setCurrentProgram  (int) {}
const juce::String VocalForgeAudioProcessor::getProgramName (int) { return {}; }
void  VocalForgeAudioProcessor::changeProgramName  (int, const juce::String&) {}

void VocalForgeAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream stream (destData, true);
    stream.writeInt (currentPresetIndex);
}

void VocalForgeAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    juce::MemoryInputStream stream (data, (size_t) sizeInBytes, false);
    int idx = stream.readInt();
    loadPreset (idx);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VocalForgeAudioProcessor();
}

juce::AudioProcessorEditor* VocalForgeAudioProcessor::createEditor()
{
    return new VocalForgeAudioProcessorEditor (*this);
}

// ── Parameter setters ────────────────────────────────────────────────────────
void VocalForgeAudioProcessor::setOversamplingEnabled (bool on) { oversamplingEnabled = on; }
void VocalForgeAudioProcessor::setPitchBypass (bool b)          { pitchBypass = b; }
void VocalForgeAudioProcessor::setPitchRetuneSpeed (float ms)
{
    currentPitchSpeedMs = ms;
    float speedSec = juce::jmax (0.001f, ms * 0.001f);
    pitchSmoothCoeff = 1.f - std::exp (-1.f / (speedSec * (float)currentSampleRate));
}
void VocalForgeAudioProcessor::setPitchHumanize (bool on)       { pitchHumanize = on; }
void VocalForgeAudioProcessor::setDelayDepth (float wet)        { delayWet = wet; }
void VocalForgeAudioProcessor::setReverbDepth (float wet)
{
    reverbParams.wetLevel = wet;
    reverbParams.dryLevel = 1.f - wet;
    reverb.setParameters (reverbParams);
}
void VocalForgeAudioProcessor::setSatDrive (float d)            { satDrive = d; }
void VocalForgeAudioProcessor::setSatMix (float m)              { satMix = m; }
void VocalForgeAudioProcessor::setChorusMix (float m)           { chMix = m; }
