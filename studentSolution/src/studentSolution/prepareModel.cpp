/*!
 * @file
 * @brief This file contains functions for model rendering
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */
#include <studentSolution/prepareModel.hpp>
#include <studentSolution/gpu.hpp>
#include <studentSolution/shaderFunctions.hpp>
#include <solutionInterface/uniformLocations.hpp>


/**
 * @brief This function prepares model into memory and creates command buffer
 *
 * @param mem gpu memory
 * @param commandBuffer command buffer
 * @param model model structure
 */
//! [drawModel]
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
//! [drawModel]

/**
 * @brief This function represents vertex shader of texture rendering method.
 *
 * @param outVertex output vertex
 * @param inVertex input vertex
 * @param si shader interface
 */
//! [drawModel_vs]
void student_drawModel_vertexShader(OutVertex&outVertex,InVertex const&inVertex,ShaderInterface const&si){
  auto pos = glm::vec4(inVertex.attributes[0].v3,1.f);
  auto nor = glm::vec4(inVertex.attributes[1].v3,0.f);
  auto tex = inVertex.attributes[2].v2;
  auto m = si.uniforms[getUniformLocation(si.gl_DrawID,MODEL_MATRIX)].m4;
  auto itm = si.uniforms[getUniformLocation(si.gl_DrawID,INVERSE_TRANSPOSE_MODEL_MATRIX)].m4;
  auto pv = si.uniforms[getUniformLocation(si.gl_DrawID,PROJECTION_VIEW_MATRIX)].m4;
  auto sm = si.uniforms[getUniformLocation(si.gl_DrawID,USE_SHADOW_MAP_MATRIX)].m4;
  auto worldPos = m * pos;
  outVertex.gl_Position = pv * worldPos;
  outVertex.attributes[0].v3 = glm::vec3(worldPos);
  outVertex.attributes[1].v3 = glm::vec3(itm * nor);
  outVertex.attributes[2].v2 = tex;
  outVertex.attributes[3].v4 = sm * worldPos;
}
//! [drawModel_vs]

#include<iostream>

/**
 * @brief This functionrepresents fragment shader of texture rendering method.
 *
 * @param outFragment output fragment
 * @param inFragment input fragment
 * @param si shader interface
 */
//! [drawModel_fs]
void student_drawModel_fragmentShader(OutFragment&outFragment,InFragment const&inFragment,ShaderInterface const&si){
  auto pos = inFragment.attributes[0].v3;
  auto nor = inFragment.attributes[1].v3;
  auto texCoord = inFragment.attributes[2].v2;
  auto shadowPos = inFragment.attributes[3].v4;

  auto lightPos = si.uniforms[getUniformLocation(si.gl_DrawID,LIGHT_POSITION)].v3;
  auto cameraPos = si.uniforms[getUniformLocation(si.gl_DrawID,CAMERA_POSITION)].v3;
  auto ambientLight = si.uniforms[getUniformLocation(si.gl_DrawID,AMBIENT_LIGHT_COLOR)].v3;
  auto lightColor = si.uniforms[getUniformLocation(si.gl_DrawID,LIGHT_COLOR)].v3;
  auto diffuseColor = si.uniforms[getUniformLocation(si.gl_DrawID,DIFFUSE_COLOR)].v4;
  auto textureID = si.uniforms[getUniformLocation(si.gl_DrawID,TEXTURE_ID)].i1;
  auto doubleSided = si.uniforms[getUniformLocation(si.gl_DrawID,DOUBLE_SIDED)].v1;
  auto shadowMapID = si.uniforms[getUniformLocation(si.gl_DrawID,SHADOWMAP_ID)].i1;

  auto N = glm::normalize(nor);
  if(doubleSided > 0.f && glm::dot(N, cameraPos - pos) < 0.f)
    N = -N;

  glm::vec4 albedoRGBA = diffuseColor;
  if(textureID >= 0)
    albedoRGBA = student_read_textureClamp(si.textures[textureID], texCoord);
  glm::vec3 albedo = glm::vec3(albedoRGBA);

  auto L = glm::normalize(lightPos - pos);
  float diffuseFactor = glm::max(glm::dot(N, L), 0.f);

  float shadowFactor = 0.f;
  if(shadowMapID >= 0){
    auto sp = shadowPos / shadowPos.w;
    if(sp.x >= 0.f && sp.x <= 1.f && sp.y >= 0.f && sp.y <= 1.f){
      float closest = student_read_textureClamp(si.textures[shadowMapID], glm::vec2(sp)).r;
      if(sp.z > closest)
        shadowFactor = 1.f;
    }
  }

  if(albedoRGBA.a < 0.5f){
    outFragment.discard = true;
    return;
  }

  glm::vec3 color = albedo * ambientLight + albedo * lightColor * diffuseFactor * (1.f - shadowFactor);
  outFragment.gl_FragColor = glm::vec4(color, albedoRGBA.a);
}
//! [drawModel_fs]

