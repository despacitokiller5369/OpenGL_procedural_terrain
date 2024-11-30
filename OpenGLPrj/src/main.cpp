// Local Headers
#include "OpenGLPrj.hpp"
#include "utils.hpp"
#include "shader.hpp"
#include "perlin.hpp"
#include "camera.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float last_x = 0.0f;
float last_y = 0.0f;
bool first_mouse = true;

float delta_time = 0.0f;
float last_frame = 0.0f;

int main() {
    GLFWwindow* window;
    if (!init_opengl(window, window_width, window_height, "OpenGLPrj")) return EXIT_FAILURE;
    
    glfwSetCursorPosCallback(window, glfw_mouse_callback);
    glfwSetScrollCallback(window, glfw_scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glEnable(GL_DEPTH_TEST);


    int width = 512, height = 512;
    float scale = 10.0f;
    std::vector<float> noise = generate_perlin_noise(width, height, scale, 4, 0.5f);
    //std::vector<float> noise = generate_perlin_noise(width, height, scale);
    //apply_gaussian_blur(noise, width, height);
    //apply_gaussian_blur(noise, width, height, 5, 1.0f);

    GLuint perlin_texture;
    glGenTextures(1, &perlin_texture);
    glBindTexture(GL_TEXTURE_2D, perlin_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, noise.data());
    glGenerateMipmap(GL_TEXTURE_2D);

    const int terrain_width = 512;
    const int terrain_height = 512;
    const float terrain_scale = 0.1f;
    float dispacement = 20.0f;

    std::vector<float> quad_vertices;
    std::vector<unsigned int> quad_indices;

    for (int z = 0; z < terrain_height; z++) {
        for (int x = 0; x < terrain_width; x++) {
            float height = noise[z * terrain_width + x] * terrain_scale * dispacement;

            quad_vertices.push_back(x * terrain_scale);
            quad_vertices.push_back(height);
            quad_vertices.push_back(z * terrain_scale);

            quad_vertices.push_back(0.2f + height);
            quad_vertices.push_back(0.7f);
            quad_vertices.push_back(0.2f);

            quad_vertices.push_back((float)x / terrain_width);
            quad_vertices.push_back((float)z / terrain_height);
        }
    }

    for (int z = 0; z < terrain_height - 1; z++) {
        for (int x = 0; x < terrain_width - 1; x++) {
            int top_left = z * terrain_width + x;
            int top_right = top_left + 1;
            int bottom_left = (z + 1) * terrain_width + x;
            int bottom_right = bottom_left + 1;

            quad_indices.push_back(top_left);
            quad_indices.push_back(bottom_left);
            quad_indices.push_back(top_right);

            quad_indices.push_back(top_right);
            quad_indices.push_back(bottom_left);
            quad_indices.push_back(bottom_right);
        }
    }

    GLuint vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, quad_vertices.size() * sizeof(float), quad_vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, quad_indices.size() * sizeof(unsigned int), quad_indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

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

        process_input(window);

        // Background fill color
        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, perlin_texture);

        // Activate shader
        shader.use();

        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)window_width / (float)window_height, 0.1f, 100.0f);
        shader.set_mat4("projection", projection);

        glm::mat4 view = camera.get_view_matrix();
        shader.set_mat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        shader.set_mat4("model", model);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, quad_indices.size(), GL_UNSIGNED_INT, 0);

        // Flip buffers and draw
        glfwSwapBuffers(window);
        glfwPollEvents();
    }   
    glfwTerminate();
    return EXIT_SUCCESS;
}

void glfw_mouse_callback(GLFWwindow* window, double xpos_input, double ypos_input) {
    float xpos = (float)xpos_input;
    float ypos = (float)ypos_input;

    if (first_mouse) {
        last_x = xpos;
        last_y = ypos;
        first_mouse = false;
    }

    float xoffset = xpos - last_x;
    float yoffset = last_y - ypos;

    last_x = xpos;
    last_y = ypos;

    camera.process_mouse_movement(xoffset, yoffset);
}

void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.process_mouse_scroll((float)yoffset);
}

void process_input(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.process_keyboard(Camera_Movement::FORWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.process_keyboard(Camera_Movement::BACKWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.process_keyboard(Camera_Movement::LEFT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.process_keyboard(Camera_Movement::RIGHT, delta_time);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.process_keyboard(Camera_Movement::UP, delta_time);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.process_keyboard(Camera_Movement::DOWN, delta_time);  
}