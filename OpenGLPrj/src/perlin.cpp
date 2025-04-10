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
    
    int perm[] = {151, 160, 137,  91,  90,  15, 131,  13, 201,  95,  96,  53, 194, 233,   7, 225,
        140,  36, 103,  30,  69, 142,   8,  99,  37, 240,  21,  10,  23, 190,   6, 148,
        247, 120, 234,  75,   0,  26, 197,  62,  94, 252, 219, 203, 117,  35,  11,  32,
        57,  177,  33,  88, 237, 149,  56,  87, 174,  20, 125, 136, 171, 168,  68, 175,
        74,  165,  71, 134, 139,  48,  27, 166,  77, 146, 158, 231,  83, 111, 229, 122,
        60,  211, 133, 230, 220, 105,  92,  41,  55,  46, 245,  40, 244, 102, 143,  54,
        65,   25,  63, 161,   1, 216,  80,  73, 209,  76, 132, 187, 208,  89,  18, 169,
        200, 196, 135, 130, 116, 188, 159,  86, 164, 100, 109, 198, 173, 186,   3,  64,
        52,  217, 226, 250, 124, 123,   5, 202,  38, 147, 118, 126, 255,  82,  85, 212,
        207, 206,  59, 227,  47,  16,  58,  17, 182, 189,  28,  42, 223, 183, 170, 213,
        119, 248, 152,   2,  44, 154, 163,  70, 221, 153, 101, 155, 167,  43, 172,   9,
        129,  22,  39, 253,  19,  98, 108, 110,  79, 113, 224, 232, 178, 185, 112, 104,
        218, 246,  97, 228, 251,  34, 242, 193, 238, 210, 144,  12, 191, 179, 162, 241,
        81,   51, 145, 235, 249,  14, 239, 107,  49, 192, 214,  31, 181, 199, 106, 157,
        184,  84, 204, 176, 115, 121,  50,  45, 127,   4, 150, 254, 138, 236, 205,  93,
        222, 114,  67,  29,  24,  72, 243, 141, 128, 195,  78,  66, 215,  61, 156, 180};
    
    
    
    int a = perm[X] + Y;
    if (a >= 255 || a < 0) a = 0;
    int aa = perm[a];
    int ab = perm[a + 1];
    int b = perm[X + 1] + Y;
    if (b >= 255 || b < 0) b = 0;
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

float generate_perlin_noise_at(int x, int z, float scale, int octaves, float persistence) {
    float total = 0.0f;
    float max_val = 0.0f;
    
    for (int i = 0; i < octaves; i++) {
        float frequency = std::pow(2.0f, i);
        float amplitude = std::pow(persistence, i);
        
        float nx = x / scale * frequency;
        float nz = z / scale * frequency;
        
        float v = perlin_noise(nx, nz);
        total += v * amplitude;
        max_val += amplitude;
    }
    
    total = (total / max_val + 1.0f) / 2.0f;
    return total;
}

std::vector<float> generate_perlin_noise(int width, int height, float scale) {
    std::vector<float> noise(width * height);
    float min_val = std::numeric_limits<float>::max();
    float max_val = std::numeric_limits<float>::lowest();
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float nx = x / (float)width * scale;
            float ny = y / (float)height * scale;
            float v = perlin_noise(nx, ny);
            noise[y * width + x] = v;
            min_val = std::min(min_val, v);
            max_val = std::max(max_val, v);
        }
    }
    
    for (unsigned int i = 0; i < noise.size(); i++) {
        noise[i] = (noise[i] - min_val) / (max_val - min_val);
    }
    
    return noise;
}

std::vector<float> generate_perlin_noise(int width, int height, float scale, int octaves, float persistence) {
    std::vector<float> noise(width * height);
    float min_val = std::numeric_limits<float>::max();
    float max_val = std::numeric_limits<float>::lowest();
    
    for (int i = 0; i < octaves; i++) {
        float amplitude = std::pow(persistence, i);
        
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                float nx = x / (float)width * scale * std::pow(2, i);
                float ny = y / (float)height * scale * std::pow(2, i);
                float v = perlin_noise(nx, ny);
                noise[y * width + x] += v * amplitude;
                min_val = std::min(min_val, v);
                max_val = std::max(max_val, v);
            }
        }
    }
    
    for (unsigned int i = 0; i < noise.size(); i++) {
        noise[i] = (noise[i] - min_val) / (max_val - min_val);
    }
    
    return noise;
}

void apply_gaussian_blur(std::vector<float>& noise, int width, int height) {
    std::vector<float> blurrded_noise = noise;
    int kernel[3][3] = {
        {1, 2, 1},
        {2, 4, 2},
        {1, 2, 1}
    };
    int kernel_sum = 16;
    
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            float sum = 0.0f;
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    int ix = (x + kx) + (y + ky) * width;
                    sum += noise[ix] * kernel[ky + 1][kx + 1];
                }
            }
            blurrded_noise[y * width + x] = sum / kernel_sum;
        }
    }
    
    noise = blurrded_noise;
}

void apply_gaussian_blur(std::vector<float>& noise, int width, int height, int kernel_size, float sigma) {
    std::vector<std::vector<float>> kernel(kernel_size, std::vector<float>(kernel_size));
    int half_size = kernel_size / 2;
    float sum = 0.0f;
    
    for (int y = -half_size; y <= half_size; y++) {
        for (int x = -half_size; x <= half_size; x++) {
            kernel[y + half_size][x + half_size] = std::exp(-(x * x + y * y) / (2 * sigma * sigma));
            sum += kernel[y + half_size][x + half_size];
        }
    }
    
    for (int y = 0; y < kernel_size; y++) {
        for (int x = 0; x < kernel_size; x++) {
            kernel[y][x] /= sum;
        }
    }
    
    std::vector<float> blurred_noise = noise;
    
    for (int y = half_size; y < height - half_size; y++) {
        for (int x = half_size; x < width - half_size; x++) {
            float sum = 0.0f;
            for (int ky = -half_size; ky <= half_size; ky++) {
                for (int kx = -half_size; kx <= half_size; kx++) {
                    int ix = x + kx;
                    int iy = y + ky;
                    sum += noise[iy * width + ix] * kernel[ky + half_size][kx + half_size];
                }
            }
            blurred_noise[y * width + x] = sum;
        }
    }
    
    noise = blurred_noise;
}