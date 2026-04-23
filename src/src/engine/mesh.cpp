#include "engine/mesh.hpp"
#include "engine/shader.hpp"
#include <glad/glad.h>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices,
  std::vector<TextureA> textures): vertices_(vertices), indices_ (indices),
  textures_ (textures) {
  setupMesh();
}

void Mesh::setupMesh() {
  // create buffers/arrays
  glGenVertexArrays(1, &VAO_);
  glGenBuffers(1, &VBO_);
  glGenBuffers(1, &EBO_);

  glBindVertexArray(VAO_);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_);

  glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), &vertices_[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(uint32_t), &indices_[0], GL_STATIC_DRAW);


  // vertex Positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
  // vertex texture coords
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
  // vertex normals
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
  // vertex tangent
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
  // vertex bitangent
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

  glBindVertexArray(0);
}

void Mesh::render(const Shader& shader) const {
  // bind appropriate textures
  uint32_t diffuseNr = 1;
  uint32_t specularNr = 1;
  uint32_t normalNr = 1;
  uint32_t heightNr = 1;
  for (uint8_t i = 0; i < textures_.size(); i++) {
    glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
    // retrieve texture number (the N in diffuse_textureN)
    std::string number;
    std::string name = textures_[i].type;
    if (name == "texture_diffuse")
      number = std::to_string(diffuseNr++);
    else if (name == "texture_specular")
      number = std::to_string(specularNr++); // transfer unsigned int to stream
    else if (name == "texture_normal")
      number = std::to_string(normalNr++); // transfer unsigned int to stream
    else if (name == "texture_height")
      number = std::to_string(heightNr++); // transfer unsigned int to stream

                       // now set the sampler to the correct texture unit
    shader.set((name + number).c_str(), i);
    // and finally bind the texture
    glBindTexture(GL_TEXTURE_2D, textures_[i].id);
  }

  // draw mesh
  glBindVertexArray(VAO_);
  glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  // always good practice to set everything back to defaults once configured.
  glActiveTexture(GL_TEXTURE0);
}