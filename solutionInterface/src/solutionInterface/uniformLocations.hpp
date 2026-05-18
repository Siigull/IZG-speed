/*!
 * @file
 * @brief This file contains declarations of uniform locations within draw model
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#pragma once

#include<cstdint>

/**
 * @brief This macro packs uniform offset and stride into one number
 */
#define UNIFORM_OFFSET_STRIDE(stride,offset) (((stride)<<16u) + (offset))

/**
 * @brief This macro unpacks offset from packed stride+offset number
 */
#define GET_UNIFORM_OFFSET(uniformName) ((uniformName)&0xffffu)

/**
 * @brief This macro unpacks stride from packed stride+offset number
 */
#define GET_UNIFORM_STRIDE(uniformName) ((uniformName)>>16u)

/**
 * @brief This enum specifies offsets for scene uniforms.
 * Scene uniforms are single uniform variables at the start of uniforms' table
 */
enum SceneUniformsOffsets{
  OFFSET_PROJECTION_VIEW_MATRIX    = 0  ,
  OFFSET_LIGHT_POSITION                 ,
  OFFSET_CAMERA_POSITION                ,
  OFFSET_USE_SHADOW_MAP_MATRIX          ,
  OFFSET_CREATE_SHADOW_MAP_MATRIX       ,
  OFFSET_SHADOW_MAP_ID                  ,
  OFFSET_AMBIENT_LIGHT_COLOR            ,
  OFFSET_LIGHT_COLOR                    ,
  OFFSET_FREE_UNIFORMS_START            ,
  NOF_SCENE_UNIFORMS               = 100,
};

/**
 * @brief This enum specifies offsets for draw call uniforms.
 * Draw call uniforms are present for every draw call.
 */
enum DrawCallUniformsOffsets{
  OFFSET_MODEL_MATRIX                    = 0,
  OFFSET_INVERSE_TRANSPOSE_MODEL_MATRIX     ,
  OFFSET_DIFFUSE_COLOR                      ,
  OFFSET_TEXTURE_ID                         ,
  OFFSET_DOUBLE_SIDED                       ,
  NOF_DRAWCALL_UNIFORMS                     ,
};

/**
 * @brief This enum specifies uniform names for receiving uniform handle.
 */
enum UniformNames{
  PROJECTION_VIEW_MATRIX            = UNIFORM_OFFSET_STRIDE(0                    ,                   OFFSET_PROJECTION_VIEW_MATRIX           ),
  LIGHT_POSITION                    = UNIFORM_OFFSET_STRIDE(0                    ,                   OFFSET_LIGHT_POSITION                   ),
  CAMERA_POSITION                   = UNIFORM_OFFSET_STRIDE(0                    ,                   OFFSET_CAMERA_POSITION                  ),
  USE_SHADOW_MAP_MATRIX             = UNIFORM_OFFSET_STRIDE(0                    ,                   OFFSET_USE_SHADOW_MAP_MATRIX            ),
  CREATE_SHADOW_MAP_MATRIX          = UNIFORM_OFFSET_STRIDE(0                    ,                   OFFSET_CREATE_SHADOW_MAP_MATRIX         ),
  SHADOWMAP_ID                      = UNIFORM_OFFSET_STRIDE(0                    ,                   OFFSET_SHADOW_MAP_ID                    ),
  AMBIENT_LIGHT_COLOR               = UNIFORM_OFFSET_STRIDE(0                    ,                   OFFSET_AMBIENT_LIGHT_COLOR              ),
  LIGHT_COLOR                       = UNIFORM_OFFSET_STRIDE(0                    ,                   OFFSET_LIGHT_COLOR                      ),
  FREE_UNIFORMS_START               = UNIFORM_OFFSET_STRIDE(0                    ,                   OFFSET_FREE_UNIFORMS_START              ),
  MODEL_MATRIX                      = UNIFORM_OFFSET_STRIDE(NOF_DRAWCALL_UNIFORMS,NOF_SCENE_UNIFORMS+OFFSET_MODEL_MATRIX                     ),
  INVERSE_TRANSPOSE_MODEL_MATRIX    = UNIFORM_OFFSET_STRIDE(NOF_DRAWCALL_UNIFORMS,NOF_SCENE_UNIFORMS+OFFSET_INVERSE_TRANSPOSE_MODEL_MATRIX   ),
  DIFFUSE_COLOR                     = UNIFORM_OFFSET_STRIDE(NOF_DRAWCALL_UNIFORMS,NOF_SCENE_UNIFORMS+OFFSET_DIFFUSE_COLOR                    ),
  TEXTURE_ID                        = UNIFORM_OFFSET_STRIDE(NOF_DRAWCALL_UNIFORMS,NOF_SCENE_UNIFORMS+OFFSET_TEXTURE_ID                       ),
  DOUBLE_SIDED                      = UNIFORM_OFFSET_STRIDE(NOF_DRAWCALL_UNIFORMS,NOF_SCENE_UNIFORMS+OFFSET_DOUBLE_SIDED                     ),
};


/**
 * @brief This function gets uniform location according to uniform name and drawId.
 *
 * @param drawId id of the drawcall
 * @param uniformName uniform name
 *
 * @return uniform location
 */
inline uint32_t getUniformLocation(uint32_t drawId,uint32_t uniformName){
  return drawId * GET_UNIFORM_STRIDE(uniformName) + GET_UNIFORM_OFFSET(uniformName);
}

/**
 * @brief This function gets uniform name from uniform location
 *
 * @param uniformLocation uniform location
 *
 * @return uniform name
 */
inline uint32_t getUniformName(uint32_t uniformLocation){
  // THIS WILL BREAK IF SCENE UNIFORMS HAVE STRIDE!!!
  if(uniformLocation<NOF_SCENE_UNIFORMS)return UNIFORM_OFFSET_STRIDE(0,uniformLocation);
  uint32_t offset = (uniformLocation - NOF_SCENE_UNIFORMS) % NOF_DRAWCALL_UNIFORMS;
  return UNIFORM_OFFSET_STRIDE(NOF_DRAWCALL_UNIFORMS,NOF_SCENE_UNIFORMS+offset);
}
