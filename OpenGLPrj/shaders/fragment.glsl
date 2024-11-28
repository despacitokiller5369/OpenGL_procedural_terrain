#version 400 core

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D perlinTexture;

void main() {
    float noiseValue = texture(perlinTexture, TexCoord).r;
    FragColor = vec4(vec3(noiseValue), 1.0);
}
