#version 330 core
#define NR_POINT_LIGHTS 4

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, TexCoords);
}
