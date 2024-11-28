#pragma once

#include <vector>

float fade(float t);
float lerp(float t, float a, float b);
float grad(int hash, float x, float y);
float perlin_noise(float x, float y);
std::vector<float> generate_perlin_noise(int width, int height, float scale);