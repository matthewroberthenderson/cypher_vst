#include "cypher/PluginProcessor.h"
#include "cypher/PluginEditor.h"

AudioPluginAudioProcessor::AudioPluginAudioProcessor()
    : AudioProcessor(getBusesProperties()),
    state(*this, nullptr, "state",
        { std::make_unique<AudioParameterFloat>(ParameterID { "gain",  1 }, "Gain",           NormalisableRange<float>(0.0f, 1.0f), 0.9f),
          std::make_unique<AudioParameterFloat>(ParameterID { "delay", 1 }, "Delay Feedback", NormalisableRange<float>(0.0f, 1.0f), 0.5f)

        })
{
    value_holder_instance = new value_holder();
    state.state.addChild({ "uiState", { { "width",  400 }, { "height", 200 } }, {} }, -1, nullptr);
    initialiseSynth(value_holder_instance);

}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor() {
    assert(value_holder_instance);
    delete value_holder_instance;
}

const juce::String AudioPluginAudioProcessor::getName() const {
  return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool AudioPluginAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const {
  return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms() {
  return 1;
}

int AudioPluginAudioProcessor::getCurrentProgram() {
  return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram(int index) {
  juce::ignoreUnused(index);
}

const juce::String AudioPluginAudioProcessor::getProgramName(int index) {
  juce::ignoreUnused(index);
  return {};
}

void AudioPluginAudioProcessor::changeProgramName(int index,
                                                  const juce::String& newName) {
  juce::ignoreUnused(index, newName);
}

void AudioPluginAudioProcessor::prepareToPlay(double sampleRate,
                                              int samplesPerBlock) {
    synth.setCurrentPlaybackSampleRate(sampleRate);
    keyboardState.reset();


    if (isUsingDoublePrecision())
    {
        delayBufferDouble.setSize(2, 12000);
        delayBufferFloat.setSize(1, 1);
    }
    else
    {
        delayBufferFloat.setSize(2, 12000);
        delayBufferDouble.setSize(1, 1);
    }

    reset();
}

void AudioPluginAudioProcessor::releaseResources() {
    keyboardState.reset();
}

void AudioPluginAudioProcessor::reset()
{
    delayBufferFloat.clear();
    delayBufferDouble.clear();
}

inline void AudioPluginAudioProcessor::initialiseSynth(value_holder* holder)
{
    auto numVoices = 8;
    for (auto i = 0; i < numVoices; ++i)
        synth.addVoice(new SineWaveVoice(holder));
    synth.addSound(new SineWaveSound());
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported(
    const BusesLayout& layouts) const {
    const auto& mainOutput = layouts.getMainOutputChannelSet();
    const auto& mainInput = layouts.getMainInputChannelSet();
    if (!mainInput.isDisabled() && mainInput != mainOutput)
        return false;

    if (mainOutput.size() > 2)
        return false;

    return true;
}


bool AudioPluginAudioProcessor::hasEditor() const {
  return true;
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor() {
    auto n = new AudioPluginAudioProcessorEditor(*this);
    n->value_holder_instance_ref = this->value_holder_instance;
    return  n;

}

void AudioPluginAudioProcessor::getStateInformation(
    juce::MemoryBlock& destData) {
  juce::ignoreUnused(destData);
}


void AudioPluginAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    jassert(!isUsingDoublePrecision());
    process(buffer, midiMessages, delayBufferFloat);
}

void AudioPluginAudioProcessor::setStateInformation(const void* data,
                                                    int sizeInBytes) {

  juce::ignoreUnused(data, sizeInBytes);
}


juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new AudioPluginAudioProcessor();
}