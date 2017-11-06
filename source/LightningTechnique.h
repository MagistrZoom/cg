#pragma once

#include "Technique.h"

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

class LightingTechnique : public Technique
{
public:
    LightingTechnique() = default;
    bool init() override;

    void set_wvp(const glm::mat4 & wvp);
    void set_world_matrix(const glm::mat4 & wvp);
    void set_texture_unit(unsigned int texture_unit);
    void set_directional_light(const DirectionLight & light);

private:
    GLuint m_wvp_location;
    GLuint m_world_matrix_location;
    GLuint m_sampler_location;
    struct
    {
        GLuint color;
        GLuint ambient_intensity;
        GLuint direction;
        GLuint diffuse_intensity;
    } m_direction_light_location;
};
