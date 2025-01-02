// #include <Infinite/Cypher.h>

// CypherRenderer::CypherRenderer()
// {
//     if (auto* peer = getPeer())
//         peer->setCurrentRenderingEngine(0);

//     setOpaque(true);
//     controlsOverlay.reset(new CypherControls(*this));
//     addAndMakeVisible(controlsOverlay.get());
//     openGLContext.setOpenGLVersionRequired(OpenGLContext::openGL3_2);
//     openGLContext.setRenderer(this);
//     openGLContext.attachTo(*this);
//     openGLContext.setContinuousRepainting(true);
//     controlsOverlay->initialise();

//     setSize(500, 500);
// }

// CypherRenderer::~CypherRenderer()
// {
//     openGLContext.detach();
// }

// void CypherRenderer::newOpenGLContextCreated()
// {
//     freeAllContextObjects();

//     if (controlsOverlay != nullptr)
//         controlsOverlay->updateShader();
// }

// void CypherRenderer::openGLContextClosing()
// {
//     freeAllContextObjects();
// }

// void CypherRenderer::freeAllContextObjects()
// {
//     shape.reset();
//     shader.reset();
//     attributes.reset();
//     uniforms.reset();
//     texture.release();
// }

// Colour CypherRenderer::getUIColourIfAvailable(LookAndFeel_V4::ColourScheme::UIColour uiColour, Colour fallback) noexcept
// {
//     if (auto* v4 = dynamic_cast<LookAndFeel_V4*>(&LookAndFeel::getDefaultLookAndFeel()))
//         return v4->getCurrentColourScheme().getUIColour(uiColour);

//     return fallback;
// }

// inline void CypherRenderer::renderOpenGL()
// {
//     using namespace ::juce::gl;
//     const ScopedLock lock(mutex);
//     jassert(OpenGLHelpers::isContextActive());

//     auto desktopScale = (float)openGLContext.getRenderingScale();

//     OpenGLHelpers::clear(Colours::black);
//     updateShader();

//     if (shader.get() == nullptr)
//         return;

//     glEnable(GL_DEPTH_TEST);
//     glDepthFunc(GL_LESS);
//     glViewport(0, 0,
//         roundToInt(desktopScale * (float)bounds.getWidth()),
//         roundToInt(desktopScale * (float)bounds.getHeight()));
//     shader->use();
//     timer += 0.1;
//     if (uniforms->projectionMatrix != nullptr)
//         uniforms->projectionMatrix->setMatrix4(getProjectionMatrix().mat, 1, false);

//     if (uniforms->viewMatrix != nullptr)
//         uniforms->viewMatrix->setMatrix4(getViewMatrix().mat, 1, false);

//     if (uniforms->t_uniform != nullptr)
//         uniforms->t_uniform->set(/*t_uniform.getValue()*/timer);

//     shape->draw(*attributes);

//     // Reset the element buffers so child Components draw correctly
//     glBindBuffer(GL_ARRAY_BUFFER, 0);
//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
// }

// Matrix3D<float> CypherRenderer::getProjectionMatrix() const
// {
//     const ScopedLock lock(mutex);

//     auto w = 1.0f / (scale + 0.1f);
//     auto h = w * bounds.toFloat().getAspectRatio(false);

//     return Matrix3D<float>::fromFrustum(-w, w, -h, h, 4.0f, 30.0f);
// }

// Matrix3D<float> CypherRenderer::getViewMatrix() const
// {
//     const ScopedLock lock(mutex);

//     auto viewMatrix = Matrix3D<float>::fromTranslation({ 0.0f, 1.0f, -10.0f });
//     auto rotationMatrix = Matrix3D<float>::rotation({ 0.0, 0.0, /*-0.3f*/ 0.0f });

//     return viewMatrix * rotationMatrix;
// }

// void CypherRenderer::setShaderProgram(const String& vertexShader, const String& fragmentShader, const String& synth)
// {
//     // No concurrent access to shader
//     const ScopedLock lock(shaderMutex);

//     // replace the marker with our new equation 
//     // from the equation editor if we have something in it
//     if (synth.isNotEmpty())
//     {
//         std::string toReplace = "float CYPHER_EQUATION = sin(x * 10.) * 0.1; ";
//         std::string replacement = "float CYPHER_EQUATION=" + synth.toStdString() + "*0.1;";
//         std::string modified_shader = replaceSubstring(fragmentShader.toStdString(), toReplace, replacement);
//         newVertexShader = vertexShader;
//         newFragmentShader = modified_shader;
//     }
//     else
//     {
//         newVertexShader = vertexShader;
//         newFragmentShader = fragmentShader;
//     }
// }

// void CypherRenderer::resized()
// {
//     const ScopedLock lock(mutex);
//     bounds = getLocalBounds();
//     controlsOverlay->setBounds(bounds);
//     draggableOrientation.setViewport(bounds);
// }

// void CypherRenderer::handleAsyncUpdate()
// {
//     const ScopedLock lock(shaderMutex); // Prevent concurrent access to shader strings and status
//     controlsOverlay->statusLabel.setText(statusText, dontSendNotification);
// }

// void CypherRenderer::updateShader()
// {
//     const ScopedLock lock(shaderMutex); // Prevent concurrent access to shader strings and status

//     if (newVertexShader.isNotEmpty() || newFragmentShader.isNotEmpty())
//     {
//         std::unique_ptr<OpenGLShaderProgram> newShader(new OpenGLShaderProgram(openGLContext));

//         if (newShader->addVertexShader(OpenGLHelpers::translateVertexShaderToV3(newVertexShader)) && newShader->addFragmentShader(OpenGLHelpers::translateFragmentShaderToV3(newFragmentShader)) && newShader->link())
//         {
//             shape.reset();
//             attributes.reset();
//             uniforms.reset();

//             shader.reset(newShader.release());
//             shader->use();

//             shape.reset(new OpenGLUtils::Shape());
//             attributes.reset(new OpenGLUtils::Attributes(*shader));
//             uniforms.reset(new OpenGLUtils::Uniforms(*shader));

//             statusText = "Equation Stable";
//         }
//         else
//         {

//             statusText = "Equation error! use linear equation functions like: sin,cos,fract,tan,atan";
//         }

//         triggerAsyncUpdate();

//         newVertexShader = {};
//         newFragmentShader = {};
//     }
// }
