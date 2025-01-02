#include "cypher/PluginEditor.h"
#include "cypher/PluginProcessor.h"


#define WINDOW_MIN_WIDTH 1024
#define WINDOW_MIN_HEIGHT 700





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

        if (debug)
            tabbedComp.addTab("Fragment", Colours::black, &fragmentEditorComp, false);
        tabbedComp.addTab("Synth", Colours::black, &synthEditorComp, false);
        //addAndMakeVisible(tabbedComp);
        tabbedComp.setTabBarDepth(25);
        tabbedComp.setTabBackgroundColour(0, juce::Colours::black);
        tabbedComp.setTabBackgroundColour(1, juce::Colours::black);
        juce::Colour n = tabbedComp.getTabBackgroundColour(0);
        fragmentEditorComp.setScrollbarThickness(0.0);
        fragmentDocument.addListener(this);
        synthDocument.addListener(this);

        openGLComponent.get()->addChildComponent(tabbedComp, -1);
        tabbedComp.setVisible(true);


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

        addAndMakeVisible(statusLabel);
        statusLabel.setJustificationType(Justification::topLeft);
        statusLabel.setFont(FontOptions(14.0f));



        lookAndFeelChanged();

        getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::white);
        getLookAndFeel().setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::grey);

        getLookAndFeel().setColour(juce::CodeEditorComponent::ColourIds::lineNumberBackgroundId, juce::Colours::black);
        getLookAndFeel().setColour(juce::CodeEditorComponent::ColourIds::backgroundColourId, juce::Colours::black);
        getLookAndFeel().setColour(juce::CodeEditorComponent::ColourIds::defaultTextColourId, juce::Colours::white);
        getLookAndFeel().setColour(juce::CodeEditorComponent::ColourIds::lineNumberTextId, juce::Colours::black);

        getLookAndFeel().setColour(juce::TabbedComponent::backgroundColourId, juce::Colours::black);
        getLookAndFeel().setColour(juce::TabbedComponent::ColourIds::outlineColourId, juce::Colours::white);
        getLookAndFeel().setColour(juce::ScrollBar::ColourIds::backgroundColourId, juce::Colours::black);
        getLookAndFeel().setColour(juce::ScrollBar::ColourIds::trackColourId, juce::Colours::black);

        
        auto presets = GL_hooks::get_preset_shaders();
        const auto& p = presets[0];
        fragmentDocument.replaceAllContent(p.fragmentShader);
        
        synthDocument.replaceAllContent(value_holder_instance_ref->last_equation);

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

        auto area_tabs = getLocalBounds().reduced(4);
        auto top = area_tabs.removeFromTop(75);
        top.removeFromRight(70);
        statusLabel.setBounds(top);
        auto shaderArea = area_tabs.removeFromBottom(area_tabs.getHeight() / 2);
        shaderArea.removeFromTop(4);
        tabbedComp.setBounds(shaderArea);



    }


    void AudioPluginAudioProcessorEditor::timerCallback()
    {
            value_holder_instance_ref->update_equation(synthDocument.getAllContent().toStdString());
            openGLComponent.get()->toBack();
            auto nc = tabbedComp.getTabBackgroundColour(0);
            auto fragmentShader = fragmentDocument.getAllContent();
            openGLComponent->setShaderProgram(vertex_shader.c_str(), fragmentShader, synthDocument.getAllContent());
    }

