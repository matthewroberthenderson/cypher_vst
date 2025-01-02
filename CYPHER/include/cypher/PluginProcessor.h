#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include "cypher/Cypher.h"

#include "JuceHeader.h"
#include <random>


    static bool has_equation_been_set = false;

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
            // editor;
            //AudioPluginAudioProcessorEditor* editor_ref;

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


