/*!
 * @file
 * @brief This file contains functions for model rendering
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */
#include <studentSolution/prepareModel.hpp>
#include <studentSolution/gpu.hpp>
#include <solutionInterface/uniformLocations.hpp>

static void prepareNode(GPUMemory&mem,CommandBuffer&cb,Node const&node,Model const&model,glm::mat4 const&parentMatrix,uint32_t&drawCounter){
  glm::mat4 modelMatrix=parentMatrix*node.modelMatrix;
  if(node.mesh>=0){
    Mesh const&mesh=model.meshes[node.mesh];
    VertexArray vao;
    vao.indexBufferID=mesh.indexBufferID;
    vao.indexOffset=mesh.indexOffset;
    vao.indexType=mesh.indexType;
    vao.vertexAttrib[0]=mesh.position;
    vao.vertexAttrib[1]=mesh.normal;
    vao.vertexAttrib[2]=mesh.texCoord;
    mem.vertexArrays[drawCounter]=vao;
    pushBindVertexArrayCommand(cb,drawCounter);
    pushSetBackfaceCullingCommand(cb,!mesh.doubleSided);
    pushDrawCommand(cb,mesh.nofIndices);
    glm::mat4 invTra=glm::transpose(glm::inverse(modelMatrix));
    mem.uniforms[getUniformLocation(drawCounter,MODEL_MATRIX                   )].m4=modelMatrix;
    mem.uniforms[getUniformLocation(drawCounter,INVERSE_TRANSPOSE_MODEL_MATRIX)].m4=invTra;
    mem.uniforms[getUniformLocation(drawCounter,DIFFUSE_COLOR                  )].v4=mesh.diffuseColor;
    mem.uniforms[getUniformLocation(drawCounter,TEXTURE_ID                     )].i1=mesh.diffuseTexture;
    mem.uniforms[getUniformLocation(drawCounter,DOUBLE_SIDED                   )].v1=mesh.doubleSided?1.f:0.f;
    drawCounter++;
  }
  for(size_t i=0;i<node.nofChildren;++i)
    prepareNode(mem,cb,node.children[i],model,modelMatrix,drawCounter);
}

void student_prepareModel(GPUMemory&mem,CommandBuffer&commandBuffer,Model const&model){
  for(size_t i=0;i<model.nofBuffers;++i)
    mem.buffers[i]=model.buffers[i];
  for(size_t i=0;i<model.nofTextures;++i)
    mem.textures[i]=model.textures[i];
  uint32_t drawCounter=0;
  for(size_t i=0;i<model.nofRoots;++i)
    prepareNode(mem,commandBuffer,model.roots[i],model,glm::mat4(1.f),drawCounter);
}
