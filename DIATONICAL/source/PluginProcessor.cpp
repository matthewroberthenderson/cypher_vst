#include "DIATONICAL/PluginProcessor.h"
#include "DIATONICAL/PluginEditor.h"

RandomChordAudioProcessor::RandomChordAudioProcessor()
    : AudioProcessor (BusesProperties()
                      .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                      .withInput  ("Input",  juce::AudioChannelSet::stereo(), false))
{}

RandomChordAudioProcessor::~RandomChordAudioProcessor() {}

void RandomChordAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    buffer.clear();
    juce::MidiBuffer processedMidi;

    for (const auto metadata : midiMessages)
    {
        auto msg = metadata.getMessage();
        int samplePos = metadata.samplePosition;

        if (msg.isNoteOn())
        {
            int triggerNote = msg.getNoteNumber();
            const auto& scale = scaleLibrary[currentScale];
            
            // Pick random Diatonic Degree (0 to 6 is fine for now)
            int degree = juce::Random::getSystemRandom().nextInt(7);
            
            // Build the Triad - Root, 3rd, 5th of the above degree
            std::vector<int> chordNotes;
            for (int i : {0, 2, 4}) 
            {
                int scaleIndex = (degree + i) % 7;
                int octaveOffset = ((degree + i) / 7) * 12;
                
                // we do the: Base Octave + Root Key + Scale Step + Octave Wrap - method
                int finalNote = 48 + currentRoot + scale[scaleIndex] + octaveOffset;
                chordNotes.push_back(finalNote);
                
                processedMidi.addEvent(juce::MidiMessage::noteOn(msg.getChannel(), finalNote, msg.getVelocity()), samplePos);
            }
            activeNotes[triggerNote] = chordNotes;
        }
        else if (msg.isNoteOff())
        {
            int triggerNote = msg.getNoteNumber();
            if (activeNotes.count(triggerNote))
            {
                for (int noteToOff : activeNotes[triggerNote])
                {
                    processedMidi.addEvent(juce::MidiMessage::noteOff(msg.getChannel(), noteToOff, msg.getVelocity()), samplePos);
                }
                activeNotes.erase(triggerNote);
            }
        }
    }
    midiMessages.swapWith(processedMidi);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new RandomChordAudioProcessor(); }
juce::AudioProcessorEditor* RandomChordAudioProcessor::createEditor() { return new RandomChordAudioProcessorEditor (*this); }