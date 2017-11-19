#include "Object.h"
#include "Camera.h"
#include "pipeline.h"
#include "ShadowMapEffect.h"
#include "ShadowMapFBO.h"
#include "LightningEffect.h"

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include <iostream>

std::ostream & operator << (std::ostream & strm, const glm::vec3 & vec)
{
    strm << '(' << vec.x << ", " << vec.y << ", " << vec.z << ')';
    return strm;
}

int main(void)
{
    // Initialise GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow * window;
    // Open a window and create its OpenGL context
    window = glfwCreateWindow(1920, 1080, "Tutorial 07 - Model Loading", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1920 / 2, 1080 / 2);

    // Dark black background
    glClearColor(0.376f, 0.235f, 0.149f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    auto camera = Camera::get_camera(window);

    /*
     * Object stuff
     */
    Texture home_texture(GL_TEXTURE_2D, "models/farm_house/texture.jpg");
    Object home("models/farm_house/house2.obj");
    assert(home.init());

    Texture quad_texture(GL_TEXTURE_2D, "models/quad/test.png");
    Object surface("models/quad/quad.obj");
    assert(surface.init());

    Texture earth(GL_TEXTURE_2D, "models/sphere/BlueMarble-2001-2002.jpg");
    Object sphere("models/sphere/sphere.obj");
    assert(sphere.init());

    /*
     * Light stuff
     */

    LightingEffect lightning_effect; 
    assert(lightning_effect.init());

    std::vector<SpotLight> sl;
    SpotLight s;
    sl.emplace_back(s);
    sl.emplace_back(s);
    sl[0].diffuse_intensity = 15.0f;
    sl[0].color = glm::vec3(1.0f, 1.0f, 1.0f);
    sl[0].attentuation.linear = 0.1f;
    sl[0].cutoff = 45.0f;
    sl[0].position = glm::vec3(19.2437f, 23.7128f, -41.0067f);
    sl[0].direction = glm::vec3(0.287091f, -0.466079f, 0.836869f);

    sl[1].diffuse_intensity = 25.0f;
    sl[1].color = glm::vec3(1.0f, 1.0f, 1.0f);
    sl[1].attentuation.linear = 0.1f;
    sl[1].cutoff = 10.0f;

    /*
     * Shadow stuff
     */
    ShadowMapFBO shadow_map_fbo;
    assert(shadow_map_fbo.init(1920, 1080));
    ShadowMapEffect shadow_map;
    assert(shadow_map.init());
    shadow_map.set_texture_unit(3);
    lightning_effect.set_shadow_map_texture(3);

    do {
        Pipeline p;
        p.set_perspective(45.0f, 1920, 1080, 0.1f, 500.0f);
        p.set_camera(sl[0].position, sl[0].direction, glm::vec3(0.0f, 1.0f, 0.0f));

        /* 1 step - make shadow */
        shadow_map_fbo.write_bind();
        glClear(GL_DEPTH_BUFFER_BIT);
        shadow_map.enable();

        p.world_position(0.0f, 0.0f, 0.0f);
        p.rotate(glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        shadow_map.set_wvp(p.get_wvp());
        p.rotate();
        home.render();

        p.world_position(29.6273, 5.8991, 17.2165);
        shadow_map.set_wvp(p.get_wvp());
        sphere.render();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        /* 2 step - make objects */

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightning_effect.enable();
        shadow_map_fbo.read_bind(GL_TEXTURE3);

        const auto & position = camera->get_position();
        const auto & target = camera->get_target();
        const auto & up = camera->get_up();

        p.world_position(0.0f, 0.0f, 0.0f);
        p.rotate(glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        p.set_camera(position, target, up);
        lightning_effect.set_wvp(p.get_wvp());
        lightning_effect.set_world_matrix(p.get_world());
        lightning_effect.set_camera_position(position);
        p.set_camera(sl[0].position, sl[0].direction, glm::vec3(0.0f, 1.0f, 0.0f));
        lightning_effect.set_light_wvp(p.get_wvp());
        lightning_effect.set_specular_intensity(0.0f);
        lightning_effect.set_specular_power(4.0f);
        lightning_effect.set_spot_lights(sl);
        lightning_effect.set_texture_unit(0);
        lightning_effect.set_shadow_map_texture(3);
        p.rotate();

        home_texture.bind(GL_TEXTURE0);
        home.render();

        p.world_position(29.6273, 5.8991, 17.2165);
        p.set_camera(position, target, up);
        lightning_effect.set_wvp(p.get_wvp());
        lightning_effect.set_world_matrix(p.get_world());
        lightning_effect.set_camera_position(position);
        p.set_camera(sl[0].position, sl[0].direction, glm::vec3(0.0f, 1.0f, 0.0f));
        lightning_effect.set_light_wvp(p.get_wvp());
        lightning_effect.set_specular_intensity(0.0f);
        lightning_effect.set_specular_power(4.0f);
        lightning_effect.set_spot_lights(sl);
        lightning_effect.set_texture_unit(2);
        lightning_effect.set_shadow_map_texture(3);

        earth.bind(GL_TEXTURE2);
        sphere.render();


        p.scale(60.0f, 60.0f, 60.0f);
        p.world_position(0.0f, 0.0f, 0.0f);
        p.set_camera(position, target, up);
        lightning_effect.set_wvp(p.get_wvp());
        lightning_effect.set_world_matrix(p.get_world());
        p.set_camera(sl[0].position, sl[0].direction, glm::vec3(0.0f, 1.0f, 0.0f));
        lightning_effect.set_light_wvp(p.get_wvp());
        lightning_effect.set_texture_unit(1);
        quad_texture.bind(GL_TEXTURE1);
        surface.render();


        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}
