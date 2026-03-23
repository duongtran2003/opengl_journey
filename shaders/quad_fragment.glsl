#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screen_texture;
uniform float screen_height;
uniform float screen_width;

void main()
{
    float offset_w = 1.0f / screen_width;
    float offset_h = 1.0f / screen_height;

    vec2 offsets[9] = vec2[](
            vec2(-offset_w, offset_h), // Top left
            vec2(0.0f, offset_h), // Top center
            vec2(offset_w, offset_h), // Top right

            vec2(-offset_w, 0.0f), // Center left
            vec2(0.0f, 0.0f), // Center center
            vec2(offset_w, 0.0f), // Center right

            vec2(-offset_w, -offset_h), // Bottom left
            vec2(0.0f, -offset_h), // Bottom center
            vec2(offset_w, -offset_h) // Bottom right
        );

    float kernel[9] = float[](
            0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f
        );

    vec3 sample_texture[9];
    for (int i = 0; i < 9; i++) {
        sample_texture[i] = vec3(texture(screen_texture, TexCoords.st + offsets[i]));
    }

    vec3 color = vec3(0.0f);
    for (int i = 0; i < 9; i++) {
        color += sample_texture[i] * kernel[i];
    }

    FragColor = vec4(color, 1.0f);
}
