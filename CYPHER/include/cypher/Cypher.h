#ifndef CYPHER_H
#define CYPHER_H

#include "JuceHeader.h"
#include <cypher/CypherControls.h>
#include <cypher/OpenGL.h>
#include <cypher/VertexBuffer.h>
#include <cypher/Cypher.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

class CypherControls;

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


    std::unique_ptr<CypherControls> controlsOverlay;
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