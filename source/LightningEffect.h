#pragma once

#include "Effect.h"

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#define MAX_POINT_LIGHTS 3
#define MAX_SPOT_LIGHTS 2

struct BaseLight
{
    glm::vec3 color;
    float ambient_intensity;
    float diffuse_intensity;

    BaseLight()
    {
        color = glm::vec3(0.0f, 0.0f, 0.0f);
        ambient_intensity = 0.0f;
        diffuse_intensity = 0.0f;
    }
};

struct DirectionalLight : public BaseLight
{
    glm::vec3 direction;

    DirectionalLight()
    {
        direction = glm::vec3(0.0f, 0.0f, 0.0f);
    }
};

struct PointLight : public BaseLight
{
    glm::vec3 position;

    struct
    {
        float constant;
        float linear;
        float exp;
    } attentuation;

    PointLight()
    {
        position = glm::vec3(0.0f, 0.0f, 0.0f);
        attentuation.constant = 1.0f;
        attentuation.linear = 0.0f;
        attentuation.exp = 0.0f;
    }
};

struct SpotLight : public PointLight
{
    glm::vec3 direction;
    float cutoff;

    SpotLight()
    {
        direction = glm::vec3(0.0f, 0.0f, 0.0f);
        cutoff = 0.0f;
    }
};

class LightingEffect : public Effect
{
public:
    LightingEffect() = default;
    bool init() override;

    void set_wvp(const glm::mat4 & wvp);
    void set_world_matrix(const glm::mat4 & wvp);
    void set_directional_light(const DirectionalLight & light);
    void set_camera_position(const glm::vec3 & pos);
    void set_specular_intensity(float instensity);
    void set_specular_power(float power);

    void set_point_lights(const std::vector<PointLight> & lights);
    void set_spot_lights(const std::vector<SpotLight> & lights);

    void set_texture_unit(unsigned int texture_unit);

    void set_shadow_map_texture(unsigned int texture_unit);
    void set_light_wvp(const glm::mat4 & wvp);

private:
    GLuint m_wvp_location;
    GLuint m_world_matrix_location;
    GLuint m_sampler_location;

    GLuint m_camera_position_location;
    GLuint m_mat_specular_intensity_location;
    GLuint m_mat_specular_power_location;

    GLuint m_shadow_map_location;
    GLuint m_light_wvp_location;

    struct
    {
        GLuint color;
        GLuint ambient_intensity;
        GLuint direction;
        GLuint diffuse_intensity;
    } m_direction_light_location;

    struct PointLightLocation
    {
        GLuint color;
        GLuint ambient_intensity;
        GLuint diffuse_intensity;
        GLuint position;
        struct
        {
            GLuint constant;
            GLuint linear;
            GLuint exp;
        } atten;
    };

    struct SpotLightLocation
    {
        GLuint color;
        GLuint ambient_intensity;
        GLuint diffuse_intensity;
        GLuint position;
        GLuint direction;
        GLuint cutoff;
        struct {
            GLuint constant;
            GLuint linear;
            GLuint exp;
        } atten;
    };

    GLuint m_num_point_location;
    PointLightLocation m_point_lights_location[MAX_POINT_LIGHTS];
    GLuint m_num_spot_location;
    SpotLightLocation m_spot_lights_location[MAX_SPOT_LIGHTS];
};
