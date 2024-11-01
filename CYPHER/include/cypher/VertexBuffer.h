#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include <cypher/WavefrontObjParser.h>

namespace cypher_graphics
{
    static inline const std::string screen_mesh =
        "# Flat plane                                                   \n"
        "#                                                              \n"
        "v 15.000000 -15.000000 0.000000                                \n"
        "v 15.000000 15.000000 0.000000                                 \n"
        "v -15.000000 15.000000 0.000000                                \n"
        "v -15.000000 -15.000000 0.000000                               \n"
        "                                                               \n"
        "vt 15.000000 0.000000                                          \n"
        "vt 15.000000 15.000000                                         \n"
        "vt 0.000000 15.000000                                          \n"
        "vt 0.000000 0.000000                                           \n"
        "                                                               \n"
        "g Teapot01                                                     \n"
        "f 1 / 1 2 / 2 3 / 3                                            \n"
        "f 1 / 1 3 / 3 4 / 4                                            \n"
        "                                                               \n"
        "g                                                              \n"
        "                                                               \n";


    struct Vertex
    {
        float position[3];
        float normal[3];
        float colour[4];
        float texCoord[2];
    };

    struct Uniforms
    {
        explicit Uniforms(OpenGLShaderProgram& shader)
        {
            projectionMatrix.reset(createUniform(shader, "projectionMatrix"));
            viewMatrix.reset(createUniform(shader, "viewMatrix"));

            t_uniform.reset(createUniform(shader, "t_uniform"));
        }

        std::unique_ptr<OpenGLShaderProgram::Uniform> projectionMatrix, viewMatrix, t_uniform;

    private:
        static OpenGLShaderProgram::Uniform* createUniform(OpenGLShaderProgram& shader,
            const char* uniformName) {
            using namespace ::juce::gl;

            if (glGetUniformLocation(shader.getProgramID(), uniformName) < 0)
                return nullptr;

            return new OpenGLShaderProgram::Uniform(shader, uniformName);
        }
    };

    struct Attributes
    {
        explicit Attributes(OpenGLShaderProgram& shader)
        {
            position.reset(createAttribute(shader, "position"));
            normal.reset(createAttribute(shader, "normal"));
            sourceColour.reset(createAttribute(shader, "sourceColour"));
            textureCoordIn.reset(createAttribute(shader, "textureCoordIn"));
        }

        void enable()
        {

            if (position.get() != nullptr)
            {
                juce::gl::glVertexAttribPointer(position->attributeID, 3, juce::gl::GL_FLOAT, juce::gl::GL_FALSE, sizeof(Vertex), nullptr);
                juce::gl::glEnableVertexAttribArray(position->attributeID);
            }

            if (normal.get() != nullptr)
            {
                juce::gl::glVertexAttribPointer(normal->attributeID, 3, juce::gl::GL_FLOAT, juce::gl::GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 3));
                juce::gl::glEnableVertexAttribArray(normal->attributeID);
            }

            if (sourceColour.get() != nullptr)
            {
                juce::gl::glVertexAttribPointer(sourceColour->attributeID, 4, juce::gl::GL_FLOAT, juce::gl::GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 6));
                juce::gl::glEnableVertexAttribArray(sourceColour->attributeID);
            }

            if (textureCoordIn.get() != nullptr)
            {
                juce::gl::glVertexAttribPointer(textureCoordIn->attributeID, 2, juce::gl::GL_FLOAT, juce::gl::GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 10));
                juce::gl::glEnableVertexAttribArray(textureCoordIn->attributeID);
            }
        }

        void disable()
        {
            if (position != nullptr)
                juce::gl::glDisableVertexAttribArray(position->attributeID);
            if (normal != nullptr)
                juce::gl::glDisableVertexAttribArray(normal->attributeID);
            if (sourceColour != nullptr)
                juce::gl::glDisableVertexAttribArray(sourceColour->attributeID);
            if (textureCoordIn != nullptr)
                juce::gl::glDisableVertexAttribArray(textureCoordIn->attributeID);
        }

        std::unique_ptr<OpenGLShaderProgram::Attribute> position, normal, sourceColour, textureCoordIn;

    private:
        static OpenGLShaderProgram::Attribute* createAttribute(OpenGLShaderProgram& shader,
            const char* attributeName)
        {

            if (juce::gl::glGetAttribLocation(shader.getProgramID(), attributeName) < 0)
                return nullptr;

            return new OpenGLShaderProgram::Attribute(shader, attributeName);
        }
    };



    static inline void createVertexListFromMesh(const WavefrontObjFile::Mesh& mesh, Array<Vertex>& list, Colour colour)
    {
        auto scale = 0.2f;
        WavefrontObjFile::TextureCoord defaultTexCoord = { 0.5f, 0.5f };
        WavefrontObjFile::Vertex defaultNormal = { 0.5f, 0.5f, 0.5f };

        for (int i = 0; i < mesh.vertices.size(); ++i)
        {
            auto& v = mesh.vertices.getReference(i);
            auto& n = (i < mesh.normals.size() ? mesh.normals.getReference(i)
                : defaultNormal);
            auto& tc = (i < mesh.textureCoords.size() ? mesh.textureCoords.getReference(i)
                : defaultTexCoord);

            list.add({ {
                          scale * v.x,
                          scale * v.y,
                          scale * v.z,
                      },
                      {
                          scale * n.x,
                          scale * n.y,
                          scale * n.z,
                      },
                      {colour.getFloatRed(), colour.getFloatGreen(), colour.getFloatBlue(), colour.getFloatAlpha()},
                      {tc.x, tc.y}
                });
        }
    };

    struct vertex_buffer
    {

        vertex_buffer(WavefrontObjFile::Shape& shape){
            Array<Vertex> vertices;
            numIndices = shape.mesh.indices.size();
            juce::gl::glGenBuffers(1, &vertexBuffer);
            juce::gl::glBindBuffer(juce::gl::GL_ARRAY_BUFFER, vertexBuffer);
            createVertexListFromMesh(shape.mesh, vertices, Colours::green);
            juce::gl::glBufferData(juce::gl::GL_ARRAY_BUFFER, vertices.size() * (int)sizeof(Vertex),
                vertices.getRawDataPointer(), juce::gl::GL_STATIC_DRAW);
            juce::gl::glGenBuffers(1, &indexBuffer);
            juce::gl::glBindBuffer(juce::gl::GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
            juce::gl::glBufferData(juce::gl::GL_ELEMENT_ARRAY_BUFFER, numIndices * (int)sizeof(juce::uint32),
                shape.mesh.indices.getRawDataPointer(), juce::gl::GL_STATIC_DRAW);
        }

        ~vertex_buffer()
        {
            juce::gl::glDeleteBuffers(1, &vertexBuffer);
            juce::gl::glDeleteBuffers(1, &indexBuffer);
        }

        void bind()
        {
            juce::gl::glBindBuffer(juce::gl::GL_ARRAY_BUFFER, vertexBuffer);
            juce::gl::glBindBuffer(juce::gl::GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        }

        GLuint vertexBuffer, indexBuffer;
        int numIndices;

    };

    struct screen_window
    {
        screen_window()
        {
            if (window_file.load(screen_mesh.c_str()).ok())
            {
                for (auto* s : window_file.shapes)
                    vertex_buffers.add(new vertex_buffer(*s));
            }
        }

        void draw(Attributes& attributes)
        {
            for (auto* vertexBuffer : vertex_buffers)
            {
                vertexBuffer->bind();
                attributes.enable();
                juce::gl::glDrawElements(juce::gl::GL_TRIANGLES, vertexBuffer->numIndices, juce::gl::GL_UNSIGNED_INT, nullptr);
                attributes.disable();
            }
        }

    private:

        WavefrontObjFile window_file;
        OwnedArray<vertex_buffer> vertex_buffers;
    };
}
#endif