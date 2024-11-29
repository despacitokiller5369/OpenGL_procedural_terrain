#pragma once

#include <string>
#include <fstream>
#include <sstream>

class Shader {
    unsigned int id;

    void check_compile_errors(unsigned int shader, std::string type);
public:
    Shader(const char* vertex_path, const char* fragment_path);

    void use();
    void set_bool(const std::string& name, bool value) const;
    void set_int(const std::string& name, int value) const;
    void set_float(const std::string& name, float value) const;
};
