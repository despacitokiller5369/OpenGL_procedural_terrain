#include "shader.hpp"
#include "utils.hpp"

#include <string>
#include <fstream>
#include <sstream>

GLuint create_shader_program_from_strings(const char* vertex_source, const char* fragment_source) {
    GLuint shader_program = glCreateProgram();
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    { // Check for errors creating the vertex shader
        glShaderSource(vertex_shader, 1, &vertex_source, NULL);
        glCompileShader(vertex_shader);
        int success;
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char info_log[512];
            glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
            gl_log_err("log.log", "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", info_log);
            glDeleteShader(vertex_shader);
            glDeleteShader(fragment_shader);
            glDeleteProgram(shader_program);
            return 0;
        }
    }

    { // Check for errors creating the fragment shader
        glShaderSource(fragment_shader, 1, &fragment_source, NULL);
        glCompileShader(fragment_shader);
        int success;
        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char info_log[512];
            glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
            gl_log_err("log.log", "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", info_log);
            glDeleteShader(vertex_shader);
            glDeleteShader(fragment_shader);
            glDeleteProgram(shader_program);
            return 0;
        }
    }

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);

    { // Check for errors linking the program
        glLinkProgram(shader_program);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        int success;
        glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
        if (!success) {
            char info_log[512];
            glGetProgramInfoLog(shader_program, 512, NULL, info_log);
            gl_log_err("log.log", "ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", info_log);
            glDeleteShader(vertex_shader);
            glDeleteShader(fragment_shader);
            glDeleteProgram(shader_program);
            return 0;
        }
    }

    return shader_program;
}

GLuint create_shader_program_from_files(const char* vertex_path, const char* fragment_path) {
    std::ifstream vertex_file(vertex_path);
    std::ifstream fragment_file(fragment_path);
    if (!vertex_file.is_open() || !fragment_file.is_open()) {
        gl_log_err("log.log", "ERROR: Could not open shader files %s and/or %s\n", vertex_path, fragment_path);
        return 1;
    }
    std::stringstream vertex_stream;
    std::stringstream fragment_stream;
    vertex_stream << vertex_file.rdbuf();
    fragment_stream << fragment_file.rdbuf();
    return create_shader_program_from_strings(vertex_stream.str().c_str(), fragment_stream.str().c_str());
}

void reload_shader_program_from_files(GLuint *program, const char* vertex_path, const char* fragment_path) {
    GLuint reloaded_program = create_shader_program_from_files(vertex_path, fragment_path);
    if (reloaded_program) {
        glDeleteProgram(*program);
        *program = reloaded_program;
    }
}