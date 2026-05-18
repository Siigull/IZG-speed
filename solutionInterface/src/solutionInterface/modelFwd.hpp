/**
 * @file 
 * @brief This file contains forward declaration for model
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#pragma once

#include <solutionInterface/gpu.hpp>

/**
 * @brief Forward declaration of model node.
 */
struct Node;

/**
 * @brief Forward declaration of model mesh.
 */
struct Mesh;

/**
 * @brief This struct represent model
 */
//! [Model]
struct Model{
  Node*   roots       = nullptr;///< list of roots of node trees
  Buffer* buffers     = nullptr;///< list of all buffers in a model
  Mesh*   meshes      = nullptr;///< list of all meshes in a meshes
  Texture*textures    = nullptr;///< list of all textures in a model
  size_t  nofRoots    = 0      ;///< number of roots
  size_t  nofBuffers  = 0      ;///< number of all buffers
  size_t  nofMeshes   = 0      ;///< number of all meshes
  size_t  nofTextures = 0      ;///< number of all textures
};
//! [Model]

/**
 * @brief This structure represents node in tree structure of model
 */
//! [Node]
struct Node{
  glm::mat4 modelMatrix = glm::mat4(1.f);///< model transformation matrix
  int32_t   mesh        = -1            ;///< id of mesh or -1 if no mesh
  Node*     children    = nullptr       ;///< list of children nodes
  size_t    nofChildren = 0             ;///< number of children - zero for life of despair
};
//! [Node]

/**
 * @brief This struct represents a mesh.
 * Mesh is a part of a 3D B-rep model with one material and geometric type.
 */
//! [Mesh]
struct Mesh{
  int32_t      indexBufferID  = -1            ;///< index of buffer used for indices
  size_t       indexOffset    = 0             ;///< offset into index buffer
  IndexType    indexType      = IndexType::U32;///< type of indices
  VertexAttrib position                       ;///< position vertex attribute
  VertexAttrib normal                         ;///< normal vertex attribute
  VertexAttrib texCoord                       ;///< tex. coord vertex attribute
  uint32_t     nofIndices     = 0             ;///< nofIndices or nofVertices (if there is no indexing)
  glm::vec4    diffuseColor   = glm::vec4(1.f);///< default diffuseColor (if there is no texture)
  int          diffuseTexture = -1            ;///< diffuse texture or -1 (no texture)
  bool         doubleSided    = false         ;///< double sided material
};
//! [Mesh]


void free(Model&m);
