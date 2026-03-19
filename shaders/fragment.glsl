#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D texture1;

void main()
{
    vec4 tex_color = texture(texture1, TexCoords);
    FragColor = tex_color;
}
