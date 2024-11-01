#ifndef CYPHERCONTROLS_H
#define CYPHERCONTROLS_H
#include "JuceHeader.h"
#include <cypher/Cypher.h>

class CypherRenderer;

class CypherControls final : public Component,
    private CodeDocument::Listener,
    private Slider::Listener,
    private Timer
{
public:
    Label statusLabel;

    CypherControls(CypherRenderer& d);
    void initialise();
    void resized() override;
    void mouseUp(const MouseEvent&) override {};
    void mouseWheelMove(const MouseEvent&, const MouseWheelDetails& d) override {};
    void mouseMagnify(const MouseEvent&, float magnifyAmmount) override {};
    void updateShader();
    bool isMouseButtonDownThreadsafe() const { return buttonDown; }
    void mouseDown(const MouseEvent& e) override;
    void mouseDrag(const MouseEvent& e) override;

    //JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CypherControls)

private:
    bool debug = false;
    void sliderValueChanged(Slider*) override {}
    void codeDocumentTextInserted(const String& /*newText*/, int /*insertIndex*/) override;
    void codeDocumentTextDeleted(int /*startIndex*/, int /*endIndex*/) override;
    void timerCallback() override;
    void lookAndFeelChanged() override {}

    CypherRenderer& cypher_renderer_inst;
    CodeDocument fragmentDocument, synthDocument;
    CodeEditorComponent fragmentEditorComp{ fragmentDocument, nullptr },
        synthEditorComp{ synthDocument, nullptr };
    TabbedComponent tabbedComp{ TabbedButtonBar::TabsAtLeft };
    std::unique_ptr<FileChooser> textureFileChooser;
    std::atomic<bool> buttonDown{ false };

};

#endif