#pragma once

#include "LightningTechnique.h"
#include "Texture.h"

// Include GLM
#include <glm/glm.hpp>

#include <memory>

class Object : public LightingTechnique
{
public:
    Object(const std::string & object_filename,
           Texture & texture,
           GLuint texture_index);
    ~Object();

    bool init() override;

    void render();

private:
    using super = LightingTechnique;

    std::string m_object_filename;
    Texture & m_texture;
    GLuint m_texture_index;

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
