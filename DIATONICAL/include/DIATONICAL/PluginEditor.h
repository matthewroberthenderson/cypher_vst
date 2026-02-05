#pragma once
#include "PluginProcessor.h"

class FancyLook : public juce::LookAndFeel_V4 {
public:
    void drawComboBox (juce::Graphics& g, int w, int h, bool, int, int, int, int, juce::ComboBox&) override {
        g.setColour(juce::Colours::cyan.withAlpha(0.1f));
        g.fillRoundedRectangle(0, 0, w, h, 4.0f);
        g.setColour(juce::Colours::cyan);
        g.drawRoundedRectangle(0, 0, w, h, 4.0f, 1.0f);
    }
};

class RandomChordAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
    RandomChordAudioProcessorEditor (RandomChordAudioProcessor& p) 
        : AudioProcessorEditor (&p), processor (p) 
    {
        setLookAndFeel(&fancyLook);

        rootSelector.addItemList({"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"}, 1);
        rootSelector.setSelectedItemIndex(processor.currentRoot, juce::dontSendNotification);
        rootSelector.onChange = [this] { 
            processor.currentRoot = rootSelector.getSelectedItemIndex(); 
            processor.generateStaticMap(); 
        };
        addAndMakeVisible(rootSelector);

        scaleSelector.addItemList({"Major", "Minor"}, 1);
        scaleSelector.setSelectedItemIndex(processor.currentScale, juce::dontSendNotification);
        scaleSelector.onChange = [this] { 
            processor.currentScale = scaleSelector.getSelectedItemIndex(); 
            processor.generateStaticMap(); 
        };
        addAndMakeVisible(scaleSelector);

        bachToggle.setButtonText("BACH MODE");
        bachToggle.setToggleState(processor.bachMode, juce::dontSendNotification);
        bachToggle.onClick = [this] { 
            processor.bachMode = bachToggle.getToggleState(); 
            processor.generateStaticMap(); 
        };
        addAndMakeVisible(bachToggle);

        setSize (300, 300);
    }

    ~RandomChordAudioProcessorEditor() { setLookAndFeel(nullptr); }

    void paint (juce::Graphics& g) override {
        juce::ColourGradient grad(juce::Colour(0xff121212), 0, 0, juce::Colour(0xff000000), 0, (float)getHeight(), false);
        g.setGradientFill(grad); 
        g.fillAll();
        
        g.setColour(juce::Colours::cyan); 
        g.setFont(24.0f);
        g.drawText("DIATONICAL", 0, 30, getWidth(), 30, juce::Justification::centred);

        g.setFont(14.0f);
        g.drawText("Root Key", 50, 75, 200, 20, juce::Justification::left);
        g.drawText("Scale", 50, 135, 200, 20, juce::Justification::left);
    }

    void resized() override {
        rootSelector.setBounds(50, 95, 200, 35);
        scaleSelector.setBounds(50, 155, 200, 35);
        bachToggle.setBounds(50, 210, 200, 40);
    }

private:
    FancyLook fancyLook;
    juce::ComboBox rootSelector, scaleSelector;
    juce::ToggleButton bachToggle;
    RandomChordAudioProcessor& processor;
};