#pragma once

#include "Effect.h"

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

struct DirectionLight
{
    glm::vec3 color;
    float ambient_intensity;
    glm::vec3 direction;
    float diffuse_intensity;
};

class LightingEffect : public Effect
{
public:
    LightingEffect() = default;
    bool init() override;

    void set_wvp(const glm::mat4 & wvp);
    void set_world_matrix(const glm::mat4 & wvp);
    void set_directional_light(const DirectionLight & light);
    void set_camera_position(const glm::vec3 & pos);
    void set_specular_intensity(float instensity);
    void set_specular_power(float power);

private:
    GLuint m_wvp_location;
    GLuint m_world_matrix_location;
    GLuint m_sampler_location;

    GLuint m_camera_position_location;
    GLuint m_mat_specular_intensity_location;
    GLuint m_mat_specular_power_location;

    struct
    {
        GLuint color;
        GLuint ambient_intensity;
        GLuint direction;
        GLuint diffuse_intensity;
    } m_direction_light_location;
};
