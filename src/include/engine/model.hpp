#ifndef __MODEL_H__
#define __MODEL_H__ 1
#include <string>
#include "engine/mesh.hpp"
#include "assimp/material.h"

class aiNode;
struct aiScene;
class aiMesh;
class aiMaterial;

class Model {
public:
  /*  Model Data */
  std::vector<TextureA> textures_loaded_;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
  std::vector<Mesh> meshes_;
  std::string directory_;
  bool gammaCorrection_;

  /*  Functions   */
  // constructor, expects a filepath to a 3D model.
  Model(std::string const &path, bool gamma = false);

  // draws the model, and thus all its meshes
  void render(const Shader& shader) const;
private:
  /*  Functions   */
  // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
  void loadModel(std::string const path);

  // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
  void processNode(aiNode *node, const aiScene *scene);
  Mesh processMesh(aiMesh *mesh, const aiScene *scene);

  // checks all material textures of a given type and loads the textures if they're not loaded yet.
  // the required info is returned as a Texture struct.
  std::vector<TextureA> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
    std::string typeName);
};

#endif