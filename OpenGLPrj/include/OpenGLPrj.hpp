// Preprocessor Directives
#ifndef OpenGLPrj
#define OpenGLPrj
#pragma once

// System Headers
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// Reference: https://github.com/nothings/stb/blob/master/stb_image.h#L4
// To use stb_image, add this in *one* C++ source file.
//     #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Define Some Constants
int window_width = 512;
int window_height = 512;
const char* project_source_dir = PROJECT_SOURCE_DIR;

#endif //~ Glitter Header
