/**
 * @file 
 * @brief This file contains forward declaration of warped funtion calls
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#pragma once

#include<solutionInterface/gpu.hpp>
#include<solutionInterface/modelFwd.hpp>

///\cond HIDDEN_SYMBOLS
using GPU_Run           = void(*)(GPUMemory&,CommandBuffer const&             );
using PrepareModel      = void(*)(GPUMemory&,CommandBuffer      &,Model const&); /// < Type of function for preparing models
using Read_Texture      = glm::vec4(*)(Texture const&,glm::vec2  const&); /// < type of function for reading form textures
using Read_TextureClamp = glm::vec4(*)(Texture const&,glm::vec2  const&); /// < type of function for clamped reading from textures
using TexelFetch        = glm::vec4(*)(Texture const&,glm::uvec2 const&); /// < type of function for texel fetching from textures

void      gpuRun                  (GPUMemory&mem,CommandBuffer const&cb);
void      prepareModel            (GPUMemory&mem,CommandBuffer&cb,Model const&model);
void      drawModel_vertexShader  (OutVertex  &outVertex  ,InVertex   const&inVertex  ,ShaderInterface const&si);
void      drawModel_fragmentShader(OutFragment&outFragment,InFragment const&inFragment,ShaderInterface const&si);
glm::vec4 read_texture            (Texture const&texture,glm::vec2  const&uv);
glm::vec4 read_textureClamp       (Texture const&texture,glm::vec2  const&uv);
glm::vec4 texelFetch              (Texture const&texture,glm::uvec2 const&uv);
///\endcond

/**
 * @brief This structure represents solution functions
 */
struct TaskFunctions{
  GPU_Run           gpu_run                  = nullptr; ///< executes GPU
  PrepareModel      prepareModel             = nullptr; ///< prepares model into command buffer and memory
  VertexShader      drawModel_vertexShader   = nullptr; ///< vertex shader for model drawing
  FragmentShader    drawModel_fragmentShader = nullptr; ///< fragment shader for model drawing
  Read_Texture      read_texture             = nullptr; ///< shader function for reading from texture
  Read_TextureClamp read_textureClamp        = nullptr; ///< shader function for reading from texture with coord campling
  TexelFetch        texelFetch               = nullptr; ///< shader function for fetching from texture
  GPU_Run           breakable_gpu_run        = nullptr; ///< breakable gpu for debugging (only for teacher)
  size_t*           idToBreak                = nullptr; ///< id of break point in breakable gpu
  size_t*           lineToBreak              = nullptr; ///< line of break point int breakable gpu
};

