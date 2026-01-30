#pragma once
#include "DIATONICAL/PluginProcessor.h"

class View : public juce::LookAndFeel_V4 {
public:
    void drawComboBox (juce::Graphics& g, int width, int height, bool, int, int, int, int, juce::ComboBox& box) override {
        g.setColour(juce::Colours::cyan.withAlpha(0.2f));
        g.fillRoundedRectangle(0, 0, width, height, 5.0f);
        g.setColour(juce::Colours::cyan);
        g.drawRoundedRectangle(0, 0, width, height, 5.0f, 1.0f);
    }
};

class RandomChordAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::ComboBox::Listener {
public:
    RandomChordAudioProcessorEditor (RandomChordAudioProcessor& p) : AudioProcessorEditor (&p), processor (p) {
        setLookAndFeel(&View);

        // Key Selector
        juce::StringArray keys = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
        rootSelector.addItemList(keys, 1);
        rootSelector.setSelectedId(1);
        rootSelector.addListener(this);
        addAndMakeVisible(rootSelector);

        // Scale Selector
        scaleSelector.addItem("Major", 1);
        scaleSelector.addItem("Minor", 2);
        scaleSelector.setSelectedId(1);
        scaleSelector.addListener(this);
        addAndMakeVisible(scaleSelector);

        setSize (300, 250);
    }

    ~RandomChordAudioProcessorEditor() { setLookAndFeel(nullptr); }

    void comboBoxChanged (juce::ComboBox* box) override {
        if (box == &rootSelector) processor.currentRoot = rootSelector.getSelectedItemIndex();
        if (box == &scaleSelector) processor.currentScale = scaleSelector.getSelectedItemIndex();
    }

    void paint (juce::Graphics& g) override {
        juce::ColourGradient grad(juce::Colour(0xff0f0c29), 0, 0, juce::Colour(0xff000000), 0, (float)getHeight(), false);
        g.setGradientFill(grad);
        g.fillAll();
        
        g.setColour(juce::Colours::cyan);
        g.setFont(20.0f);
        g.drawText("DIATONICAL", 0, 20, getWidth(), 30, juce::Justification::centred);
        
        g.setFont(12.0f);
        g.drawText("ROOT", 50, 70, 200, 20, juce::Justification::left);
        g.drawText("SCALE", 50, 130, 200, 20, juce::Justification::left);
    }

    void resized() override {
        rootSelector.setBounds(50, 90, 200, 30);
        scaleSelector.setBounds(50, 150, 200, 30);
    }

private:
    View View;
    juce::ComboBox rootSelector, scaleSelector;
    RandomChordAudioProcessor& processor;
};