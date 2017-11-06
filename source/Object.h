#pragma once

#include "LightningTechnique.h"

// Include GLM
#include <glm/glm.hpp>

#include <memory>

class Object : public LightingTechnique
{
public:
    Object() = default;
    ~Object();
    bool init() override;

    /* Technique::enable must be called before render */
    void render();

private:
    using super = LightingTechnique;
    GLuint m_mvp_location;
    GLuint m_sampler_location;

    GLuint m_vertex_array_id;

    GLuint m_vertexbuffer;
    GLuint m_uvbuffer;
    GLuint m_normalbuffer;

    std::vector<glm::vec3> m_vertices;
    std::vector<glm::vec2> m_uvs;
    std::vector<glm::vec3> m_normals;
};
