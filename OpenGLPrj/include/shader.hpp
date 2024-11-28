#pragma once

#include <glad/glad.h>

GLuint create_shader_program_from_strings(const char* vertex_source, const char* fragment_source);
GLuint create_shader_program_from_files(const char* vertex_path, const char* fragment_path);
void reload_shader_program_from_files(GLuint *program, const char* vertex_path, const char* fragment_path);