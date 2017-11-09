#pragma once

#include <GL/glew.h>
#include <vector>

class Effect
{
public:
    Effect() = default;
    virtual ~Effect();
    virtual bool init();
    void enable();

protected:
    bool add_shader(GLenum shader_type, const char * shader_code);
    bool finalize();
    GLint get_uniform_location(const char * uniform_name);

private:
    GLuint m_shader_prog;

    using ShaderObjectList = std::vector<GLuint>;
    ShaderObjectList m_shader_obj_list;
};
