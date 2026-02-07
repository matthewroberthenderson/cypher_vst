#pragma once
#include <JuceHeader.h>
#include <vector>
#include <map>

class ChordAudioProcessor : public juce::AudioProcessor
{
public:
    ChordAudioProcessor();
    ~ChordAudioProcessor() override;

    // For settings that persist between sessions (ableton/logic)
    // New versions of Juce docs want a AudioProcessorValueTreeState
    // The DAW should ask us for a "chunk" of data; on save
    // when we re-open the project, it hands it back over.
    void getStateInformation(juce::MemoryBlock &destData) override;
    void setStateInformation(const void *data, int sizeInBytes) override;

    void prepareToPlay(double, int) override {}
    void releaseResources() override {}
    void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

    juce::AudioProcessorEditor *createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "DIATONICAL"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return true; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String &) override {}

    // Parameters
    int currentRoot = 0;
    int currentScale = 0;
    bool bachMode = false;

    void generateStaticMap();

private:
    juce::ValueTree tree{"DIATONICAL_SETTINGS"};
    std::map<int, std::vector<int>> noteToChordMap;
    std::map<int, std::vector<int>> activeNotes;

    const std::vector<std::vector<int>> scaleLibrary = {
        {0, 2, 4, 5, 7, 9, 11}, // Major
        {0, 2, 3, 5, 7, 8, 10}  // Minor
    };

    std::vector<int> generateBachVoicing(int degree, const std::vector<int> &scale);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChordAudioProcessor)
};