#pragma once

#include <vector>

float fade(float t);
float lerp(float t, float a, float b);
float grad(int hash, float x, float y);
float perlin_noise(float x, float y);
float generate_perlin_noise_at(int x, int z, float scale, int octaves, float persistence);
std::vector<float> generate_perlin_noise(int width, int height, float scale);
std::vector<float> generate_perlin_noise(int width, int height, float scale, int octaves, float persistence);
void apply_gaussian_blur(std::vector<float>& noise, int width, int height);
void apply_gaussian_blur(std::vector<float>& noise, int width, int height, int kernel_size, float sigma);