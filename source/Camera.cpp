#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

Camera::Camera(GLFWwindow * window,
               glm::vec3 position,
               glm::vec3 target,
               glm::vec3 up,
               float range_view,
               float horizontal_angle,
               float vertical_angle,
               float field_of_view,
               float move_speed,
               float mouse_speed)
    : m_window(window)
    , m_position(position)
    , m_range_view(range_view)
    , m_horizontal_angle(horizontal_angle)
    , m_vertical_angle(vertical_angle)
    , m_field_of_view(field_of_view)
    , m_move_speed(move_speed)
    , m_mouse_speed(mouse_speed)
    , m_last_time(glfwGetTime())
    , m_target(target)
    , m_up(up)
{
    glfwGetWindowSize(m_window, &m_screen_width, &m_screen_height);

    glfwSetKeyCallback(window, &Camera::handle_keyboard);
    glfwSetCursorPosCallback(window, &Camera::handle_mouse);
}


struct InternalCamera : public Camera
{
    InternalCamera(GLFWwindow * window)
        : Camera(window)
    {
    }
};


std::shared_ptr<Camera> Camera::get_camera(GLFWwindow * window)
{
    static std::weak_ptr<Camera> camera;
    if (auto sc = camera.lock()) {
        return sc;
    }
    auto sc = std::make_shared<InternalCamera>(window);
    camera = sc;
    return sc;
}


void Camera::handle_keyboard(GLFWwindow * window, int key, int scancode, int action, int mods)
{
    auto camera = get_camera();
    camera->handle_keyboard_i(key, action);
}


void Camera::handle_keyboard_i(int key, int action)
{
    if (action != GLFW_PRESS && action != GLFW_REPEAT) {
        return;
    }

    switch (key) {
    case GLFW_KEY_W:
        m_position += (m_target * m_move_speed);
        break;
    case GLFW_KEY_S:
        m_position -= (m_target * m_move_speed);
        break;
    case GLFW_KEY_D: {
        glm::vec3 right = glm::normalize(glm::cross(m_target, m_up));
        right *= m_move_speed;
        m_position += right;
    } break;
    case GLFW_KEY_A: {
        glm::vec3 left = glm::normalize(glm::cross(m_up, m_target));
        left *= m_move_speed;
        m_position += left;
    } break;
    }
}


void Camera::handle_mouse(GLFWwindow * window, double xpos, double ypos)
{
    auto camera = get_camera();
    camera->handle_mouse_i(xpos, ypos);
}


void Camera::handle_mouse_i(double xpos, double ypos)
{
    glfwSetCursorPos(m_window, m_screen_width / 2, m_screen_height / 2);

    // Compute new orientation
    m_horizontal_angle += m_mouse_speed * float(m_screen_width / 2 - xpos);
    m_vertical_angle += m_mouse_speed * float(-m_screen_height / 2 + ypos);

    const glm::vec3 vaxis(0.0f, 1.0f, 0.0f);

    // rotate the view vector by the horizontal angle around the vertical axis
    glm::vec3 view(1.0f, 0.0f, 0.0f);
    view = glm::normalize(glm::rotateY(view, m_horizontal_angle));

    // rotate the view vector by the vertical angle around the horizontal axis
    glm::vec3 haxis = glm::cross(vaxis, view);
    haxis = glm::normalize(haxis);
    view = glm::rotate(view, m_vertical_angle, haxis);

    m_target = glm::normalize(view);

    m_up = glm::normalize(glm::cross(m_target, haxis));
}
