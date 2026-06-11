#pragma once
#include <JuceHeader.h>

struct VocalPreset
{
    juce::String name;
    juce::String category;
    juce::Colour colour;

    float eqLowCutHz    = 90.f;
    float eqLowBellHz   = 400.f;
    float eqLowBellGain = 0.f;
    float eqLowBellQ    = 0.9f;
    float eqAirHz       = 10000.f;
    float eqAirGain     = 2.f;

    float compThresholdDb = -18.f;
    float compRatio       = 3.f;
    float compAttackMs    = 10.f;
    float compReleaseMs   = 80.f;
    float compMakeupGain  = 0.f;

    float deEssLowThreshDb  = -20.f;
    float deEssHighThreshDb = -18.f;

    int   satType = 0;
    float satDrive  = 0.f;
    float satMix    = 0.5f;

    float pitchSpeedMs = 50.f;
    float pitchAmount  = 0.f;

    float reverbRoomSize  = 0.3f;
    float reverbDamping   = 0.5f;
    float reverbWet       = 0.15f;
    float reverbWidth     = 0.8f;

    float delayTimeMs    = 250.f;
    float delayFeedback  = 0.25f;
    float delayWet       = 0.0f;
    int   delayDivision  = 1;
    bool  delayPingPong  = false;

    bool  reverbDuckEnabled = false;

    float chorusRate     = 1.5f;
    float chorusDepth    = 0.1f;
    float chorusMix      = 0.0f;
    int   chorusMode     = 0;
};

class VocalForgeAudioProcessor  : public juce::AudioProcessor
{
public:
    VocalForgeAudioProcessor();
    ~VocalForgeAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    int    calcLatencySamples()  const;

    void setOversamplingEnabled (bool enabled);

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    const juce::Array<VocalPreset>& getPresets()  const { return presets; }
    int   getCurrentPresetIndex()                  const { return currentPresetIndex; }
    void  loadPreset (int index);

    float getOutputLevelDb()     const { return outputLevelDb.load(); }
    float getInputLevelDb()      const { return inputLevelDb.load(); }
    float getGainReductionDb()   const { return gainReductionDb.load(); }

    void setModuleEnabled (int moduleIdx, bool enabled);

    void setEqLowBellGain (float gainDb);
    void setCompThreshold (float threshDb);
    void setDeEsserThresh (float threshDb);
    void setSatDrive      (float drive01);
    void setDelayWet      (float wet01);
    void setDelayDepth    (float depth01);
    void setDelayDivision (int div);
    void setDelayPingPong (bool pingPong);
    void setChorusMix     (float mix01);
    void setReverbWet     (float wet01);

    float getCurrentBpm() const { return currentBpm.load(); }

    void setPitchKey   (int key)   { pitchKey.store   (juce::jlimit (0, 11, key)); }
    int  getPitchKey   ()  const   { return pitchKey.load(); }
    void setPitchScale (int scale) { pitchScale.store (juce::jlimit (0,  2, scale)); }
    int  getPitchScale ()  const   { return pitchScale.load(); }

    void setPitchBypass      (bool bypass);
    void setPitchRetuneSpeed (float speedMs);
    void setPitchHumanize    (float amount);
    bool  getPitchBypass     () const { return pitchBypass.load(); }
    float getPitchSpeedMs    () const { return currentPitchSpeedMs; }
    float getPitchHumanize   () const { return pitchHumanize.load(); }

    void setMasterBypass (bool bypass) { masterBypass.store (bypass); }
    bool getMasterBypass () const      { return masterBypass.load(); }

private:
    void initPresets();
    void applyPresetToDSP (const VocalPreset& p);

    juce::Array<VocalPreset> presets;
    int currentPresetIndex = 0;

    double currentSampleRate  = 44100.0;
    int    currentBlockSize   = 512;

    juce::dsp::ProcessorChain<
        juce::dsp::IIR::Filter<float>,
        juce::dsp::IIR::Filter<float>,
        juce::dsp::IIR::Filter<float>
    > eqChain;

    float optoFastEnv   = 0.f;
    float optoSlowEnv   = 0.f;
    float optoAttCoeff  = 0.f;
    float optoFRelCoeff = 0.f;
    float optoSRelCoeff = 0.f;

    float compThreshDb  = -18.f;
    float compRatio     =   3.f;
    static constexpr float kCompKneeDb = 6.f;
    float compMakeupDb  =   0.f;

    juce::dsp::Reverb reverb;
    juce::dsp::Reverb::Parameters reverbParams;

    juce::dsp::IIR::Filter<float> reverbMidEQL;
    juce::dsp::IIR::Filter<float> reverbMidEQR;

    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear>
        chDelL { 8192 }, chDelR { 8192 };
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear>
        chDelL2 { 8192 }, chDelR2 { 8192 };

    float chLfoPhL  = 0.f;
    float chLfoPhR  = 0.f;
    float chLfoPhL2 = 0.f;
    float chLfoPhR2 = 0.f;

    float chRate     = 1.f;
    float chDepthSmp = 0.f;
    float chMix      = 0.f;
    int   chMode     = 0;

    static constexpr float kChorusCentreMs = 12.f;

    juce::dsp::Gain<float> outputGain;

    juce::dsp::Oversampling<float> oversampler { 2, 1,
        juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR };
    bool oversamplingEnabled = false;

    std::atomic<float> outputLevelDb   { -100.f };
    std::atomic<float> inputLevelDb    { -100.f };
    std::atomic<float> gainReductionDb {    0.f };

    bool moduleBypass[7] { false, false, false, false, false, false, false };

    int   satType  = 0;
    float satDrive = 0.f;
    float satMix   = 0.5f;

    static float satTube        (float x, float drive);
    static float satTape        (float x, float drive);
    static float satTransistor  (float x, float drive);

public:
    int  getSatType() const { return satType; }
private:

    static constexpr float kDeEssCrossover = 4000.f;

    juce::dsp::IIR::Filter<float> deEssLowDetect;
    juce::dsp::IIR::Filter<float> deEssHighDetect;
    juce::dsp::IIR::Filter<float> deEssHighLP;

    juce::dsp::IIR::Filter<float> deEssLowSplit[2];

    float deEssLowEnv   = 0.f;
    float deEssHighEnv  = 0.f;
    float deEssAttCoeff = 0.f;
    float deEssRelCoeff = 0.f;

    float deEssLowThreshLin  = 1.f;
    float deEssHighThreshLin = 1.f;

    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLineL { 192000 };
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLineR { 192000 };
    float delayFeedback  = 0.25f;
    float delayWet       = 0.f;
    int   delaySamples   = 0;
    int   delayDivision  = 1;
    bool  delayPingPong  = false;

    float delayFbLP_L   = 0.f;
    float delayFbLP_R   = 0.f;
    float delayFbLpAlpha = 0.5f;

    float duckEnv      = 0.f;
    float duckAttCoeff = 0.f;
    float duckRelCoeff = 0.f;

    float reverbDuckEnv      = 0.f;
    float reverbDuckAttCoeff = 0.f;
    float reverbDuckRelCoeff = 0.f;
    bool  reverbDuckEnabled  = false;

    std::atomic<float> currentBpm { 120.f };

    static constexpr int kPitchDetN   = 1024;
    static constexpr int kPitchUpdSmp =   64;
    float pitchDetBuf[kPitchDetN] = {};
    int   pitchDetBufIdx  = 0;
    int   pitchDetTimer   = 0;
    float pitchDetFreq    = -1.f;
    float pitchRatioTarget = 1.f;
    float pitchRatioSmooth = 1.f;
    float pitchSmoothCoeff = 0.f;

    static constexpr int kGrainBufSz = 8192;
    static constexpr int kGrainSize  =  256;
    float grainBufL[kGrainBufSz] = {};
    float grainBufR[kGrainBufSz] = {};
    int   grainWrite  = 0;
    float grainRead1  = 0.f;
    float grainRead2  = 0.f;
    int   grainPhase1 = 0;
    int   grainPhase2 = 0;

    std::atomic<int>  pitchKey     { 0 };
    std::atomic<int>  pitchScale   { 0 };

    std::atomic<bool>  pitchBypass   { false };
    std::atomic<float> pitchHumanize { 0.f };
    float pitchHumanizeLfoPhase = 0.f;
    float currentPitchSpeedMs   = 50.f;

    float       runPitchDetect() const;
    static float noteToHz       (int midiNote);
    static float nearestNoteHz  (float hz, int key, int scale);

    std::atomic<bool> masterBypass { false };

    juce::FileLogger* logger = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VocalForgeAudioProcessor)
};
