#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

Camera::Camera(GLFWwindow * window,
               Vector3f position,
               Vector3f target,
               Vector3f up,
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
    case GLFW_KEY_UP:
        m_position += (m_target * m_move_speed);
        break;
    case GLFW_KEY_DOWN:
        m_position -= (m_target * m_move_speed);
        break;
    case GLFW_KEY_RIGHT: {
        Vector3f Right = m_up.Cross(m_target);
        Right.Normalize();
        Right *= m_move_speed;
        m_position += Right;
    } break;
    case GLFW_KEY_LEFT: {
        Vector3f Left = m_target.Cross(m_up);
        Left.Normalize();
        Left *= m_move_speed;
        m_position += Left;
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
    m_horizontal_angle += m_mouse_speed * float(-m_screen_width / 2 + xpos);
    m_vertical_angle += m_mouse_speed * float(-m_screen_height / 2 + ypos);

    const Vector3f Vaxis(0.0f, 1.0f, 0.0f);

    // Rotate the view vector by the horizontal angle around the vertical axis
    Vector3f View(1.0f, 0.0f, 0.0f);
    View.Rotate(m_horizontal_angle, Vaxis);
    View.Normalize();

    // Rotate the view vector by the vertical angle around the horizontal axis
    Vector3f Haxis = Vaxis.Cross(View);
    Haxis.Normalize();
    View.Rotate(m_vertical_angle, Haxis);

    m_target = View;
    m_target.Normalize();

    m_up = m_target.Cross(Haxis);
    m_up.Normalize();
}
