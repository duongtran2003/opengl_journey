#pragma once

#include "learn_opengl/mesh.hpp"
#include "learn_opengl/shader.hpp"
#include <assimp/material.h>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <string>
#include <vector>

class Model
{
  public:
    Model(const char* path);

    void         draw(Shader& shader);
    unsigned int texture_from_file(const char* path, const std::string& directory, bool gamma = false);

  private:
    std::vector<Texture> textures_loaded;
    std::vector<Mesh>    meshes;
    std::string          directory;

    void                 load_model(std::string path);
    void                 process_node(aiNode* node, const aiScene* scene);
    Mesh                 process_mesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> load_material_textures(aiMaterial* mat, aiTextureType type, std::string type_name);
};
