#include "Technique.h"

#include <assert.h>
#include <iostream>
#include <stdio.h>
#include <string.h>

Technique::~Technique()
{
    for (const auto & shader_id : m_shader_obj_list) {
        glDeleteShader(shader_id);
    }
    if (m_shader_prog != 0) {
        glDeleteProgram(m_shader_prog);
    }
}

bool Technique::init()
{
    m_shader_prog = glCreateProgram();
    if (m_shader_prog == 0) {
        std::cout << "Error creating shader program\n";
        return false;
    }
    assert(m_shader_prog != 0 && "Error creating shader program");
    return true;
}

bool Technique::add_shader(GLenum shader_type, const char * shader_code)
{
    GLuint shader_obj = glCreateShader(shader_type);

    if (shader_obj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", shader_type);
        return false;
    }

    m_shader_obj_list.emplace_back(shader_obj);

    GLint lengths;
    lengths = strlen(shader_code);
    glShaderSource(shader_obj, 1, (const GLchar * const *)&shader_code, &lengths);

    glCompileShader(shader_obj);

    GLint success;
    glGetShaderiv(shader_obj, GL_COMPILE_STATUS, &success);

    if (!success) {
        GLchar info_log[1024];
        glGetShaderInfoLog(shader_obj, 1024, NULL, info_log);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", shader_type, info_log);
        return false;
    }

    glAttachShader(m_shader_prog, shader_obj);

    return true;
}

bool Technique::finalize()
{
    GLint success = 0;
    GLchar ErrorLog[1024] = {0};

    glLinkProgram(m_shader_prog);

    glGetProgramiv(m_shader_prog, GL_LINK_STATUS, &success);
    if (success == 0) {
        glGetProgramInfoLog(m_shader_prog, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        return false;
    }

    glValidateProgram(m_shader_prog);
    glGetProgramiv(m_shader_prog, GL_VALIDATE_STATUS, &success);
    if (success == 0) {
        glGetProgramInfoLog(m_shader_prog, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        return false;
    }

    for (const auto & shader_id : m_shader_obj_list) {
        glDeleteShader(shader_id);
    }
    m_shader_obj_list.clear();

    return true;
}

void Technique::enable()
{
    glUseProgram(m_shader_prog);
}

GLint Technique::get_uniform_location(const char * uniform_name)
{
    GLint location = glGetUniformLocation(m_shader_prog, uniform_name);

    if (location == 0xFFFFFFFF) {
        fprintf(stderr, "Warning! Unable to get the location of uniform '%s'\n", uniform_name);
    }

    return location;
}
