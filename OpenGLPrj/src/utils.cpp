#include "utils.hpp"

#include <glad/glad.h>
#include <fstream>
#include <ctime>
#include <cstdarg>
#include <sstream>

bool init_opengl(GLFWwindow*& window, int window_width, int window_height, const char* title) {
    if (!glfwInit()) {
        gl_log_err("log.log", "Failed to initialize GLFW");
        return false;
    }
    //gl_log("log.log", "OpenGL version: %s\n", glGetString(GL_VERSION));
    glfwSetErrorCallback(glfw_error_callback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    window = glfwCreateWindow(window_width, window_height, title, NULL, NULL);
    if (!window) {
        gl_log_err("log.log", "Failed to create GLFW window");
        glfwTerminate();
        return false;
    }

    glfwSetFramebufferSizeCallback(window, glfw_framebuffer_size_callback);
    glfwMakeContextCurrent(window);
    
    if (!gladLoadGL()) {
        gl_log_err("log.log", "Failed to initialize GLAD");
        glfwTerminate();
        return false;
    }
    return true;
}

void configure_opengl(GLFWwindow* window, Camera& camera) {
    glfwSetWindowUserPointer(window, &camera);
    glfwSetCursorPosCallback(window, glfw_mouse_callback);
    glfwSetScrollCallback(window, glfw_scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glEnable(GL_DEPTH_TEST);
}

bool restart_gl_log(const std::string& log_file_path) {
    std::ofstream log_file(log_file_path.c_str(), std::ios::out | std::ios::trunc);
    if (!log_file.is_open()) {
        fprintf(stderr, "ERROR: Could not open log file %s for writing\n", log_file_path.c_str());
        return false;
    }
    std::time_t now = std::time(nullptr);
    std::string date = std::ctime(&now);
    log_file << "GL_LOG_FILE log. Log started: " << date << std::endl;
    log_file.close();
    return true;
}

bool gl_log(const std::string& log_file_path, const char* message, ...) {
    std::ofstream log_file(log_file_path.c_str(), std::ios::out | std::ios::app);
    if (!log_file.is_open()) {
        fprintf(stderr, "ERROR: Could not open log file %s for writing\n", log_file_path.c_str());
        return false;
    }
    va_list args;
    va_start(args, message);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), message, args);
    va_end(args);
    log_file << buffer << std::endl;
    log_file.close();
    return true;
}

bool gl_log_err(const std::string& log_file_path, const char* message, ...) {
    std::ofstream log_file(log_file_path.c_str(), std::ios::out | std::ios::app);
    if (!log_file.is_open()) {
        fprintf(stderr, "ERROR: Could not open log file %s for writing\n", log_file_path.c_str());
        return false;
    }
    va_list args;
    va_start(args, message);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), message, args);
    va_end(args);
    log_file << "ERROR: " << buffer << std::endl;
    va_start(args, message);
    vfprintf(stderr, message, args);
    va_end(args);
    log_file.close();
    return true;
}

void glfw_error_callback(int error, const char* description) {
    gl_log_err("log.log", "ERROR: GLFW error %i: %s\n", error, description);
}

void glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    //window_width = width;
    //window_height = height;
    gl_log("log.log", "Framebuffer size: %i x %i\n", width, height);
    glViewport(0, 0, width, height);
    gl_log("log.log", "Viewport: %i x %i\n", width, height);
}

void glfw_mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    Camera *camera = (Camera*)glfwGetWindowUserPointer(window);
    if (!camera) return;

    static bool first_mouse = true;
    static float last_x = 0.0f;
    static float last_y = 0.0f;

    float xpos_output = (float)xpos;
    float ypos_output = (float)ypos;

    if (first_mouse) {
        last_x = xpos_output;
        last_y = ypos_output;
        first_mouse = false;
    }

    float xoffset = xpos_output - last_x;
    float yoffset = last_y - ypos_output;

    last_x = xpos_output;
    last_y = ypos_output;

    camera->process_mouse_movement(xoffset, yoffset);
}

void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    Camera *camera = (Camera*)glfwGetWindowUserPointer(window);
    if (!camera) return;
    camera->process_mouse_scroll((float)yoffset);
}

void process_input(GLFWwindow* window, Camera& camera, float delta_time) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            camera.process_keyboard(Camera_Movement::FORWARD, true, delta_time);
        camera.process_keyboard(Camera_Movement::FORWARD, false, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            camera.process_keyboard(Camera_Movement::BACKWARD, true, delta_time);
        camera.process_keyboard(Camera_Movement::BACKWARD, false, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            camera.process_keyboard(Camera_Movement::LEFT, true, delta_time);
        camera.process_keyboard(Camera_Movement::LEFT, false, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            camera.process_keyboard(Camera_Movement::RIGHT, true, delta_time);
        camera.process_keyboard(Camera_Movement::RIGHT, false, delta_time);
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            camera.process_keyboard(Camera_Movement::UP, true, delta_time);
        camera.process_keyboard(Camera_Movement::UP, false, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            camera.process_keyboard(Camera_Movement::DOWN, true, delta_time);
        camera.process_keyboard(Camera_Movement::DOWN, false, delta_time);
    }
}