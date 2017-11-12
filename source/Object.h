#pragma once

#include "LightningEffect.h"
#include "Texture.h"

// Include GLM
#include <glm/glm.hpp>

class Object
{
public:
    Object(const std::string & object_filename);
    ~Object();

    bool init();

    void render();

private:
    using super = LightingEffect;

    std::string m_object_filename;
    GLuint m_texture_index;

    GLuint m_vertex_array_id;

    GLuint m_vertexbuffer;
    GLuint m_uvbuffer;
    GLuint m_normalbuffer;

    std::vector<glm::vec3> m_vertices;
    std::vector<glm::vec2> m_uvs;
    std::vector<glm::vec3> m_normals;
};
