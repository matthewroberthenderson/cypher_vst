#pragma once
#include <JuceHeader.h>
#include <vector>
#include <map>

class RandomChordAudioProcessor : public juce::AudioProcessor {
public:
    RandomChordAudioProcessor();
    ~RandomChordAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override {}
    void releaseResources() override {}
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "DIATONICAL"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return true; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int index) override {}
    const juce::String getProgramName (int index) override { return {}; }
    void changeProgramName (int index, const juce::String& newName) override {}
    void getStateInformation (juce::MemoryBlock& destData) override {}
    void setStateInformation (const void* data, int sizeInBytes) override {}

    // Public parameters for the UI to change
    int currentRoot = 0;   // 0=C, 1=C#, etc.
    int currentScale = 0;  // 0=Major, 1=Minor

private:
    std::map<int, std::vector<int>> activeNotes;
    
    // Scale intervals: Major {W,W,H,W,W,W,H}, Minor {W,H,W,W,H,W,W}
    const std::vector<std::vector<int>> scaleLibrary = {
        {0, 2, 4, 5, 7, 9, 11}, // Major
        {0, 2, 3, 5, 7, 8, 10}  // Natural Minor
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RandomChordAudioProcessor)
};