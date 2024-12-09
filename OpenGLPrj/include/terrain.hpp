#pragma once

#include "chunk.hpp"
#include "camera.hpp"

#include <GLFW/glfw3.h>
#include <vector>

class Terrain {
    int width;
    int height;
    float scale;
    float displacement;

    float noise_scale;
    int noise_octaves;
    float noise_persistence;
    std::vector<float> noise;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    unsigned int vao, vbo, ebo, texture_id;

    void generate_noise();
    void generate_vertices();
    void generate_indices();
    void generate_texture();

public:
    Terrain(int width, int height, float scale, float displacement, float noise_scale, int noise_octaves, float noise_persistence);
    ~Terrain();

    void upload_to_gpu();
    void render() const;
};

class TerrainManager {
    int chunk_size;
    int render_distance;
    float chunk_displacement;
    int noise_octaves;
    float noise_persistence;
    std::vector<Chunk> chunks;

public:
    TerrainManager(int chunk_size, int render_distance, float chunk_displacement, int noise_octaves, float noise_persistence);
    ~TerrainManager();

    void generate_chunks();
    void update_chunks(GLFWwindow* window);

    void render() const;
};