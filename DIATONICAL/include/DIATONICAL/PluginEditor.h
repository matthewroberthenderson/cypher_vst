#pragma once
#include "PluginProcessor.h"
#include <juce_opengl/juce_opengl.h>

using namespace juce::gl;
class Look : public juce::LookAndFeel_V4
{
public:
    void drawComboBox(juce::Graphics &g, int w, int h, bool, int, int, int, int, juce::ComboBox &) override
    {
        g.setColour(juce::Colours::cyan.withAlpha(0.1f));
        g.fillRoundedRectangle(0, 0, w, h, 4.0f);
        g.setColour(juce::Colours::cyan);
        g.drawRoundedRectangle(0, 0, w, h, 4.0f, 1.0f);
    }
};

class ChordAudioProcessorEditor : public juce::AudioProcessorEditor,
                                  public juce::OpenGLRenderer
{
public:
    ChordAudioProcessorEditor(ChordAudioProcessor &p)
        : AudioProcessorEditor(&p), processor(p)
    {
        setLookAndFeel(&customLook);

        // Add items BEFORE setting the index 
        rootSelector.addItemList({"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"}, 1);
        rootSelector.setSelectedItemIndex(processor.currentRoot, juce::dontSendNotification);

        rootSelector.onChange = [this]
        {
            processor.currentRoot = rootSelector.getSelectedItemIndex();
            processor.generateStaticMap();
            processor.updateHostDisplay(); // Tells daw the state has changed
        };
        addAndMakeVisible(rootSelector);

        scaleSelector.addItemList({"Major", "Minor"}, 1);
        scaleSelector.setSelectedItemIndex(processor.currentScale, juce::dontSendNotification);

        scaleSelector.onChange = [this]
        {
            processor.currentScale = scaleSelector.getSelectedItemIndex();
            processor.generateStaticMap();
            processor.updateHostDisplay(); 
        };
        addAndMakeVisible(scaleSelector);

        bachToggle.setButtonText("BACH MODE");
        bachToggle.setToggleState(processor.bachMode, juce::dontSendNotification);

        bachToggle.onClick = [this]
        {
            processor.bachMode = bachToggle.getToggleState();
            processor.generateStaticMap();
            processor.updateHostDisplay();
        };
        addAndMakeVisible(bachToggle);

        openGLContext.setRenderer(this);
        openGLContext.setSwapInterval(1); // Enable VSync
        openGLContext.attachTo(*this);

        setSize(300, 300);
    }

    ~ChordAudioProcessorEditor() override
    {
        openGLContext.detach();
        setLookAndFeel(nullptr);
    }

    void newOpenGLContextCreated() override
    {
        shader.reset(new juce::OpenGLShaderProgram(openGLContext));

        const char *vShader = R"(
            attribute vec4 position;
            void main() { gl_Position = position; }
        )";

        const char *fShader = R"(
            uniform float utime;
            uniform vec2 uresolution;
            void main() {
                vec2 uv = gl_FragCoord.xy / uresolution.xy;
                vec3 S=vec3(0.);
                for(int j=0;j<25;++j) { 
                    vec3 s=vec3(1.); 
                    float t=(utime+float(j)/(10.*25.))*.2;
                    vec2 uv = mat2(cos(t),sin(t),-sin(t),cos(t))*(((gl_FragCoord.xy+vec2(j*5,j/5)/5.)-uresolution.xy/2.)/uresolution.y);
                    float a=length(uv)*3.1415*mix(0.1,0.8,0.5+0.5*cos(1.5*t)), b=atan(-uv.y, -uv.x)+3.1415;
                    uv = vec2(cos(b)*sin(a/cos(a))/(1.+cos(a))+2.*t,sin(b)*sin(a)/(1.+cos(a))+0.7*t);
                    for(int i=0;i<3;++i) {
                        float f=6.1*float(i)/3.; vec2 d = vec2(cos(f),sin(f));
                        s*=sin(40.*length(d*dot(uv, d)-uv)*sign(dot(uv, vec2(-d.y,d.x)))+t)*sin(6.282*(vec3(j+i)/25.+vec3(0.,t*0.33,0.66)));
                    } 
                    S+=pow(max(vec3(0.), s),vec3(1./3.));
                } gl_FragColor = vec4(pow(3.*S/67.,vec3(1.5))*0.3, 1.);
            }
        )";

        if (shader->addVertexShader(vShader) && shader->addFragmentShader(fShader) && shader->link())
            shader->use();
    }

    void renderOpenGL() override
    {
        juce::OpenGLHelpers::clear(juce::Colours::black);
        if (shader != nullptr && shader->getProgramID() != 0)
        {
            shader->use();

            shader->setUniform("utime", (float)frameCounter / 60.0f);
            shader->setUniform("uresolution", (float)getWidth(), (float)getHeight());

            auto posAttrib = openGLContext.extensions.glGetAttribLocation(shader->getProgramID(), "position");
            openGLContext.extensions.glEnableVertexAttribArray(posAttrib);

            // Just doing a full screen quad. Not great, could probably use a triangle but getting weired errors with that.
            // \TODO resolve that.
            static const float vertices[] = {-1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f};
            openGLContext.extensions.glVertexAttribPointer(posAttrib, 2, juce::gl::GL_FLOAT, juce::gl::GL_FALSE, 0, vertices);
            glDrawArrays(juce::gl::GL_TRIANGLE_STRIP, 0, 4);

            openGLContext.extensions.glDisableVertexAttribArray(posAttrib);
        }

        frameCounter++;
        openGLContext.triggerRepaint();
    }

    void openGLContextClosing() override
    {
        shader.reset();
    }

    void paint(juce::Graphics &g) override
    {
        g.setColour(juce::Colours::cyan);
        g.setFont(24.0f);
        g.drawText("DIATONICAL", 0, 30, getWidth(), 30, juce::Justification::centred);

        g.setFont(14.0f);
        g.drawText("Root Key", 50, 75, 200, 20, juce::Justification::left);
        g.drawText("Scale", 50, 135, 200, 20, juce::Justification::left);
    }

    void resized() override
    {
        rootSelector.setBounds(50, 95, 200, 35);
        scaleSelector.setBounds(50, 155, 200, 35);
        bachToggle.setBounds(50, 210, 200, 40);
    }

private:
    Look customLook;
    juce::ComboBox rootSelector, scaleSelector;
    juce::ToggleButton bachToggle;

    juce::OpenGLContext openGLContext;
    std::unique_ptr<juce::OpenGLShaderProgram> shader;
    int frameCounter = 0;

    ChordAudioProcessor &processor;
};