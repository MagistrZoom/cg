#include "LightningTechnique.h"

static const char * pVS = "                                                         \n\
#version 330                                                                        \n\
                                                                                    \n\
layout (location = 0) in vec3 Position;                                             \n\
layout (location = 1) in vec2 TexCoord;                                             \n\
layout (location = 2) in vec3 Normal;                                               \n\
                                                                                    \n\
uniform mat4 gWVP;                                                                  \n\
uniform mat4 gWorld;                                                                \n\
                                                                                    \n\
out vec2 UV;                                                                        \n\
out vec3 Normal0;                                                                   \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    gl_Position = gWVP * vec4(Position, 1.0);                                       \n\
    UV = TexCoord;                                                                  \n\
    Normal0 = (gWorld * vec4(Normal, 0.0)).xyz;                                     \n\
}";

static const char * pFS = "                                                         \n\
#version 330                                                                        \n\
                                                                                    \n\
in vec2 UV;                                                                         \n\
in vec3 Normal0;                                                                    \n\
                                                                                    \n\
out vec4 FragColor;                                                                 \n\
                                                                                    \n\
struct DirectionalLight                                                             \n\
{                                                                                   \n\
    vec3 color;                                                                     \n\
    float ambient_intensity;                                                        \n\
    vec3 direction;                                                                 \n\
    float diffuse_intensity;                                                        \n\
};                                                                                  \n\
                                                                                    \n\
uniform DirectionalLight gDirectionalLight;                                         \n\
uniform sampler2D gSampler;                                                         \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    vec4 AmbientColor = vec4(gDirectionalLight.color, 1.0f) *                       \n\
                        gDirectionalLight.ambient_intensity;                        \n\
                                                                                    \n\
    float DiffuseFactor = dot(normalize(Normal0), -gDirectionalLight.direction);    \n\
                                                                                    \n\
    vec4 DiffuseColor;                                                              \n\
                                                                                    \n\
    if (DiffuseFactor > 0){                                                         \n\
        DiffuseColor = vec4(gDirectionalLight.color, 1.0f) *                        \n\
                       gDirectionalLight.diffuse_intensity *                        \n\
                       DiffuseFactor;                                               \n\
    }                                                                               \n\
    else{                                                                           \n\
        DiffuseColor = vec4(0,0,0,0);                                               \n\
    }                                                                               \n\
                                                                                    \n\
    FragColor = texture2D(gSampler, UV.xy) *                                        \n\
                (AmbientColor + DiffuseColor);                                      \n\
}";

bool LightingTechnique::init()
{
    if (!Technique::init()) {
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
    m_world_matrix_location = get_uniform_location("gWorld");
    m_sampler_location = get_uniform_location("gSampler");
    m_direction_light_location.color = get_uniform_location("gDirectionalLight.color");
    m_direction_light_location.ambient_intensity = get_uniform_location("gDirectionalLight.ambient_intensity");
    m_direction_light_location.direction = get_uniform_location("gDirectionalLight.direction");
    m_direction_light_location.diffuse_intensity = get_uniform_location("gDirectionalLight.diffuse_intensity");

    if (m_direction_light_location.ambient_intensity == 0xFFFFFFFF ||
        m_wvp_location == 0xFFFFFFFF ||
        m_world_matrix_location == 0xFFFFFFFF ||
        m_sampler_location == 0xFFFFFFFF ||
        m_direction_light_location.color == 0xFFFFFFFF ||
        m_direction_light_location.diffuse_intensity == 0xFFFFFFFF ||
        m_direction_light_location.direction == 0xFFFFFFFF) {
        return false;
    }

    return true;
}

void LightingTechnique::set_wvp(const Matrix4f & wvp)
{
    glUniformMatrix4fv(m_wvp_location, 1, GL_TRUE, (const GLfloat*)wvp.m);
}

void LightingTechnique::set_world_matrix(const Matrix4f & wvp)
{
    glUniformMatrix4fv(m_world_matrix_location, 1, GL_TRUE, (const GLfloat*)wvp.m);
}

void LightingTechnique::set_texture_unit(unsigned int texture_unit)
{
    glUniform1i(m_sampler_location, texture_unit);
}

void LightingTechnique::set_directional_light(const DirectionLight & light)
{
    glUniform3f(m_direction_light_location.color, light.color.x, light.color.y, light.color.z);
    glUniform1f(m_direction_light_location.ambient_intensity, light.ambient_intensity);
    Vector3f direction = light.direction;
    direction.Normalize();
    glUniform3f(m_direction_light_location.direction, direction.x, direction.y, direction.z);
    glUniform1f(m_direction_light_location.diffuse_intensity, light.diffuse_intensity);
}
