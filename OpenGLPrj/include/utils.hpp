#pragma once

#include "camera.hpp"

#include <GLFW/glfw3.h>
#include <string>

bool init_opengl(GLFWwindow*& window, int window_width, int window_height, const char* title);
void configure_opengl(GLFWwindow* window, Camera& camera);
bool restart_gl_log(const std::string& log_file_path);
bool gl_log(const std::string& log_file_path, const char* message, ...);
bool gl_log_err(const std::string& log_file_path, const char* message, ...);
void glfw_error_callback(int error, const char* description);
void glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height);
void glfw_mouse_callback(GLFWwindow* window, double xpos, double ypos);
void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void process_input(GLFWwindow* window, Camera& camera, float delta_time);
