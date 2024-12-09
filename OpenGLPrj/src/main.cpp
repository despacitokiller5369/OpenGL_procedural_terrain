// Local Headers
#include "OpenGLPrj.hpp"
#include "utils.hpp"
#include "shader.hpp"
#include "perlin.hpp"
#include "camera.hpp"
#include "terrain.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

int main() {
    GLFWwindow* window;
    if (!init_opengl(window, window_width, window_height, "OpenGLPrj")) return EXIT_FAILURE;

    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
    float delta_time = 0.0f;
    float last_frame = 0.0f;

    configure_opengl(window, camera);

    // Terrain
    int terrain_width = 512, terrain_height = 512;
    float terrain_scale = 0.1f, terrain_displacement = 20.0f;
    float noise_scale = 10, noise_octaves = 4, noise_persistence = 0.5f;
    Terrain terrain(terrain_width, terrain_height, terrain_scale, terrain_displacement, noise_scale, noise_octaves, noise_persistence);
    terrain.upload_to_gpu();

    const std::string vertex_shader_path = std::string(project_source_dir) + "/shaders/vertex.glsl";
    const std::string fragment_shader_path = std::string(project_source_dir) + "/shaders/fragment.glsl";

    Shader shader(vertex_shader_path.c_str(), fragment_shader_path.c_str());
    shader.use();
    shader.set_int("texture1", 0);

    // Rendering loop
    while (!glfwWindowShouldClose(window)) {
        float current_frame = (float)glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        process_input(window, camera, delta_time);

        // Background fill color
        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Activate shader
        shader.use();

        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)window_width / (float)window_height, 0.1f, 100.0f);
        shader.set_mat4("projection", projection);

        glm::mat4 view = camera.get_view_matrix();
        shader.set_mat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        shader.set_mat4("model", model);

        terrain.render();

        // Flip buffers and draw
        glfwSwapBuffers(window);
        glfwPollEvents();
    }   
    glfwTerminate();
    return EXIT_SUCCESS;
}