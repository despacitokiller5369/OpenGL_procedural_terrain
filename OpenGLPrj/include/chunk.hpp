#pragma once

#include "perlin.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

class Chunk {
public:
    glm::vec3 position;
    int chunk_size;
    float chunk_displacement;
    int noise_octaves;
    float noise_persistence;
    bool is_active;
    unsigned int vao, vbo, ebo;
    unsigned int texture_id;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    Chunk(glm::vec3 position, int chunk_size, float chunk_displacement, int noise_octaves, float noise_persistence);
    ~Chunk();

    void generate_vertices();
    void generate_indices();
    void generate_texture();
    void upload_to_gpu();
    void render() const;
};