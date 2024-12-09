#include "terrain.hpp"
#include "perlin.hpp"

#include <glad/glad.h>

Terrain::Terrain(int width, int height, float scale, float displacement, float noise_scale, int noise_octaves, float noise_persistence)
    : width(width), height(height), scale(scale), displacement(displacement), noise_scale(noise_scale), noise_octaves(noise_octaves), noise_persistence(noise_persistence) {
        generate_noise();
        generate_texture();
        generate_vertices();
        generate_indices();
    }

Terrain::~Terrain() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteTextures(1, &texture_id);
}

void Terrain::generate_noise() {
    noise = generate_perlin_noise(width, height, noise_scale, noise_octaves, noise_persistence);
}

void Terrain::generate_vertices() {
    for (int z = 0; z < height; z++) {
        for (int x = 0; x < width; x++) {
            float noise_height = noise[z * width + x] * scale * displacement;

            vertices.push_back(x * scale);
            vertices.push_back(noise_height);
            vertices.push_back(z * scale);

            vertices.push_back(0.2f + noise_height);
            vertices.push_back(0.7f);
            vertices.push_back(0.2f);

            vertices.push_back((float)x / width);
            vertices.push_back((float)z / height);
        }
    }
}

void Terrain::generate_indices() {
    for (int z = 0; z < height - 1; z++) {
        for (int x = 0; x < width - 1; x++) {
            int top_left = z * width + x;
            int top_right = top_left + 1;
            int bottom_left = (z + 1) * width + x;
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

void Terrain::generate_texture() {
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, noise.data());
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Terrain::upload_to_gpu() {
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

void Terrain::render() const {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}