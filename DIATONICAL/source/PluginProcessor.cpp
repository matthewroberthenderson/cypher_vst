#include "DIATONICAL/PluginProcessor.h"
#include "DIATONICAL/PluginEditor.h"


RandomChordAudioProcessor::RandomChordAudioProcessor()
    : AudioProcessor (BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    generateStaticMap();
}

RandomChordAudioProcessor::~RandomChordAudioProcessor() {}

void RandomChordAudioProcessor::generateStaticMap() {
    noteToChordMap.clear();
    const auto& scale = scaleLibrary[currentScale];
    
    for (int i = 0; i < 128; ++i) {
        int degree = i % 7; 
        std::vector<int> chord;

        if (bachMode) {
            chord = generateBachVoicing(degree, scale);
        } else {
            for (int j : {0, 2, 4}) {
                int idx = (degree + j) % 7;
                int octWrap = ((degree + j) / 7) * 12;
                chord.push_back(48 + currentRoot + scale[idx] + octWrap);
            }
        }
        noteToChordMap[i] = chord;
    }
}

std::vector<int> RandomChordAudioProcessor::generateBachVoicing(int degree, const std::vector<int>& scale) {
    std::vector<int> notes;
    int rootBase = 48 + currentRoot + scale[degree];
    notes.push_back(rootBase - 12);                         
    notes.push_back(rootBase + scale[(degree + 4) % 7]);    
    notes.push_back(rootBase + 12);                         
    notes.push_back(rootBase + 12 + scale[(degree + 2) % 7]); 
    return notes;
}

void RandomChordAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    buffer.clear();
    juce::MidiBuffer processedMidi;

    for (const auto metadata : midiMessages) {
        auto msg = metadata.getMessage();
        int triggerNote = msg.getNoteNumber();

        if (msg.isNoteOn()) {
            if (noteToChordMap.count(triggerNote)) {
                for (int n : noteToChordMap[triggerNote])
                    processedMidi.addEvent(juce::MidiMessage::noteOn(msg.getChannel(), n, msg.getVelocity()), metadata.samplePosition);
                activeNotes[triggerNote] = noteToChordMap[triggerNote];
            }
        } 
        else if (msg.isNoteOff()) {
            if (activeNotes.count(triggerNote)) {
                for (int n : activeNotes[triggerNote])
                    processedMidi.addEvent(juce::MidiMessage::noteOff(msg.getChannel(), n, msg.getVelocity()), metadata.samplePosition);
                activeNotes.erase(triggerNote);
            }
        }
    }
    midiMessages.swapWith(processedMidi);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new RandomChordAudioProcessor(); }
juce::AudioProcessorEditor* RandomChordAudioProcessor::createEditor() { return new RandomChordAudioProcessorEditor (*this); }