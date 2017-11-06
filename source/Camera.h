// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>

#include "math_3d.h"

#include <memory>

// Camera operator class
//
// Calculates camera position, perspective etc.
// handles peripherial devices
//
// Need to divide this monster into operator and camera entities

class Camera
{
protected:
    Camera(GLFWwindow * window,
           Vector3f position = Vector3f(0.0f, 15.0f, 60.0f),
           Vector3f target = Vector3f(0.0f, 0.0f, 1.0f),
           Vector3f up = Vector3f(0.0f, 1.0f, 0.0f),
           float range_view = 500.0f,
           float horizontal_angle = M_PI, // radians
           float vertical_angle = 0.0f,   // radians
           float field_of_view = 45.0f,   // degrees
           float move_speed = 0.5f,      // units per press
           float mouse_speed = 0.02f);

public:
    static std::shared_ptr<Camera> get_camera(GLFWwindow * window = nullptr);

    const Vector3f & get_position()
    {
        return m_position;
    }
    const Vector3f & get_target()
    {
        return m_target;
    }
    const Vector3f & get_up()
    {
        return m_up;
    }

private:
    static void handle_keyboard(GLFWwindow * window, int key, int scancode, int action, int mods);
    void handle_keyboard_i(int key, int action);
    static void handle_mouse(GLFWwindow * window, double xpos, double ypos);
    void handle_mouse_i(double xpos, double ypos);

    GLFWwindow * m_window;

    Vector3f m_position;
    Vector3f m_target;
    Vector3f m_up;

    int m_screen_width;
    int m_screen_height;

    float m_horizontal_angle;
    float m_vertical_angle;
    float m_field_of_view;
    float m_range_view;

    float m_move_speed;
    float m_mouse_speed;

    double m_last_time;
};
