#include "cypher/PluginEditor.h"
#include "cypher/PluginProcessor.h"


#define WINDOW_MIN_WIDTH 1024
#define WINDOW_MIN_HEIGHT 700

namespace audio_plugin {

    AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor & owner)
            : AudioProcessorEditor(owner),
            midiKeyboard(owner.keyboardState, MidiKeyboardComponent::horizontalKeyboard),
            gainAttachment(owner.state, "gain", gainSlider),
            delayAttachment(owner.state, "delay", delaySlider)
        {

        setSize(WINDOW_MIN_WIDTH, WINDOW_MIN_HEIGHT);
        openGLComponent = std::make_unique<CypherRenderer>();

        openGLComponent.get()->setBounds(juce::Rectangle<int>(WINDOW_MIN_WIDTH, WINDOW_MIN_HEIGHT));
        addAndMakeVisible(openGLComponent.get(), 0);



        openGLComponent.get()->addChildComponent(gainSlider, -1);
        gainSlider.setSliderStyle(Slider::Rotary);
        gainSlider.setVisible(true);

        openGLComponent.get()->addChildComponent(delaySlider, -1);
        delaySlider.setSliderStyle(Slider::Rotary);
        backgroundColour.fromRGB(0, 0, 0);
        delaySlider.setVisible(true);

        openGLComponent.get()->addChildComponent(gainLabel, -1);
        gainLabel.attachToComponent(&gainSlider, false);
        gainLabel.setFont(Font(11.0f));
        gainLabel.setVisible(true);

        openGLComponent.get()->addChildComponent(delayLabel, -1);
        delayLabel.attachToComponent(&delaySlider, false);
        delayLabel.setFont(Font(11.0f));
        delayLabel.setVisible(true);

        openGLComponent.get()->addChildComponent(EquationEditorLabel, -1);

        EquationEditorLabel.setFont(Font(11.0f));

        openGLComponent.get()->addChildComponent(midiKeyboard, -1);
        //midiKeyboard.setVisible(true);

        setResizeLimits(WINDOW_MIN_WIDTH, WINDOW_MIN_HEIGHT, WINDOW_MIN_WIDTH, WINDOW_MIN_HEIGHT);
        setResizable(true, owner.wrapperType != juce::AudioProcessor::wrapperType_AudioUnitv3);

        lastUIWidth.referTo(owner.state.state.getChildWithName("uiState").getPropertyAsValue("width", nullptr));
        lastUIHeight.referTo(owner.state.state.getChildWithName("uiState").getPropertyAsValue("height", nullptr));

        setSize(WINDOW_MIN_WIDTH, WINDOW_MIN_HEIGHT);

        lastUIWidth.addListener(this);
        lastUIHeight.addListener(this);



        startTimerHz(30);

    }

    AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor() {}

    void AudioPluginAudioProcessorEditor::paint(juce::Graphics& g) {

        g.fillAll(
            getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

        g.setColour(juce::Colours::white);
        g.setFont(15.0f);

    }

    void AudioPluginAudioProcessorEditor::resized() {




        auto area = getLocalBounds().reduced(10).removeFromBottom(getLocalBounds().reduced(10).getHeight() * 0.9);
        auto buttonArea = area.removeFromTop(area.getHeight() / 2); // Top half for buttons
        auto headerFooterHeight = 250.0;

        juce::GridItem gain_slider = juce::GridItem(gainSlider);
        gain_slider = gain_slider.withHeight(.5f).withWidth(.5f);
        gain_slider.minWidth = headerFooterHeight;
        gain_slider.minHeight = headerFooterHeight;

        juce::GridItem delay_slider = juce::GridItem(delaySlider);
        delay_slider.minWidth = headerFooterHeight;
        delay_slider.minHeight = headerFooterHeight;
        delay_slider = delay_slider.withHeight(.5f).withWidth(.5f);

        area.removeFromTop(80);

        ////keyboard
        auto keyboardArea = area;


        juce::Grid grid;
        grid.templateColumns = {
            juce::Grid::TrackInfo(juce::Grid::Fr(1)),
            juce::Grid::TrackInfo(juce::Grid::Fr(1)),
           // juce::Grid::TrackInfo(juce::Grid::Fr(1))
        };
        grid.templateRows = {
            juce::Grid::TrackInfo(juce::Grid::Fr(1)),
            juce::Grid::TrackInfo(juce::Grid::Fr(1))
        };



        grid.columnGap = juce::Grid::Px(10);
        grid.rowGap = juce::Grid::Px(10);

        grid.items.add(gain_slider);
        grid.items.add(delay_slider);

        auto gridPadding = 10;
        grid.performLayout(buttonArea);

        midiKeyboard.setBounds(keyboardArea);

    }


    void AudioPluginAudioProcessorEditor::timerCallback()
    {
            String equation = openGLComponent.get()->get_equation();
            value_holder_instance_ref->eq->text = equation.toStdString();
            persistent_data::getInstance().equation_text_current = equation.toStdString();
            openGLComponent.get()->toBack();
    }


}  // namespace audio_plugin