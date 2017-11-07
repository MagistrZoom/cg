// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>

#include <memory>

// Camera operator class
//
// Calculates camera position, perspective etc.
// handles peripherial devices
//

class Camera
{
protected:
    Camera(GLFWwindow * window,
           glm::vec3 position = glm::vec3(-80.0f, 40.0f, 0.0f),
           glm::vec3 target = glm::vec3(1.0f, 0.0f, 0.0f),
           glm::vec3 up = glm::vec3(0.5f, 0.5f, 0.0f),
           float range_view = 500.0f,
           float horizontal_angle = 0, // radians
           float vertical_angle = 0.0f,   // radians
           float field_of_view = 45.0f,   // degrees
           float move_speed = 1.0f,      // units per press
           float mouse_speed = 0.005f);

public:
    static std::shared_ptr<Camera> get_camera(GLFWwindow * window = nullptr);

    const glm::vec3 & get_position()
    {
        return m_position;
    }
    const glm::vec3 & get_target()
    {
        return m_target;
    }
    const glm::vec3 & get_up()
    {
        return m_up;
    }

private:
    static void handle_keyboard(GLFWwindow * window, int key, int scancode, int action, int mods);
    void handle_keyboard_i(int key, int action);
    static void handle_mouse(GLFWwindow * window, double xpos, double ypos);
    void handle_mouse_i(double xpos, double ypos);

    GLFWwindow * m_window;

    glm::vec3 m_position;
    glm::vec3 m_target;
    glm::vec3 m_up;

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
