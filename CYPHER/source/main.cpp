#include "cypher/Cypher.h"
class MainWindow : public juce::DocumentWindow
{
public:
    MainWindow() : DocumentWindow("Main Window",
        juce::Desktop::getInstance().getDefaultLookAndFeel()
        .findColour(juce::ResizableWindow::backgroundColourId),
        juce::DocumentWindow::allButtons)
    {

        backgroundComponent.reset(new CypherRenderer());
        setContentOwned(backgroundComponent.get(), true);

        setVisible(true);
    }

    ~MainWindow() override
    {
    }

private:
    std::unique_ptr<CypherRenderer> backgroundComponent;
};
