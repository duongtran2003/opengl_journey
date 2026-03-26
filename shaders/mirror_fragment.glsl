#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screen_texture;

void main()
{
    float thickness = 0.05f;
    vec3 border_color = vec3(1.0f, 1.0f, 1.0f);

    if (TexCoords.x < thickness || TexCoords.x > (1.0f - thickness) || TexCoords.y < thickness || TexCoords.y > (1.0f - thickness))
    {
        FragColor = vec4(border_color, 1.0f);
    }
    else
    {
        vec4 color = texture(screen_texture, TexCoords);
        FragColor = color;
    }
}
