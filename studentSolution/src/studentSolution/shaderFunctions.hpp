/*!
 * @file
 * @brief This file contains declaration of function that can be used in shaders.
 * Students are required to implement these functions.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#pragma once

#include<solutionInterface/gpu.hpp>
#include<cmath>

glm::vec4 student_read_texture     (Texture const&texture,glm::vec2  const&uv);
glm::vec4 student_read_texture     (Texture const&texture,float u,float v);
glm::vec4 student_read_textureClamp(Texture const&texture,glm::vec2  const&uv);
glm::vec4 student_read_textureClamp(Texture const&texture,float u,float v);
glm::vec4 student_texelFetch(Texture const&texture,glm::uvec2 const&pix);

static inline glm::vec4 __student_texelFetch_inline(Texture const&texture,glm::uvec2 const&pix){
  auto&img = texture.img;
  glm::vec4 color = glm::vec4(0.f,0.f,0.f,1.f);
  if(pix.x>=texture.width || pix.y >=texture.height)return color;
  if(img.format == Image::U8){
    auto colorPtr = (uint8_t*)img.data + pix.y * img.pitch + pix.x * img.bytesPerPixel;
    if(img.channels==4 && img.channelTypes[0]==Image::RED && img.channelTypes[1]==Image::GREEN && img.channelTypes[2]==Image::BLUE && img.channelTypes[3]==Image::ALPHA){
      return glm::vec4(colorPtr[0]/255.f,colorPtr[1]/255.f,colorPtr[2]/255.f,colorPtr[3]/255.f);
    }
    for(uint32_t c=0;c<img.channels;++c)
      color[c] = colorPtr[img.channelTypes[c]]/255.f;
  }
  else if(img.format == Image::F32){
    auto colorPtr = (float*)((uint8_t*)img.data + pix.y * img.pitch + pix.x * img.bytesPerPixel);
    if(img.channels==1 && img.channelTypes[0]==Image::RED){
      return glm::vec4(*colorPtr,0.f,0.f,1.f);
    }
    for(uint32_t c=0;c<img.channels;++c)
      color[c] = colorPtr[img.channelTypes[c]];
  }
  return color;
}

static inline glm::vec4 student_read_texture_impl(Texture const&texture,float u,float v){
  if(!texture.img.data)return glm::vec4(0.f);
  float xf = u * float(texture.width - 1) + 0.5f;
  float yf = v * float(texture.height - 1) + 0.5f;
  return __student_texelFetch_inline(texture,glm::uvec2((uint32_t)xf,(uint32_t)yf));
}

static inline glm::vec4 student_read_textureClamp_impl(Texture const&texture,float u,float v){
  if(!texture.img.data)return glm::vec4(0.f);
  float xf = u * float(texture.width - 1) + 0.5f;
  float yf = v * float(texture.height - 1) + 0.5f;
  return __student_texelFetch_inline(texture,glm::uvec2((uint32_t)xf,(uint32_t)yf));
}
