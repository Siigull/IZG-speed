/*!
 * @file
 * @brief This file contains function declarations for model rendering
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */
#pragma once

#include <solutionInterface/modelFwd.hpp>
#include <solutionInterface/gpu.hpp>
#include <solutionInterface/uniformLocations.hpp>
#include <studentSolution/shaderFunctions.hpp>
#include <glm/glm.hpp>
#include <xmmintrin.h>

void student_prepareModel(GPUMemory&mem,CommandBuffer&commandBuffer,Model const&model);

__attribute__((always_inline)) inline void student_drawModel_vertexShader(OutVertex&outVertex,InVertex const&inVertex,ShaderInterface const&si){
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

__attribute__((always_inline)) inline float student_fastRsqrt(float x){
  return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(x)));
}
__attribute__((always_inline)) inline glm::vec3 student_fastNormalize(glm::vec3 const&v){
  float len2=v.x*v.x+v.y*v.y+v.z*v.z;
  return v*student_fastRsqrt(len2);
}

__attribute__((always_inline)) inline void student_drawModel_fragmentShader(OutFragment&outFragment,InFragment const&inFragment,ShaderInterface const&si){
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

  auto N = student_fastNormalize(nor);
  if(doubleSided > 0.f && glm::dot(N, cameraPos - pos) < 0.f)
    N = -N;

  glm::vec4 albedoRGBA = diffuseColor;
  if(textureID >= 0){
    auto&t=si.textures[textureID];
    float ux=texCoord.x<0.f?0.f:(texCoord.x>1.f?1.f:texCoord.x);
    float uy=texCoord.y<0.f?0.f:(texCoord.y>1.f?1.f:texCoord.y);
    float xf=ux*float(t.width-1)+0.5f;
    float yf=uy*float(t.height-1)+0.5f;
    albedoRGBA=__student_texelFetch_inline_nobounds(t,glm::uvec2((uint32_t)xf,(uint32_t)yf));
  }
  glm::vec3 albedo = glm::vec3(albedoRGBA);

  auto L = student_fastNormalize(lightPos - pos);
  float diffuseFactor = glm::max(glm::dot(N, L), 0.f);

  float shadowFactor = 0.f;
  if(shadowMapID >= 0){
    float invW = _mm_cvtss_f32(_mm_rcp_ss(_mm_set_ss(shadowPos.w)));
    float spx = shadowPos.x * invW;
    float spy = shadowPos.y * invW;
    float spz = shadowPos.z * invW;
    if(spx >= 0.f && spx <= 1.f && spy >= 0.f && spy <= 1.f){
      auto&t=si.textures[shadowMapID];
      float xf=spx*float(t.width-1)+0.5f;
      float yf=spy*float(t.height-1)+0.5f;
      float closest=__student_texelFetch_inline_nobounds(t,glm::uvec2((uint32_t)xf,(uint32_t)yf)).r;
      if(spz>closest)
        shadowFactor=1.f;
    }
  }

  if(albedoRGBA.a < 0.5f){
    outFragment.discard = true;
    return;
  }

  glm::vec3 color = albedo * ambientLight + albedo * lightColor * diffuseFactor * (1.f - shadowFactor);
  outFragment.gl_FragColor = glm::vec4(color, albedoRGBA.a);
}
