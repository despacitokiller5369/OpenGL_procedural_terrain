#version 400 core

out vec4 FragColor;

in vec3 outColor;
in vec2 TexCoord;

uniform sampler2D perlinTexture;

void main() {
    float noiseValue = texture(perlinTexture, TexCoord).r;

    vec3 finalColor = outColor * noiseValue;

    float grayscale = dot(finalColor, vec3(0.299, 0.587, 0.114));

    FragColor = vec4(vec3(grayscale), 1.0);
}
