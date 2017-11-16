#include "LightningEffect.h"
#include <stdio.h>

static const char * pVS = "                                                          \n\
#version 330                                                                         \n\
                                                                                     \n\
layout (location = 0) in vec3 Position;                                              \n\
layout (location = 1) in vec2 TexCoord;                                              \n\
layout (location = 2) in vec3 Normal;                                                \n\
                                                                                     \n\
uniform mat4 gWVP;                                                                   \n\
uniform mat4 gLightWVP;                                                              \n\
uniform mat4 gWorld;                                                                 \n\
                                                                                     \n\
out vec2 UV;                                                                         \n\
out vec3 Normal0;                                                                    \n\
out vec3 WorldPos0;                                                                  \n\
out vec4 LightSpacePos;                                                              \n\
                                                                                     \n\
void main()                                                                          \n\
{                                                                                    \n\
    gl_Position = gWVP * vec4(Position, 1.0);                                        \n\
    UV = TexCoord;                                                                   \n\
    LightSpacePos = gLightWVP * vec4(Position, 1.0);                                 \n\
    Normal0 = (gWorld * vec4(Normal, 0.0)).xyz;                                      \n\
    WorldPos0 = (gWorld * vec4(Position, 1.0)).xyz;                                  \n\
}";

static const char * pFS = "                                                         \n\
#version 330                                                                        \n\
                                                                                    \n\
const int MAX_POINT_LIGHTS = 3;                                                     \n\
const int MAX_SPOT_LIGHTS = 2;                                                      \n\
                                                                                    \n\
in vec2 UV;                                                                         \n\
in vec3 Normal0;                                                                    \n\
in vec3 WorldPos0;                                                                  \n\
in vec4 LightSpacePos;                                                              \n\
                                                                                    \n\
out vec4 FragColor;                                                                 \n\
                                                                                    \n\
struct BaseLight                                                                    \n\
{                                                                                   \n\
    vec3 color;                                                                     \n\
    float ambient_intensity;                                                        \n\
    float diffuse_intensity;                                                        \n\
};                                                                                  \n\
                                                                                    \n\
struct DirectionalLight                                                             \n\
{                                                                                   \n\
    BaseLight base;                                                                 \n\
    vec3 direction;                                                                 \n\
};                                                                                  \n\
                                                                                    \n\
struct Attenuation                                                                  \n\
{                                                                                   \n\
    float constant;                                                                 \n\
    float linear;                                                                   \n\
    float exp;                                                                      \n\
};                                                                                  \n\
                                                                                    \n\
struct PointLight                                                                   \n\
{                                                                                   \n\
    BaseLight base;                                                                 \n\
    vec3 position;                                                                  \n\
    Attenuation atten;                                                              \n\
};                                                                                  \n\
                                                                                    \n\
struct SpotLight                                                                    \n\
{                                                                                   \n\
    PointLight base;                                                                \n\
    vec3 direction;                                                                 \n\
    float cutoff;                                                                   \n\
};                                                                                  \n\
                                                                                    \n\
uniform DirectionalLight gDirectionalLight;                                         \n\
uniform sampler2D gSampler;                                                         \n\
uniform sampler2D gShadowMap;                                                       \n\
                                                                                    \n\
uniform vec3 gCameraPosition;                                                       \n\
uniform float gMatSpecularIntensity;                                                \n\
uniform float gMatSpecularPower;                                                    \n\
uniform PointLight gPointLights[MAX_POINT_LIGHTS];                                  \n\
uniform SpotLight  gSpotLights[MAX_SPOT_LIGHTS];                                    \n\
uniform int gNumPointLights;                                                        \n\
uniform int gNumSpotLights;                                                         \n\
                                                                                    \n\
float CalcShadowFactor(vec4 LightSpacePos)                                          \n\
{                                                                                   \n\
    vec3 ProjCoords = LightSpacePos.xyz / LightSpacePos.w;                          \n\
    vec2 UVCoords;                                                                  \n\
    UVCoords.x = 0.5 * ProjCoords.x + 0.5;                                          \n\
    UVCoords.y = 0.5 * ProjCoords.y + 0.5;                                          \n\
    float z= 0.5 * ProjCoords.z + 0.5;                                              \n\
    float Depth = texture(gShadowMap, UVCoords).x;                                  \n\
    if (Depth < (z + 0.00001))                                                      \n\
        return 0.5;                                                                 \n\
    else                                                                            \n\
        return 1.0;                                                                 \n\
}                                                                                   \n\
                                                                                    \n\
vec4 CalcLightInternal(BaseLight light, vec3 light_direction, vec3 Normal, float ShadowFactor)       \n\
{                                                                                                    \n\
    vec4 AmbientColor = vec4(light.color, 1.0) * light.ambient_intensity;                            \n\
    float DiffuseFactor = dot(Normal, -light_direction);                                             \n\
                                                                                                     \n\
    vec4 DiffuseColor  = vec4(0, 0, 0, 0);                                                           \n\
    vec4 SpecularColor = vec4(0, 0, 0, 0);                                                           \n\
                                                                                                     \n\
    if (DiffuseFactor > 0) {                                                                         \n\
        DiffuseColor = vec4(light.color, 1.0) * light.diffuse_intensity * DiffuseFactor;             \n\
        vec3 VertexToEye = normalize(gCameraPosition - WorldPos0);                                   \n\
        vec3 LightReflect = normalize(reflect(light_direction, Normal));                             \n\
        float SpecularFactor = dot(VertexToEye, LightReflect);                                       \n\
        if (SpecularFactor > 0 && light.diffuse_intensity > 0) {                                     \n\
            SpecularFactor = pow(SpecularFactor, gMatSpecularPower);                                 \n\
            SpecularColor = vec4(light.color, 1.0) *                                                 \n\
                    gMatSpecularIntensity * SpecularFactor;                                          \n\
        }                                                                                            \n\
    }                                                                                                \n\
                                                                                                     \n\
    return (AmbientColor + ShadowFactor * (DiffuseColor + SpecularColor));                           \n\
}                                                                                                    \n\
                                                                                                     \n\
vec4 CalcDirectionalLight(vec3 Normal)                                                               \n\
{                                                                                                    \n\
    return CalcLightInternal(gDirectionalLight.base, gDirectionalLight.direction, Normal, 1.0);      \n\
}                                                                                                    \n\
                                                                                                     \n\
vec4 CalcPointLight(PointLight light, vec3 Normal)                                                   \n\
{                                                                                                    \n\
    vec3 light_direction = WorldPos0 - light.position;                                 \n\
    float Distance = length(light_direction);                                                        \n\
    light_direction = normalize(light_direction);                                                    \n\
                                                                                                     \n\
    float ShadowFactor = CalcShadowFactor(LightSpacePos);                                            \n\
                                                                                                     \n\
    vec4 color = CalcLightInternal(light.base, light_direction, Normal, ShadowFactor); \n\
    float attentuation = light.atten.constant +                                        \n\
                         light.atten.linear * Distance +                               \n\
                         light.atten.exp * Distance * Distance;                        \n\
                                                                             \n\
    return color / attentuation;                                             \n\
}                                                                            \n\
                                                                             \n\
vec4 CalcSpotLight(SpotLight l, vec3 Normal)                                 \n\
{                                                                            \n\
    vec3 LightToPixel = normalize(WorldPos0 - l.base.position);              \n\
    float SpotFactor = dot(LightToPixel, l.direction);                       \n\
                                                                             \n\
    if (SpotFactor > l.cutoff) {                                             \n\
        vec4 Color = CalcPointLight(l.base, Normal);                         \n\
        return Color * (1.0 - (1.0 - SpotFactor) * 1.0/(1.0 - l.cutoff));    \n\
    }                                                                        \n\
    else {                                                                   \n\
        return vec4(0,0,0,0);                                                \n\
    }                                                                        \n\
}                                                                            \n\
                                                                             \n\
void main()                                                                  \n\
{                                                                            \n\
    vec3 Normal = normalize(Normal0);                                        \n\
    vec4 TotalLight = CalcDirectionalLight(Normal);                          \n\
                                                                             \n\
    for (int i = 0 ; i < gNumPointLights ; i++) {                            \n\
        TotalLight += CalcPointLight(gPointLights[i], Normal);               \n\
    }                                                                        \n\
                                                                             \n\
    for (int i = 0 ; i < gNumSpotLights ; i++) {                             \n\
        TotalLight += CalcSpotLight(gSpotLights[i], Normal);                 \n\
    }                                                                        \n\
                                                                             \n\
    FragColor = texture2D(gSampler, UV) * TotalLight;                        \n\
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
    m_direction_light_location.color = get_uniform_location("gDirectionalLight.base.color");
    m_direction_light_location.ambient_intensity = get_uniform_location("gDirectionalLight.base.ambient_intensity");
    m_direction_light_location.direction = get_uniform_location("gDirectionalLight.direction");
    m_direction_light_location.diffuse_intensity = get_uniform_location("gDirectionalLight.base.diffuse_intensity");

    // Specular light
    m_camera_position_location = get_uniform_location("gCameraPosition");
    m_mat_specular_intensity_location = get_uniform_location("gMatSpecularIntensity");
    m_mat_specular_power_location = get_uniform_location("gMatSpecularPower");

    m_num_point_location = get_uniform_location("gNumPointLights");

    m_light_wvp_location = get_uniform_location("gLightWVP");
    m_shadow_map_location = get_uniform_location("gShadowMap");

    for (unsigned int i = 0 ; i < sizeof(m_point_lights_location)/sizeof(m_point_lights_location[0]) ; i++) {
        char Name[128] = { 0 };
        // TODO: WTF???
        snprintf(Name, sizeof(Name), "gPointLights[%d].base.color", i);
        m_point_lights_location[i].color = get_uniform_location(Name);

        snprintf(Name, sizeof(Name), "gPointLights[%d].base.ambient_intensity", i);
        m_point_lights_location[i].ambient_intensity = get_uniform_location(Name);

        snprintf(Name, sizeof(Name), "gPointLights[%d].position", i);
        m_point_lights_location[i].position = get_uniform_location(Name);

        snprintf(Name, sizeof(Name), "gPointLights[%d].base.diffuse_intensity", i);
        m_point_lights_location[i].diffuse_intensity = get_uniform_location(Name);

        snprintf(Name, sizeof(Name), "gPointLights[%d].atten.constant", i);
        m_point_lights_location[i].atten.constant = get_uniform_location(Name);

        snprintf(Name, sizeof(Name), "gPointLights[%d].atten.linear", i);
        m_point_lights_location[i].atten.linear = get_uniform_location(Name);

        snprintf(Name, sizeof(Name), "gPointLights[%d].atten.exp", i);
        m_point_lights_location[i].atten.exp = get_uniform_location(Name);

        if (m_point_lights_location[i].color == invalid_uniform_location ||
            m_point_lights_location[i].ambient_intensity == invalid_uniform_location ||
            m_point_lights_location[i].position == invalid_uniform_location ||
            m_point_lights_location[i].diffuse_intensity == invalid_uniform_location ||
            m_point_lights_location[i].atten.constant == invalid_uniform_location ||
            m_point_lights_location[i].atten.linear == invalid_uniform_location ||
            m_point_lights_location[i].atten.exp == invalid_uniform_location) {
            return false;
        }
    }

    m_num_spot_location = get_uniform_location("gNumSpotLights");

    for (unsigned int i = 0 ; i < sizeof(m_spot_lights_location)/sizeof(m_spot_lights_location[0]) ; i++) {
        char Name[128] = { 0 };
        // TODO: WTF???
        snprintf(Name, sizeof(Name), "gSpotLights[%d].base.base.color", i);
        m_spot_lights_location[i].color = get_uniform_location(Name);

        snprintf(Name, sizeof(Name), "gSpotLights[%d].base.base.ambient_intensity", i);
        m_spot_lights_location[i].ambient_intensity = get_uniform_location(Name);

        snprintf(Name, sizeof(Name), "gSpotLights[%d].base.position", i);
        m_spot_lights_location[i].position = get_uniform_location(Name);

        snprintf(Name, sizeof(Name), "gSpotLights[%d].direction", i);
        m_spot_lights_location[i].direction = get_uniform_location(Name);

        snprintf(Name, sizeof(Name), "gSpotLights[%d].cutoff", i);
        m_spot_lights_location[i].cutoff = get_uniform_location(Name);

        snprintf(Name, sizeof(Name), "gSpotLights[%d].base.base.diffuse_intensity", i);
        m_spot_lights_location[i].diffuse_intensity = get_uniform_location(Name);

        snprintf(Name, sizeof(Name), "gSpotLights[%d].base.atten.constant", i);
        m_spot_lights_location[i].atten.constant = get_uniform_location(Name);

        snprintf(Name, sizeof(Name), "gSpotLights[%d].base.atten.linear", i);
        m_spot_lights_location[i].atten.linear = get_uniform_location(Name);

        snprintf(Name, sizeof(Name), "gSpotLights[%d].base.atten.exp", i);
        m_spot_lights_location[i].atten.exp = get_uniform_location(Name);

        if (m_spot_lights_location[i].color == invalid_uniform_location ||
            m_spot_lights_location[i].ambient_intensity == invalid_uniform_location ||
            m_spot_lights_location[i].position == invalid_uniform_location ||
            m_spot_lights_location[i].direction == invalid_uniform_location ||
            m_spot_lights_location[i].cutoff == invalid_uniform_location ||
            m_spot_lights_location[i].diffuse_intensity == invalid_uniform_location ||
            m_spot_lights_location[i].atten.constant == invalid_uniform_location ||
            m_spot_lights_location[i].atten.linear == invalid_uniform_location ||
            m_spot_lights_location[i].atten.exp == invalid_uniform_location) {
            return false;
        }
    }

    if (m_direction_light_location.ambient_intensity == invalid_uniform_location ||
        m_wvp_location == invalid_uniform_location ||
        m_world_matrix_location == invalid_uniform_location ||
        m_sampler_location == invalid_uniform_location ||
        m_direction_light_location.color == invalid_uniform_location ||
        m_direction_light_location.diffuse_intensity == invalid_uniform_location ||
        m_direction_light_location.direction == invalid_uniform_location ||
        m_camera_position_location == invalid_uniform_location ||
        m_mat_specular_intensity_location == invalid_uniform_location ||
        m_num_point_location == invalid_uniform_location ||
        m_num_spot_location == invalid_uniform_location ||
        m_shadow_map_location == invalid_uniform_location ||
        m_light_wvp_location == invalid_uniform_location ||
        m_mat_specular_power_location == invalid_uniform_location) {
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

void LightingEffect::set_directional_light(const DirectionalLight & light)
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

void LightingEffect::set_point_lights(const std::vector<PointLight> & lights)
{
    assert(lights.size() <= MAX_POINT_LIGHTS);
    glUniform1i(m_num_point_location, lights.size());

    for (unsigned int i = 0; i < lights.size(); i++) {
        glUniform3f(m_point_lights_location[i].color, lights[i].color.x, lights[i].color.y, lights[i].color.z);
        glUniform1f(m_point_lights_location[i].ambient_intensity, lights[i].ambient_intensity);
        glUniform1f(m_point_lights_location[i].diffuse_intensity, lights[i].diffuse_intensity);
        glUniform3f(m_point_lights_location[i].position, lights[i].position.x, lights[i].position.y, lights[i].position.z);
        glUniform1f(m_point_lights_location[i].atten.constant, lights[i].attentuation.constant);
        glUniform1f(m_point_lights_location[i].atten.linear, lights[i].attentuation.linear);
        glUniform1f(m_point_lights_location[i].atten.exp, lights[i].attentuation.exp);
    }
}

void LightingEffect::set_spot_lights(const std::vector<SpotLight> & lights)
{
    assert(lights.size() <= MAX_SPOT_LIGHTS);
    glUniform1i(m_num_spot_location, lights.size());

    for (unsigned int i = 0; i < lights.size(); i++) {
        glUniform3f(m_spot_lights_location[i].color, lights[i].color.x, lights[i].color.y, lights[i].color.z);
        glUniform1f(m_spot_lights_location[i].ambient_intensity, lights[i].ambient_intensity);
        glUniform1f(m_spot_lights_location[i].diffuse_intensity, lights[i].diffuse_intensity);
        glUniform3f(m_spot_lights_location[i].position, lights[i].position.x, lights[i].position.y, lights[i].position.z);
        glUniform3f(m_spot_lights_location[i].direction, lights[i].direction.x, lights[i].direction.y, lights[i].direction.z);
        glUniform1f(m_spot_lights_location[i].cutoff, glm::cos(glm::radians(lights[i].cutoff)));
        glUniform1f(m_spot_lights_location[i].atten.constant, lights[i].attentuation.constant);
        glUniform1f(m_spot_lights_location[i].atten.linear, lights[i].attentuation.linear);
        glUniform1f(m_spot_lights_location[i].atten.exp, lights[i].attentuation.exp);
    }
}

void LightingEffect::set_texture_unit(unsigned int texture_unit)
{
    glUniform1i(m_sampler_location, texture_unit);
}

void LightingEffect::set_shadow_map_texture(unsigned int texture_unit)
{
    glUniform1i(m_shadow_map_location, texture_unit);
}

void LightingEffect::set_light_wvp(const glm::mat4 & wvp)
{
    glUniformMatrix4fv(m_light_wvp_location, 1, GL_FALSE, &wvp[0][0]);
}
