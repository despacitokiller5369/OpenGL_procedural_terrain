#include "perlin.hpp"

#include <cmath>
#include <vector>
#include <algorithm>
#include <random>

float fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float lerp(float t, float a, float b) {
    return a + t * (b - a);
}

float grad(int hash, float x, float y) {
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : h == 12 || h == 14 ? x : y;
    return (h & 1 ? -1 : 1) * (u + v);
}

float perlin_noise(float x, float y) {
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;

    x -= std::floor(x);
    y -= std::floor(y);

    float u = fade(x);
    float v = fade(y);

    std::vector<int> perm(256);
    std::iota(perm.begin(), perm.end(), 0);
    std::random_shuffle(perm.begin(), perm.end());

    int a = perm[X] + Y;
    int aa = perm[a];
    int ab = perm[a + 1];
    int b = perm[X + 1] + Y;
    int ba = perm[b];
    int bb = perm[b + 1];

    float gradAA = grad(aa, x, y);
    float gradAB = grad(ab, x, y - 1);
    float gradBA = grad(ba, x - 1, y);
    float gradBB = grad(bb, x - 1, y - 1);

    float lerpX1 = lerp(u, gradAA, gradBA);
    float lerpX2 = lerp(u, gradAB, gradBB);

    return lerp(v, lerpX1, lerpX2);
}

std::vector<float> generate_perlin_noise(int width, int height, float scale) {
    std::vector<float> noise(width * height);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float nx = x / (float)width * scale;
            float ny = y / (float)height * scale;
            noise[y * width + x] = perlin_noise(nx, ny);
        }
    }
    return noise;
}