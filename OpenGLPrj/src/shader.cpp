#include "shader.hpp"
#include "utils.hpp"

#include <string>
#include <fstream>
#include <sstream>

Shader::Shader(const char* vertex_path, const char* fragment_path) {
    std::string vertex_code;
    std::string fragment_code;
    std::ifstream vertex_file(vertex_path);
    std::ifstream fragment_file(fragment_path);

    if (!vertex_file.is_open() || !fragment_file.is_open()) {
        gl_log_err("log.log", "ERROR: Could not open shader files %s and/or %s\n", vertex_path, fragment_path);
        return;
    }

    std::stringstream vertex_stream;
    std::stringstream fragment_stream;

    vertex_stream << vertex_file.rdbuf();
    fragment_stream << fragment_file.rdbuf();
    vertex_file.close();
    fragment_file.close();
    vertex_code = vertex_stream.str();
    fragment_code = fragment_stream.str();

    const char* vertex_source = vertex_code.c_str();
    const char* fragment_source = fragment_code.c_str();

    unsigned int vertex;
    unsigned int fragment;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertex_source, NULL);
    glCompileShader(vertex);
    check_compile_errors(vertex, "VERTEX");

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragment_source, NULL);
    glCompileShader(fragment);
    check_compile_errors(fragment, "FRAGMENT");

    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);
    check_compile_errors(id, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use() {
    glUseProgram(id);
}

void Shader::set_bool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}

void Shader::set_int(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::set_float(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::check_compile_errors(unsigned int shader, std::string type) {
    int success;
    char info_log[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, info_log);
            gl_log_err("log.log", "ERROR::SHADER_COMPILATION_ERROR of type: %s\n%s\n", type.c_str(), info_log);
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, info_log);
            gl_log_err("log.log", "ERROR::PROGRAM_LINKING_ERROR of type: %s\n%s\n", type.c_str(), info_log);
        }
    }
}