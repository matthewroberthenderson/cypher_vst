#ifndef OPEN_GL
#define OPEN_GL

#include <cypher/Cypher.h>
#include <cypher/VertexBuffer.h>
#include "WavefrontObjParser.h"

//embed unchanging elements
static const std::string vertex_shader =
"attribute vec4 position;                                       \n"
"attribute vec4 sourceColour;                                   \n"
"attribute vec2 textureCoordIn;                                 \n"
"                                                               \n"
"uniform mat4 projectionMatrix;                                 \n"
"uniform mat4 viewMatrix;                                       \n"
"                                                               \n"
"varying vec4 destinationColour;                                \n"
"varying vec2 textureCoordOut;                                  \n"
"                                                               \n"
"void main()                                                    \n"
"{                                                              \n"
"    destinationColour = sourceColour;                          \n"
"    textureCoordOut = textureCoordIn;                          \n"
"    gl_Position = projectionMatrix * viewMatrix * position;    \n"
"}                                                              \n";

static const std::string fragment_shader =
"varying vec4 destinationColour;                                                                                \n"
"varying vec2 textureCoordOut;                                                                                  \n"
"uniform float t_uniform;                                                                                       \n"
"#define FALL_OFF_DX 1.2                                                                                        \n"
"#define resX 15.0                                                                                              \n"
"#define resY 15.0                                                                                              \n"
"#define RESFACT 512                                                                                            \n"
"float pix_ratio;                                                                                               \n"
"float lerp_s(float a, float b, float x) {                                                                      \n"
"    return (clamp(x, a, b) - a) / (b - a);                                                                     \n"
"}                                                                                                              \n"
"void blend(inout vec4 baseCol, vec4 color, float alpha) {                                                      \n"
"    baseCol = vec4(mix(baseCol.rgb, color.rgb, alpha * color.a), 1.0);                                         \n"
"}                                                                                                              \n"
"void grid_draw(inout vec4 baseCol, vec2 xy, float stepSize, vec4 gridCol) {                                    \n"
"    float hlw = 0.1 * pix_ratio * 0.5;                                                                         \n"
"    float mul = 1.0 / stepSize;                                                                                \n"
"    vec2 gf = abs(vec2(-0.5) + fract((xy + vec2(stepSize) * 0.5) * mul));                                      \n"
"    float g = 1.0 - lerp_s(hlw * mul, (hlw + pix_ratio * FALL_OFF_DX) * mul, min(gf.x, gf.y));                 \n"
"    blend(baseCol, gridCol, g);                                                                                \n"
"}                                                                                                              \n"
"void main(){                                                                                                   \n"
"    vec2 uv = textureCoordOut / vec2(resX, resY);                                                              \n"
"    float aspect = resX / resY;                                                                                \n"
"    float z = 0.0;                                                                                             \n"
"    float graphRange = 0.4 + pow(1.2, z * z * z);                                                              \n"
"    vec2 graphSize = vec2(aspect * graphRange, graphRange) * 2.0;                                              \n"
"    vec2 graphCenter = vec2(0.5, 0.1);                                                                         \n"
"    vec2 graphPos = graphCenter - graphSize * 0.5;                                                             \n"
"    vec2 xy = graphPos + uv * graphSize;                                                                       \n"
"    pix_ratio = graphSize.y / RESFACT;                                                                         \n"
"    float t = length(0.5 - uv) * 1.414;                                                                        \n"
"    t = t * t * t;                                                                                             \n"
"    vec4 col = mix(vec4(.1, .1, .1, 1.0),vec4(0.0, 0.0, 0.0, 1.0), t);                                   \n"
"    float range = graphSize.y * 2.0;                                                                           \n"
"    grid_draw(col, xy, 0.1, vec4(1.0, 1.0, 1.0, 0.1));                                                         \n"
"    grid_draw(col, xy, 0.5, vec4(1.0, 1.0, 1.0, 0.1));                                                         \n"
"    grid_draw(col, xy, 1.0, vec4(1.0, 1.0, 1.0, 0.4));                                                         \n"
"    float rate = 0.5;                                                                                          \n"
"    float pulse = fract(t_uniform * rate) * 4.0 - 1.5;                                                         \n"
"    float fade = pulse - xy.x;                                                                                 \n"
"    if (fade < 0.0) fade += 4.0;                                                                               \n"
"    fade *= 0.25;                                                                                              \n"
"    fade = clamp(fade / rate, 0.0, 1.0);                                                                       \n"
"    fade = 1.0 - fade;                                                                                         \n"
"    fade = fade * fade * fade;                                                                                 \n"
"    fade *= step(-1.5, xy.x) * step(xy.x, 2.5);                                                                \n"
"    vec4 pulseCol = vec4(0.0, 1.0, 0.7, fade * 1.5);                                                           \n"
"    float hlw = 10.0 * pix_ratio * 0.5;                                                                        \n"
"    float left = xy.x - hlw - pix_ratio * FALL_OFF_DX;                                                         \n"
"    float right = xy.x + hlw + pix_ratio * FALL_OFF_DX;                                                        \n"
"    float closest = 100000.0;                                                                                  \n"
"    for (float x = left; x <= right; x += pix_ratio * 0.08){                                                   \n"
"        float CYPHER_EQUATION = sin(x * 10.) * 0.1;                                                            \n"
"        vec2 diff = vec2(x, CYPHER_EQUATION) - xy;                                                             \n"
"        float dSqr = dot(diff, diff);                                                                          \n"
"        closest = min(dSqr, closest);                                                                          \n"
"    }                                                                                                          \n"
"    float c = 1.0 - lerp_s(0.0, hlw + pix_ratio * FALL_OFF_DX, sqrt(closest));                                 \n"
"    blend(col, pulseCol, c * c * c);                                                                           \n"
"    pulseCol.a = 1.5;                                                                                          \n"
"    gl_FragColor = col;                                                                                        \n"
"}                                                                                                              \n";

namespace cypher_graphics
{

}


struct GL_hooks
{

    struct shader_type
    {
        const char *name;
        const char *fragmentShader;
    };

    static Array<shader_type> get_preset_shaders() {
        shader_type presets[] =
        {
            { "Oscilloscope",
            fragment_shader.c_str()
            }
        };

        return Array<shader_type>(presets, numElementsInArray(presets));
    }

};
#endif