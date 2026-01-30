#pragma once
#include "DIATONICAL/PluginProcessor.h"

class View : public juce::LookAndFeel_V4 {
public:
    void drawButtonBackground (juce::Graphics& g, juce::Button& b, const juce::Colour& backgroundColour,
                               bool isMouseOverButton, bool isButtonDown) override {
        auto bounds = b.getLocalBounds().toFloat().reduced(2);
        auto color = isButtonDown ? juce::Colours::cyan : (isMouseOverButton ? juce::Colours::white : juce::Colours::cyan.withAlpha(0.5f));
        
        g.setColour(color.withAlpha(0.1f));
        g.fillRoundedRectangle(bounds, 8.0f);
        g.setColour(color);
        g.drawRoundedRectangle(bounds, 8.0f, 2.0f); // Neon border
    }
};

class RandomChordAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
    RandomChordAudioProcessorEditor (RandomChordAudioProcessor& p) : AudioProcessorEditor (&p), processor (p) {
        setLookAndFeel(&View);
        addAndMakeVisible(generateBtn);
        generateBtn.setButtonText("RANDOMIZE CHORDS");
        setSize (400, 300);
    }
    
    ~RandomChordAudioProcessorEditor() { setLookAndFeel(nullptr); }

    void paint (juce::Graphics& g) override {
        juce::ColourGradient grad(juce::Colour(0xff0f0c29), 0, 0, juce::Colour(0xff24243e), 0, getHeight(), false);
        g.setGradientFill(grad);
        g.fillAll();

        g.setColour(juce::Colours::cyan.withAlpha(0.7f));
        g.setFont(24.0f);
        g.drawText("CHORD GEN", getLocalBounds().removeFromTop(50), juce::Justification::centred);
    }

    void resized() override {
        generateBtn.setBounds(100, 120, 200, 50);
    }

private:
    View View;
    juce::TextButton generateBtn;
    RandomChordAudioProcessor& processor;
};