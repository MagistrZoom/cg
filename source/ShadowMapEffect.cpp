#include "ShadowMapEffect.h"

static const char* pVS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
layout (location = 0) in vec3 Position;                                             \n\
layout (location = 1) in vec2 TexCoord;                                             \n\
                                                                                    \n\
uniform mat4 gWVP;                                                                  \n\
                                                                                    \n\
out vec2 UV;                                                                        \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    gl_Position = gWVP * vec4(Position, 1.0);                                       \n\
    UV = TexCoord;                                                                  \n\
}";

static const char* pFS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
in vec2 UV;                                                                         \n\
uniform sampler2D gShadowMap;                                                       \n\
                                                                                    \n\
out vec4 FragColor;                                                                 \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    float Depth = texture2D(gShadowMap, UV).x;                                      \n\
    Depth = 1.0 - (1.0 - Depth) * 25.0;                                             \n\
    FragColor = vec4(Depth);                                                        \n\
}";

bool ShadowMapEffect::init()
{
    if (!Effect::init()) {
        return false;
    }

    if (!add_shader(GL_VERTEX_SHADER, pVS)) {
        return false;
    }

    if (!add_shader(GL_FRAGMENT_SHADER, pFS)) {
        return false;
    }

    if (!finalize()) {
        return false;
    }

    m_wvp_location = get_uniform_location("gWVP");
    m_texture_location = get_uniform_location("gShadowMap");

    if (m_wvp_location == invalid_uniform_location ||
        m_texture_location == invalid_uniform_location) {
        return false;
    }

    return true;
}

void ShadowMapEffect::set_wvp(const glm::mat4 & wvp)
{
    glUniformMatrix4fv(m_wvp_location, 1, GL_FALSE, &wvp[0][0]);
}

void ShadowMapEffect::set_texture_unit(unsigned int texture_unit)
{
    glUniform1i(m_texture_location, texture_unit);
}

