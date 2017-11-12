#pragma once

#include <glm/glm.hpp>
// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>



class ShadowMapFBO
{
public:
    ShadowMapFBO();

    ~ShadowMapFBO();

    bool init(unsigned int window_width, unsigned int window_height);

    void write_bind();

    void read_bind(GLenum texture_uint);

private:
    GLuint m_fbo;
    GLuint m_shadow_map;
};
