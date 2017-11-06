#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/transform.hpp>

class Pipeline
{
public:
    Pipeline()
    {
        m_scale_matrix = glm::mat4(1.0f);
        m_world_matrix = glm::mat4(0.0f);
        m_rotate_matrix = glm::mat4(1.0f);
    }

    void scale(float x, float y, float z)
    {
        m_scale_matrix = glm::scale(glm::vec3(x, y, z));
    }

    void world_position(float x, float y, float z)
    {
        m_world_matrix = glm::translate(glm::vec3(x, y, z));
    }

    void rotate(float angle, const glm::vec3 & vector)
    {
        m_rotate_matrix = glm::rotate(angle, vector);
    }

    void set_perspective(float fov, float width, float height, float znear, float zfar)
    {
        m_perspective_matrix = glm::perspective(glm::radians(fov), width / height, znear, zfar);
    }

    void set_camera(const glm::vec3 & position, const glm::vec3 & target, const glm::vec3 & up)
    {
        m_camera = glm::lookAt(position, position + target, up);
    }

    const glm::mat4 & get_wvp();

    const glm::mat4 & get_world();

private:
    glm::mat4 m_scale_matrix;
    glm::mat4 m_world_matrix;
    glm::mat4 m_rotate_matrix;

    glm::mat4 m_perspective_matrix;

    glm::mat4 m_camera;

    glm::mat4 m_wvp;
    glm::mat4 m_world;
};
