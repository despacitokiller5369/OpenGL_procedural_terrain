// Local Headers
#include "OpenGLPrj.hpp"
#include "utils.hpp"
#include "shader.hpp"
#include "perlin.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <string>

int main() {
    GLFWwindow* window;
    if (!init_opengl(window, window_width, window_height, "OpenGLPrj")) return EXIT_FAILURE;

    int width = 512, height = 512;
    float scale = 100.0f;
    //std::vector<float> noise = generate_perlin_noise(width, height, scale, 4, 0.5f);
    std::vector<float> noise = generate_perlin_noise(width, height, scale);
    //apply_gaussian_blur(noise, width, height);
    //apply_gaussian_blur(noise, width, height, 5, 1.0f);

    GLuint perlin_texture;
    glGenTextures(1, &perlin_texture);
    glBindTexture(GL_TEXTURE_2D, perlin_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, noise.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    float quad_vertices[] = {
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f
    };
    GLuint quad_indices[] = {0, 1, 2, 2, 3, 0};

    GLuint vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices), quad_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    const std::string vertex_shader_path = std::string(project_source_dir) + "/shaders/vertex.glsl";
    const std::string fragment_shader_path = std::string(project_source_dir) + "/shaders/fragment.glsl";

    Shader shader(vertex_shader_path.c_str(), fragment_shader_path.c_str());

    // Rendering loop
    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Background fill color
        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        glBindVertexArray(vao);
        glBindTexture(GL_TEXTURE_2D, perlin_texture);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Flip buffers and draw
        glfwSwapBuffers(window);
        glfwPollEvents();
    }   
    glfwTerminate();
    return EXIT_SUCCESS;
}
