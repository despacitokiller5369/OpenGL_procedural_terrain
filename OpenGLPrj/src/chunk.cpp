#include "chunk.hpp"

Chunk::Chunk(glm::vec3 position, int chunk_size, float chunk_displacement, int noise_octaves, float noise_persistence) 
    : position(position), chunk_size(chunk_size), chunk_displacement(chunk_displacement), noise_octaves(noise_octaves), noise_persistence(noise_persistence), is_active(true) {
        generate_vertices();
        generate_indices();
        generate_texture();
        upload_to_gpu();
}

Chunk::~Chunk() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteTextures(1, &texture_id);
}

void Chunk::generate_vertices() {
    for (int z = 0; z < chunk_size; z++) {
        for (int x = 0; x < chunk_size; x++) {
            float world_x = position.x + x;
            float world_z = position.z + z;
            float height = perlin_noise(world_x, world_z, noise_octaves, noise_persistence) * chunk_displacement;

            vertices.push_back(world_x);
            vertices.push_back(height);
            vertices.push_back(world_z);

            vertices.push_back(0.2f + height);
            vertices.push_back(0.7f);
            vertices.push_back(0.2f);

            vertices.push_back((float)x / chunk_size);
            vertices.push_back((float)z / chunk_size);
        }
    }
}

void Chunk::generate_indices() {
    for (int z = 0; z < chunk_size - 1; z++) {
        for (int x = 0; x < chunk_size - 1; x++) {
            int top_left = z * chunk_size + x;
            int top_right = top_left + 1;
            int bottom_left = (z + 1) * chunk_size + x;
            int bottom_right = bottom_left + 1;
        
            indices.push_back(top_left);
            indices.push_back(bottom_left);
            indices.push_back(top_right);
        
            indices.push_back(top_right);
            indices.push_back(bottom_left);
            indices.push_back(bottom_right);
        }
    }
}

void Chunk::generate_texture() {
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, chunk_size, chunk_size, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Chunk::upload_to_gpu() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Chunk::render() const {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}