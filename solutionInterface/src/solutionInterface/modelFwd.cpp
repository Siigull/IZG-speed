#include <solutionInterface/modelFwd.hpp>

void deleteNode(Node*n){
  if(!n)return;
  for(size_t i=0;i<n->nofChildren;++i)
    deleteNode(n->children+i);
  delete[]n->children;
}

void free(Model&model){
  if(model.buffers )delete[]model.buffers ;
  if(model.meshes  )delete[]model.meshes  ;
  if(model.textures)delete[]model.textures;

  if(model.roots){
    for(size_t i=0;i<model.nofRoots;++i)
      deleteNode(model.roots+i);
    delete[]model.roots;
  }

  model.buffers  = nullptr;
  model.meshes   = nullptr;
  model.textures = nullptr;
  model.roots    = nullptr;

  model.nofBuffers  = 0;
  model.nofMeshes   = 0;
  model.nofTextures = 0;
  model.roots       = 0;
}

