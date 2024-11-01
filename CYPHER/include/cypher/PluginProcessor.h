#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "cypher/exprtk.hpp"
#include "JuceHeader.h"
#include <random>

namespace audio_plugin {

    static bool has_equation_been_set = false;

    class SineWaveSound : public SynthesiserSound
    {
    public:
        SineWaveSound() {}

        bool appliesToNote(int /*midiNoteNumber*/) override { return true; }
        bool appliesToChannel(int /*midiChannel*/) override { return true; }
    };


    class EquationEditor : public TextEditor
    {
    public:
        static constexpr auto kDefaultJustification{ juce::Justification::topLeft };

        using TextEditor::TextEditor;

        double _t = 0;
        std::string defaultEquation = "sin(x)";

        void setBBText(const juce::String& bbText) {

            initialise();
            addEquationEditorText("ss");
        };

        typedef exprtk::symbol_table<double> symbol_table_t;
        typedef exprtk::expression<double>   expression_t;
        typedef exprtk::parser<double>       parser_t;
        symbol_table_t symbol_table;
        expression_t expression;
        parser_t parser;
        double delta = 0;
        std::string text;

        bool update_expression(std::string expression_string) {
            expression.register_symbol_table(symbol_table);
            symbol_table.add_variable("x", delta);

            if (parser.compile(expression_string, expression)) {
                return true; //todo error check?
            }
            return false;
        };

        EquationEditor() {
        };


        //template <typename T>
        double trig_function(double _delta) {
            delta = _delta;
            return expression.value();
        };



    private:
        bool                             m_listPrefix{ false };
        bool                             m_quotePrefix{ false };
        juce::Justification              m_justification{ kDefaultJustification };

        void initialise() {
            setJustification(kDefaultJustification);
            m_listPrefix = false;
            m_quotePrefix = false;
            clear();
        };
        void addEquationEditorText(const juce::String& text, const juce::String& value = {}) {

            if (text.isEmpty())
                return;
            setColour(juce::TextEditor::textColourId, juce::Colour::fromString("blue"));
            moveCaretToEnd();
            insertTextAtCaret(text);
            m_listPrefix = false;
            m_quotePrefix = false;
        };

        void parseJustification(const juce::String& justification, bool enable) {
            m_justification = kDefaultJustification;
            setJustification(m_justification);
        };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EquationEditor)
    };

    enum synth_type {
        saw,
        sine,
        sqr,
        custom

    };

    class value_holder {
    public:

        double custom_expression_result = 0;
        EquationEditor* eq;
        value_holder() {
            eq = new EquationEditor();
            eq->update_expression(eq->defaultEquation);



        };


        double compile(juce::String string, float delta)
        {
            return eq->trig_function(delta);
        };

        ~value_holder() {
            assert(eq);
            delete eq;
        }
    };



    class SineWaveVoice : public SynthesiserVoice
    {
    public:
        SineWaveVoice(value_holder* custom_value_holder_ref) :custom_value_holder(custom_value_holder_ref) {}

        synth_type type = synth_type::custom;
        value_holder* custom_value_holder;

        double custom_equation_value = 0;
        std::string alog_updated = "sin(x)";

        bool canPlaySound(SynthesiserSound* sound) override
        {
            return dynamic_cast<SineWaveSound*> (sound) != nullptr;
        }

        void startNote(int midiNoteNumber, float velocity,
            SynthesiserSound* /*sound*/,
            int /*currentPitchWheelPosition*/) override
        {
            currentAngle = 0.0;
            level = velocity * 0.15;
            tailOff = 0.0;

            //get text from custom input
            auto algo = custom_value_holder->eq->text;

            if (alog_updated != algo) {
                alog_updated = custom_value_holder->eq->text;

                custom_value_holder->eq->update_expression(alog_updated);
            }

            auto cyclesPerSecond = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
            auto cyclesPerSample = cyclesPerSecond / getSampleRate();
            angleDelta = cyclesPerSample * MathConstants<double>::twoPi;
        }

        void stopNote(float /*velocity*/, bool allowTailOff) override
        {
            if (allowTailOff)
            {

                if (tailOff == 0.0)
                    tailOff = 1.0;
            }
            else
            {
                clearCurrentNote();
                angleDelta = 0.0;
            }
        }



        void pitchWheelMoved(int /*newValue*/) override
        {
        }

        void controllerMoved(int /*controllerNumber*/, int /*newValue*/) override
        {
        }

        double square_wave(double currentAngle, double duty_cycle = 0.5) {

            double decision = 0.0;
            double angle_mod = fmod(currentAngle, 2.0 * MathConstants<double>::pi);

            if (angle_mod < (2 * MathConstants<double>::pi * duty_cycle)) {
                decision = 1.0;
            }
            else {
                decision = -1.0;
            }
            return decision;
        }


        void renderNextBlock(AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
        {
            double waveform = 0;

            auto sawtooth_wave = (2 / MathConstants<double>::pi) * (currentAngle - MathConstants<double>::pi / 2 - MathConstants<double>::pi * ceil((currentAngle + MathConstants<double>::pi / 2) / MathConstants<double>::pi));
            auto sinewave = sin(currentAngle);

            if (angleDelta != 0.0)
            {
                if (tailOff > 0.0)
                {
                    while (--numSamples >= 0)
                    {
                        waveform = custom_value_holder->compile(alog_updated, currentAngle);
                        auto currentSample = (float)(waveform * level * tailOff);
                        for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                            outputBuffer.addSample(i, startSample, currentSample);

                        currentAngle += angleDelta;
                        ++startSample;

                        //check tail off
                        tailOff *= 0.99;
                        if (tailOff <= 0.005)
                        {
                            clearCurrentNote();
                            angleDelta = 0.0;
                            break;
                        }
                    }
                }
                else
                {
                    while (--numSamples >= 0)
                    {
                        waveform = custom_value_holder->compile(alog_updated, currentAngle);
                        auto currentSample = (float)(waveform * level);
                        for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                            outputBuffer.addSample(i, startSample, currentSample);

                        currentAngle += angleDelta;
                        ++startSample;
                    }
                }
            }
        }

        using SynthesiserVoice::renderNextBlock;

    private:
        double currentAngle = 0.0;
        double angleDelta = 0.0;
        double level = 0.0;
        double tailOff = 0.0;
    };



class AudioPluginAudioProcessor : public juce::AudioProcessor {
public:

  // Our plug-in's current state
  juce::AudioProcessorValueTreeState state;
  value_holder* value_holder_instance;
  value_holder  value_holder_instance_ref;
  Synthesiser synth;
  MidiKeyboardState keyboardState;
  int delayPosition = 0;
  AudioBuffer<float> delayBufferFloat;
  AudioBuffer<double> delayBufferDouble;
  CriticalSection trackPropertiesLock;
  TrackProperties trackProperties;




  AudioPluginAudioProcessor();
  ~AudioPluginAudioProcessor() override;


  template <typename FloatType>
  void applyDelay(AudioBuffer<FloatType>& buffer, AudioBuffer<FloatType>& delayBuffer, float delayLevel)
  {
      std::random_device rd;
      std::mt19937 gen(rd());
      std::uniform_real_distribution<> dis(0.0, 0.5);

      // Generate a random number between 0.0 and 1.0
      double random_number = dis(gen);
      auto numSamples = buffer.getNumSamples();
      auto delayPos = 0;

      for (auto channel = 0; channel < getTotalNumOutputChannels(); ++channel)
      {
          auto channelData = buffer.getWritePointer(channel);
          auto delayData = delayBuffer.getWritePointer(jmin(channel, delayBuffer.getNumChannels() - 1));
          delayPos = delayPosition;

          for (auto i = 0; i < numSamples; ++i)
          {
              auto in = channelData[i];
              channelData[i] += delayData[delayPos];
              delayData[delayPos] = (delayData[delayPos] + in) * delayLevel;

              if (++delayPos >= delayBuffer.getNumSamples())
                  delayPos = 0;
          }
      }

      delayPosition = delayPos;
  }


  template <typename FloatType>
  void applyGain(AudioBuffer<FloatType>& buffer, AudioBuffer<FloatType>& delayBuffer, float gainLevel)
  {
      ignoreUnused(delayBuffer);

      for (auto channel = 0; channel < getTotalNumOutputChannels(); ++channel)
          buffer.applyGain(channel, 0, buffer.getNumSamples(), gainLevel);
  }


  template <typename FloatType>
  void process(AudioBuffer<FloatType>& buffer, MidiBuffer& midiMessages, AudioBuffer<FloatType>& delayBuffer)
  {

      auto gainParamValue = state.getParameter("gain")->getValue();
      auto delayParamValue = state.getParameter("delay")->getValue();
      auto numSamples = buffer.getNumSamples();
      for (auto i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
          buffer.clear(i, 0, numSamples);
      keyboardState.processNextMidiBuffer(midiMessages, 0, numSamples, true);
      synth.renderNextBlock(buffer, midiMessages, 0, numSamples);
      applyDelay(buffer, delayBuffer, delayParamValue);
      applyGain(buffer, delayBuffer, gainParamValue);

  }


  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

  static BusesProperties getBusesProperties()
  {
      return BusesProperties().withInput("Input", AudioChannelSet::stereo(), true)
          .withOutput("Output", AudioChannelSet::stereo(), true);
  }

  void initialiseSynth(value_holder* holder);

  bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

  void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

  juce::AudioProcessorEditor* createEditor() override;
  bool hasEditor() const override;

  const juce::String getName() const override;

  TrackProperties getTrackProperties() const
  {
      const ScopedLock sl(trackPropertiesLock);
      return trackProperties;
  }

  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  void reset() override;

  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int index, const juce::String& newName) override;

  void getStateInformation(juce::MemoryBlock& destData) override;
  void setStateInformation(const void* data, int sizeInBytes) override;



private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessor)
};
}  // namespace audio_plugin
