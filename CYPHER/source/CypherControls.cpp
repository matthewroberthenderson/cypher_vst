#include <cypher/CypherControls.h>

#define EXAMPLE_EQ "fract(mod(sin(x + tan(x)), 1.0) * 2.0)* sin(x) * 0.1"


CypherControls::CypherControls(CypherRenderer& d)
    : cypher_renderer_inst(d)
{
    addAndMakeVisible(statusLabel);
    statusLabel.setJustificationType(Justification::topLeft);
    statusLabel.setFont(FontOptions(14.0f));

    if (debug)
        tabbedComp.addTab("Fragment", Colours::black, &fragmentEditorComp, false);
    tabbedComp.addTab("Synth", Colours::black, &synthEditorComp, false);
    addAndMakeVisible(tabbedComp);
    tabbedComp.setTabBarDepth(25);
    tabbedComp.setTabBackgroundColour(0, juce::Colours::black);
    tabbedComp.setTabBackgroundColour(1, juce::Colours::black);
    juce::Colour n = tabbedComp.getTabBackgroundColour(0);
    fragmentEditorComp.setScrollbarThickness(0.0);
    fragmentDocument.addListener(this);
    synthDocument.addListener(this);
    auto presets = GL_hooks::get_preset_shaders();
}

void CypherControls::initialise() {
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

    // trigger the select visualiser code.
    // sets the inital shader up
    const auto presets = GL_hooks::get_preset_shaders();
    const auto& p = presets[0];
    fragmentDocument.replaceAllContent(p.fragmentShader);
    synthDocument.replaceAllContent(EXAMPLE_EQ);
    startTimer(1);
}

void CypherControls::resized()
{
    auto area = getLocalBounds().reduced(4);
    auto top = area.removeFromTop(75);
    top.removeFromRight(70);
    statusLabel.setBounds(top);
    auto shaderArea = area.removeFromBottom(area.getHeight() / 2);
    shaderArea.removeFromTop(4);
    tabbedComp.setBounds(shaderArea);
}

void CypherControls::updateShader()
{
    startTimer(10);
}

void CypherControls::mouseDown(const MouseEvent& e) {
    const ScopedLock lock(cypher_renderer_inst.mutex);
    cypher_renderer_inst.draggableOrientation.mouseDown(e.getPosition());
    buttonDown = true;
}

void CypherControls::mouseDrag(const MouseEvent& e) {
    const ScopedLock lock(cypher_renderer_inst.mutex);
    cypher_renderer_inst.draggableOrientation.mouseDrag(e.getPosition());
}

void CypherControls::codeDocumentTextInserted(const String&, int)
{
    startTimer(500);
}

void CypherControls::codeDocumentTextDeleted(int, int)
{
    startTimer(500);
}

void CypherControls::timerCallback()
{
    stopTimer();
    auto nc = tabbedComp.getTabBackgroundColour(0);
    auto fragmentShader = fragmentDocument.getAllContent();
    cypher_renderer_inst.current_equation = synthDocument.getAllContent();
    cypher_renderer_inst.setShaderProgram(vertex_shader.c_str(), fragmentShader, synthDocument.getAllContent());
}
