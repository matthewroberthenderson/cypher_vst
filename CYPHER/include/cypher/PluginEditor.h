#ifndef PLUGIN_EDITOR_H
#define PLUGIN_EDITOR_H

#include "PluginProcessor.h"
#include "cypher/Cypher.h"
#include "JuceHeader.h"
namespace audio_plugin {

class AudioPluginAudioProcessorEditor : public juce::AudioProcessorEditor,
    private juce::Timer,
    private juce::Value::Listener {

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



  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorEditor)
};
}  // namespace audio_plugin
#endif