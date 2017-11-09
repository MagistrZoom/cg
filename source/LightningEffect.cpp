#include "LightningEffect.h"

static const char * pVS = "                                                          \n\
#version 330                                                                         \n\
                                                                                     \n\
layout (location = 0) in vec3 Position;                                              \n\
layout (location = 1) in vec2 TexCoord;                                              \n\
layout (location = 2) in vec3 Normal;                                                \n\
                                                                                     \n\
uniform mat4 gWVP;                                                                   \n\
uniform mat4 gWorld;                                                                 \n\
                                                                                     \n\
out vec2 UV;                                                                         \n\
out vec3 Normal0;                                                                    \n\
out vec3 WorldPos0;                                                                  \n\
                                                                                     \n\
void main()                                                                          \n\
{                                                                                    \n\
    gl_Position = gWVP * vec4(Position, 1.0);                                        \n\
    UV = TexCoord;                                                                   \n\
    Normal0 = (gWorld * vec4(Normal, 0.0)).xyz;                                      \n\
    WorldPos0 = (gWorld * vec4(Position, 1.0)).xyz;                                  \n\
}";

static const char * pFS = "                                                          \n\
#version 330                                                                         \n\
                                                                                     \n\
in vec2 UV;                                                                          \n\
in vec3 Normal0;                                                                     \n\
in vec3 WorldPos0;                                                                   \n\
                                                                                     \n\
out vec4 FragColor;                                                                  \n\
                                                                                     \n\
struct DirectionalLight                                                              \n\
{                                                                                    \n\
    vec3 color;                                                                      \n\
    float ambient_intensity;                                                         \n\
    vec3 direction;                                                                  \n\
    float diffuse_intensity;                                                         \n\
};                                                                                   \n\
                                                                                     \n\
uniform DirectionalLight gDirectionalLight;                                          \n\
uniform sampler2D gSampler;                                                          \n\
                                                                                     \n\
uniform vec3 gCameraPosition;                                                        \n\
uniform float gMatSpecularIntensity;                                                 \n\
uniform float gMatSpecularPower;                                                     \n\
                                                                                     \n\
void main()                                                                          \n\
{                                                                                    \n\
    vec4 AmbientColor = vec4(gDirectionalLight.color, 1.0f) *                        \n\
                        gDirectionalLight.ambient_intensity;                         \n\
                                                                                     \n\
    vec3 LightDirection = -gDirectionalLight.direction;                              \n\
    vec3 Normal = normalize(Normal0);                                                \n\
                                                                                     \n\
    float DiffuseFactor = dot(Normal, LightDirection);                               \n\
                                                                                     \n\
    vec4 DiffuseColor  = vec4(0, 0, 0, 0);                                           \n\
    vec4 SpecularColor = vec4(0, 0, 0, 0);                                           \n\
                                                                                     \n\
    if (DiffuseFactor > 0){                                                          \n\
        DiffuseColor = vec4(gDirectionalLight.color, 1.0f) *                         \n\
                       gDirectionalLight.diffuse_intensity *                         \n\
                       DiffuseFactor;                                                \n\
                                                                                     \n\
        vec3 VertexToCamera = normalize(gCameraPosition - WorldPos0);                \n\
        vec3 LightReflect = normalize(reflect(gDirectionalLight.direction, Normal)); \n\
        float SpecularFactor = dot(VertexToCamera, LightReflect);                    \n\
        if (SpecularFactor > 0) {                                                    \n\
            SpecularFactor = pow(SpecularFactor, gMatSpecularPower);                 \n\
            SpecularColor = vec4(gDirectionalLight.color, 1.0f) *                    \n\
                gMatSpecularIntensity * SpecularFactor;                              \n\
        }                                                                            \n\
    }                                                                                \n\
                                                                                     \n\
    FragColor = texture2D(gSampler, UV.xy) *                                         \n\
                (AmbientColor + DiffuseColor + SpecularColor);                       \n\
}";

bool LightingEffect::init()
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
    m_world_matrix_location = get_uniform_location("gWorld");
    m_sampler_location = get_uniform_location("gSampler");
    
    // Ambient and diffuse light
    m_direction_light_location.color = get_uniform_location("gDirectionalLight.color");
    m_direction_light_location.ambient_intensity = get_uniform_location("gDirectionalLight.ambient_intensity");
    m_direction_light_location.direction = get_uniform_location("gDirectionalLight.direction");
    m_direction_light_location.diffuse_intensity = get_uniform_location("gDirectionalLight.diffuse_intensity");

    // Specular light
    m_camera_position_location = get_uniform_location("gCameraPosition");
    m_mat_specular_intensity_location = get_uniform_location("gMatSpecularIntensity");
    m_mat_specular_power_location = get_uniform_location("gMatSpecularPower");

    if (m_direction_light_location.ambient_intensity == 0xFFFFFFFF ||
        m_wvp_location == 0xFFFFFFFF ||
        m_world_matrix_location == 0xFFFFFFFF ||
        m_sampler_location == 0xFFFFFFFF ||
        m_direction_light_location.color == 0xFFFFFFFF ||
        m_direction_light_location.diffuse_intensity == 0xFFFFFFFF ||
        m_direction_light_location.direction == 0xFFFFFFFF ||
        m_camera_position_location == 0xFFFFFFFF ||
        m_mat_specular_intensity_location == 0xFFFFFFFF ||
        m_mat_specular_power_location == 0xFFFFFFFF ) {
        return false;
    }

    return true;
}

void LightingEffect::set_wvp(const glm::mat4 & wvp)
{
    glUniformMatrix4fv(m_wvp_location, 1, GL_FALSE, &wvp[0][0]);
}

void LightingEffect::set_world_matrix(const glm::mat4 & wvp)
{
    glUniformMatrix4fv(m_world_matrix_location, 1, GL_FALSE, &wvp[0][0]);
}

void LightingEffect::set_directional_light(const DirectionLight & light)
{
    glUniform3f(m_direction_light_location.color, light.color.x, light.color.y, light.color.z);
    glUniform1f(m_direction_light_location.ambient_intensity, light.ambient_intensity);
    glm::vec3 direction = glm::normalize(light.direction);
    glUniform3f(m_direction_light_location.direction, direction.x, direction.y, direction.z);
    glUniform1f(m_direction_light_location.diffuse_intensity, light.diffuse_intensity);
}


void LightingEffect::set_camera_position(const glm::vec3 & pos)
{
    glUniform3f(m_camera_position_location, pos.x, pos.y, pos.z);
}


void LightingEffect::set_specular_intensity(float instensity)
{
    glUniform1f(m_mat_specular_intensity_location, instensity);
}


void LightingEffect::set_specular_power(float power)
{
    glUniform1f(m_mat_specular_power_location, power);
}
