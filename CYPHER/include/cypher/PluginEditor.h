#ifndef PLUGIN_EDITOR_H
#define PLUGIN_EDITOR_H

#include "PluginProcessor.h"
#include "cypher/Cypher.h"
#include "JuceHeader.h"

class AudioPluginAudioProcessorEditor : public juce::AudioProcessorEditor,
    private juce::Timer,
    private juce::Value::Listener,
    private CodeDocument::Listener {

public:
  explicit AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor&);
  ~AudioPluginAudioProcessorEditor() override;

  std::unique_ptr<CypherRenderer> openGLComponent;

  void paint(juce::Graphics&) override;
  void resized() override;

  MidiKeyboardComponent midiKeyboard;

  Label gainLabel{ {}, "Throughput level:" },
      delayLabel{ {}, "Delay:" },
      EquationEditorLabel{ {}, "Equation Editor" };

  Slider gainSlider, delaySlider;

  //plugin editor has the code documents, that's all we need
  CodeDocument fragmentDocument, synthDocument;
  CodeEditorComponent fragmentEditorComp{ fragmentDocument, nullptr },
        synthEditorComp{ synthDocument, nullptr };
    TabbedComponent tabbedComp{ TabbedButtonBar::TabsAtLeft };

  void valueChanged(Value&) override
  {
      setSize(lastUIWidth.getValue(), lastUIHeight.getValue());
  }

  AudioProcessorValueTreeState::SliderAttachment gainAttachment, delayAttachment;

  Colour backgroundColour = Colour();
  Value lastUIWidth, lastUIHeight;

  double custom_equation_value = 0;
  EquationEditor equation_editor;
  double math_value;
  value_holder* value_holder_instance_ref;
  Label statusLabel;


    void codeDocumentTextInserted (const String& newText, int insertIndex) override{

    };

    /** Called by a CodeDocument when text is deleted. */
    void codeDocumentTextDeleted (int startIndex, int endIndex) override{

    };
    
  void timerCallback() override;
  void hostMIDIControllerIsAvailable(bool controllerIsAvailable) override
  {
      midiKeyboard.setVisible(!controllerIsAvailable);
  }

  int getControlParameterIndex(Component& control) override
  {
      if (&control == &gainSlider)
          return 0;

      if (&control == &delaySlider)
          return 1;

      return -1;
  }


  AudioPluginAudioProcessor& getProcessor() const
  {
      return static_cast<AudioPluginAudioProcessor&> (processor);
  }

  void updateTrackProperties()
  {
      auto trackColour = getProcessor().getTrackProperties().colour;
      auto& lf = getLookAndFeel();

      /* backgroundColour = (trackColour == Colour() ? lf.findColour (ResizableWindow::backgroundColourId)
                                                   : trackColour.withAlpha (1.0f).withBrightness (0.266f));*/
      repaint();
  }


private:

bool debug = false;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorEditor)
};

#endif