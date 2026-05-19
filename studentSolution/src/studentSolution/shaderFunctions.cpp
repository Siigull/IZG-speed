#include<studentSolution/shaderFunctions.hpp>

glm::vec4 student_read_texture(Texture const&texture,glm::vec2 const&uv){
  float u = uv.x - std::floor(uv.x);
  float v = uv.y - std::floor(uv.y);
  return student_read_texture_impl(texture,u,v);
}

glm::vec4 student_read_texture(Texture const&texture,float u,float v){
  float uf = u - std::floor(u);
  float vf = v - std::floor(v);
  return student_read_texture_impl(texture,uf,vf);
}

glm::vec4 student_read_textureClamp(Texture const&texture,glm::vec2 const&uv){
  float u = uv.x < 0.f ? 0.f : (uv.x > 1.f ? 1.f : uv.x);
  float v = uv.y < 0.f ? 0.f : (uv.y > 1.f ? 1.f : uv.y);
  return student_read_textureClamp_impl(texture,u,v);
}

glm::vec4 student_read_textureClamp(Texture const&texture,float u,float v){
  float uc = u < 0.f ? 0.f : (u > 1.f ? 1.f : u);
  float vc = v < 0.f ? 0.f : (v > 1.f ? 1.f : v);
  return student_read_textureClamp_impl(texture,uc,vc);
}

glm::vec4 student_texelFetch(Texture const&texture,glm::uvec2 const&pix){
  return __student_texelFetch_inline(texture,pix);
}
