#pragma once

#include "ext/vector_float3.hpp"
#include "fwd.hpp"
#include "learn_opengl/shader.hpp"
#include <glm/glm.hpp>
#include <string>
#include <vector>
struct Vertex
{
    glm::vec3 position;
    glm::vec2 normal;
    glm::vec2 tex_coords;
};

struct Texture
{
    unsigned int id;
    std::string  type;
    std::string  path;
};

class Mesh
{
  public:
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    void draw(Shader& shader);

  private:
    unsigned int VAO, VBO, EBO;

    void setup_mesh();
};
