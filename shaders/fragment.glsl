#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 direction;
    float cut_off;
    float outer_cut_off;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    // Attenuation value
    // F_att = 1 / (d ^ 2 * a + d * b + c)
    float a;
    float b;
    float c;
};

uniform Light light;

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 viewPos;

uniform Material material;

void main()
{
    vec4 diffuse_texture = texture(material.diffuse, TexCoords);
    vec3 ambient = diffuse_texture.rgb * light.ambient;

    vec3 lightDir = normalize(light.position - FragPos);
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cut_off - light.outer_cut_off;
    float intensity = clamp((theta - light.outer_cut_off) / epsilon, 0.0, 1.0);

    float distance = length(light.position - FragPos);
    float attenuation = 1.0f / (light.a * distance * distance + light.b * distance + light.c);

    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * diffuse_texture.rgb;

    vec4 specular_texture = texture(material.specular, TexCoords);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * light.specular * specular_texture.rgb;

    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    FragColor = vec4((ambient + diffuse + specular), 1.0);
}
