#version 330 core
#define NR_POINT_LIGHTS 4

struct DirectionalLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;

    // Attenuation value
    // F_att = 1 / (d ^ 2 * a + d * b + c)
    float a;
    float b;
    float c;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;

    float cut_off;
    float outer_cut_off;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float a;
    float b;
    float c;

    bool enable;
};

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 u_view_pos;
uniform Material material;
uniform SpotLight spot_light;
uniform PointLight point_lights[NR_POINT_LIGHTS];
uniform DirectionalLight directional_light;

out vec4 FragColor;

vec3 calculate_directional_light(DirectionalLight light, vec3 normal, vec3 view_dir, vec3 diffuse_color, vec3 specular_color);
vec3 calculate_point_light(PointLight point_light, vec3 normal, vec3 frag_pos, vec3 view_dir, vec3 diffuse_color, vec3 specular_color);
vec3 calculate_spot_light(SpotLight light, vec3 normal, vec3 view_dir, vec3 frag_pos, vec3 diffuse_color, vec3 specular_color);

float calculate_attenuation(float a, float b, float c, vec3 light_position, vec3 frag_pos);
void phong_shading(vec3 light_ambient, vec3 light_diffuse, vec3 light_specular, vec3 light_dir, vec3 normal, vec3 view_dir, vec3 diffuse_map, vec3 specular_map, inout vec3 ambient, inout vec3 diffuse, inout vec3 specular);

void main()
{
    vec3 output_color = vec3(0.0f);

    vec3 normal = normalize(Normal);
    vec3 view_dir = normalize(u_view_pos - FragPos);

    vec3 diffuse_color = vec3(texture(material.diffuse, TexCoords));
    vec3 specular_color = vec3(texture(material.specular, TexCoords));

    output_color += calculate_directional_light(directional_light, normal, view_dir, diffuse_color, specular_color);
    for (int i = 0; i < NR_POINT_LIGHTS; i++)
    {
        output_color += calculate_point_light(point_lights[i], normal, FragPos, view_dir, diffuse_color, specular_color);
    }

    if (spot_light.enable) {
        output_color += calculate_spot_light(spot_light, normal, view_dir, FragPos, diffuse_color, specular_color);
    }

    FragColor = vec4(output_color, 1.0f);
}

float calculate_attenuation(float a, float b, float c, vec3 light_position, vec3 frag_pos)
{
    float d = length(light_position - frag_pos);
    float attenuation = 1.0f / (a * d * d + b * d + c);

    return attenuation;
}

void phong_shading(vec3 light_ambient, vec3 light_diffuse, vec3 light_specular, vec3 light_dir, vec3 normal, vec3 view_dir, vec3 diffuse_map, vec3 specular_map, inout vec3 ambient, inout vec3 diffuse, inout vec3 specular)
{
    float diff = max(dot(normal, light_dir), 0.0f);
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0f), material.shininess);

    ambient = light_ambient * diffuse_map;
    diffuse = light_diffuse * diff * diffuse_map;
    specular = light_specular * spec * specular_map;
}

vec3 calculate_directional_light(DirectionalLight light, vec3 normal, vec3 view_dir, vec3 diffuse_color, vec3 specular_color)
{
    vec3 ambient, diffuse, specular;
    vec3 light_dir = normalize(-light.direction);
    phong_shading(light.ambient, light.diffuse, light.specular, light_dir, normal, view_dir, diffuse_color, specular_color, ambient, diffuse, specular);

    return ambient + diffuse + specular;
}

vec3 calculate_point_light(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir, vec3 diffuse_color, vec3 specular_color)
{
    vec3 ambient, diffuse, specular;
    vec3 light_dir = normalize(light.position - frag_pos);
    phong_shading(light.ambient, light.diffuse, light.specular, light_dir, normal, view_dir, diffuse_color, specular_color, ambient, diffuse, specular);

    float attenuation = calculate_attenuation(light.a, light.b, light.c, light.position, frag_pos);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

vec3 calculate_spot_light(SpotLight light, vec3 normal, vec3 view_dir, vec3 frag_pos, vec3 diffuse_color, vec3 specular_color)
{
    vec3 ambient, diffuse, specular;
    vec3 light_dir = normalize(light.position - frag_pos);
    phong_shading(light.ambient, light.diffuse, light.specular, light_dir, normal, view_dir, diffuse_color, specular_color, ambient, diffuse, specular);

    float attenuation = calculate_attenuation(light.a, light.b, light.c, light.position, frag_pos);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    float theta = dot(light_dir, normalize(-light.direction));
    float epsilon = light.cut_off - light.outer_cut_off;

    float intensity = clamp((theta - light.outer_cut_off) / epsilon, 0.0f, 1.0f);

    diffuse *= intensity;
    specular *= intensity;

    return ambient + diffuse + specular;
}
