#ifndef CYPHER_H
#define CYPHER_H

#include "JuceHeader.h"
//#include <cypher/CypherControls.h>
#include <cypher/OpenGL.h>
#include <cypher/VertexBuffer.h>
#include <cypher/Cypher.h>
#include "cypher/exprtk.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <assert.h>



class SineWaveSound : public SynthesiserSound
{
public:
    SineWaveSound() {}

    bool appliesToNote(int /*midiNoteNumber*/) override { return true; }
    bool appliesToChannel(int /*midiChannel*/) override { return true; }
};

class EquationEditor : public TextEditor
{
public:
    static constexpr auto kDefaultJustification{ juce::Justification::topLeft };
    using TextEditor::TextEditor;
    double _t = 0;
    std::string defaultEquation = "sin(x)";

    void setBBText(const juce::String& bbText) {

        initialise();
        addEquationEditorText("ss");
    };

    typedef exprtk::symbol_table<double> symbol_table_t;
    typedef exprtk::expression<double>   expression_t;
    typedef exprtk::parser<double>       parser_t;
    symbol_table_t symbol_table;
    expression_t expression;
    parser_t parser;
    double delta = 0;
    std::string text;

    bool update_expression(std::string expression_string) {
        expression.register_symbol_table(symbol_table);
        symbol_table.add_variable("x", delta);

        if (parser.compile(expression_string, expression)) {
            return true; //todo error check?
        }
        return false;
    };

    EquationEditor() {
    };


    //template <typename T>
    double trig_function(double _delta) {
        delta = _delta;
        return expression.value();
    };


private:
    bool                     m_listPrefix{ false };
    bool                     m_quotePrefix{ false };
    juce::Justification      m_justification{ kDefaultJustification };

    void initialise() {
        setJustification(kDefaultJustification);
        m_listPrefix = false;
        m_quotePrefix = false;
        clear();
    };

    void addEquationEditorText(const juce::String& text, const juce::String& value = {}) {
        if (text.isEmpty())
            return;
        setColour(juce::TextEditor::textColourId, juce::Colour::fromString("blue"));
        moveCaretToEnd();
        insertTextAtCaret(text);
        m_listPrefix = false;
        m_quotePrefix = false;
    };

    void parseJustification(const juce::String& justification, bool enable) {
        m_justification = kDefaultJustification;
        setJustification(m_justification);
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EquationEditor)
};

enum synth_type {
    saw,
    sine,
    sqr,
    custom

};

class value_holder {
public:
    double custom_expression_result = 0;
    EquationEditor* eq;
    std::string last_equation;
    value_holder() {
        eq = new EquationEditor();
        eq->update_expression(eq->defaultEquation);
    };

    void update_equation(std::string equation) {
        if (equation != "")
            eq->update_expression(equation);
        last_equation = equation;
    };

    double compile(float delta)
    {
        return eq->trig_function(delta);
    };

    ~value_holder() {
        assert(eq);
        delete eq;
    }
};

class SineWaveVoice : public SynthesiserVoice
{
public:
    SineWaveVoice(value_holder* custom_value_holder_ref) :custom_value_holder(custom_value_holder_ref) {}

    synth_type type = synth_type::custom;
    value_holder* custom_value_holder;
    double custom_equation_value = 0;
    std::string alog_updated = "sin(x)";

    bool canPlaySound(SynthesiserSound* sound) override
    {
        return dynamic_cast<SineWaveSound*> (sound) != nullptr;
    }

    void startNote(int midiNoteNumber, float velocity,
        SynthesiserSound* /*sound*/,
        int /*currentPitchWheelPosition*/) override
    {
        currentAngle = 0.0;
        level = velocity * 0.15;
        tailOff = 0.0;

        ////get text from custom input
        //auto algo = custom_value_holder->eq->text;

        //if (alog_updated != algo) {
        //    alog_updated = custom_value_holder->eq->text;

        //    custom_value_holder->eq->update_expression(alog_updated);
        //}

        auto cyclesPerSecond = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        auto cyclesPerSample = cyclesPerSecond / getSampleRate();
        angleDelta = cyclesPerSample * MathConstants<double>::twoPi;
    }

    void stopNote(float /*velocity*/, bool allowTailOff) override
    {
        if (allowTailOff)
        {

            if (tailOff == 0.0)
                tailOff = 1.0;
        }
        else
        {
            clearCurrentNote();
            angleDelta = 0.0;
        }
    }



    void pitchWheelMoved(int /*newValue*/) override
    {
    }

    void controllerMoved(int /*controllerNumber*/, int /*newValue*/) override
    {
    }

    double square_wave(double currentAngle, double duty_cycle = 0.5) {

        double decision = 0.0;
        double angle_mod = fmod(currentAngle, 2.0 * MathConstants<double>::pi);

        if (angle_mod < (2 * MathConstants<double>::pi * duty_cycle)) {
            decision = 1.0;
        }
        else {
            decision = -1.0;
        }
        return decision;
    }


    void renderNextBlock(AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        double waveform = 0;

        auto sawtooth_wave = (2 / MathConstants<double>::pi) * (currentAngle - MathConstants<double>::pi / 2 - MathConstants<double>::pi * ceil((currentAngle + MathConstants<double>::pi / 2) / MathConstants<double>::pi));
        auto sinewave = sin(currentAngle);

        if (angleDelta != 0.0)
        {
            if (tailOff > 0.0)
            {
                while (--numSamples >= 0)
                {
                    waveform = custom_value_holder->compile(currentAngle);
                    auto currentSample = (float)(waveform * level * tailOff);
                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample(i, startSample, currentSample);

                    currentAngle += angleDelta;
                    ++startSample;

                    //check tail off
                    tailOff *= 0.99;
                    if (tailOff <= 0.005)
                    {
                        clearCurrentNote();
                        angleDelta = 0.0;
                        break;
                    }
                }
            }
            else
            {
                while (--numSamples >= 0)
                {
                    waveform = custom_value_holder->compile(currentAngle);
                    auto currentSample = (float)(waveform * level);
                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample(i, startSample, currentSample);

                    currentAngle += angleDelta;
                    ++startSample;
                }
            }
        }
    }

    using SynthesiserVoice::renderNextBlock;

private:
    double currentAngle = 0.0;
    double angleDelta = 0.0;
    double level = 0.0;
    double tailOff = 0.0;
};




//class CypherControls;

struct cypher_time
{
    virtual ~cypher_time() = default;

    //should use this but borks my amd cpu?? not my intel?
    //need to work out why
    float getValue() const
    {
        double v = fmod(Time::getMillisecondCounterHiRes(), 100.0);
        return (float)v; //(v >= 1.0 ? (2.0 - v) : v);
    }

protected:
    double speed = 0.0004 + 0.0007 * Random::getSystemRandom().nextDouble(),
           phase = Random::getSystemRandom().nextDouble();
};

class CypherRenderer final : public Component,
                             private OpenGLRenderer,
                             private AsyncUpdater
{
public:
    String current_equation = "";
    float timer = 0.0;
    CypherRenderer();

    ~CypherRenderer() override;

    String get_equation()
    {
        return current_equation;
    };

    void newOpenGLContextCreated() override;

    void openGLContextClosing() override;

    void freeAllContextObjects();

    inline Colour getUIColourIfAvailable(LookAndFeel_V4::ColourScheme::UIColour uiColour, Colour fallback = Colour(0.0)) noexcept;

    void renderOpenGL() override;

    Matrix3D<float> getProjectionMatrix() const;

    Matrix3D<float> getViewMatrix() const;

    std::string replaceSubstring(std::string str, const std::string &toReplace, const std::string &replacement)
    {
        std::string::size_type pos = 0;
        int finds = 0;
        while ((pos = str.find(toReplace, pos)) != std::string::npos)
        {
                str.replace(pos, toReplace.length(), replacement);
                pos += replacement.length(); // Move past the replacement

        }
        return str;
    }

    void setShaderProgram(const String &vertexShader, const String &fragmentShader, const String &synth);

    void paint(Graphics &) override {}

    void resized() override;

    juce::Rectangle<int> bounds;
    Draggable3DOrientation draggableOrientation;
    float scale = 0.800f, rotationSpeed = 0.0f;
    cypher_time t_uniform;
    CriticalSection mutex;

private:
    void handleAsyncUpdate() override;

    OpenGLContext openGLContext;


   // std::unique_ptr<CypherControls> controlsOverlay;
    std::unique_ptr<OpenGLShaderProgram> shader;
    std::unique_ptr<cypher_graphics::screen_window> shape;
    std::unique_ptr<cypher_graphics::Attributes> attributes;
    std::unique_ptr<cypher_graphics::Uniforms> uniforms;

    OpenGLTexture texture;

    CriticalSection shaderMutex;
    String newVertexShader, newFragmentShader, statusText;

    // todo need to use these later, but need to get a good hot reload system going that doesnt make the cpu hitch.
    std::string readFileToString(const std::string &filePath)
    {
        std::ifstream file(filePath);

        if (!file.is_open())
        {
            throw std::runtime_error("file io failed: " + filePath);
        }

        std::ostringstream sstream;
        sstream << file.rdbuf();

        return sstream.str();
    }



    void updateShader();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CypherRenderer)
};
#endif