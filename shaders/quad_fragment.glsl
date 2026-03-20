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

    FragColor = texture(screen_texture, TexCoords);
}
