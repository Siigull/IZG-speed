#include<framework/switchSolution.hpp>

#include<solutionInterface/taskFunctions.hpp>
#include<studentSolution/gpu.hpp>
#include<studentSolution/prepareModel.hpp>
#include<studentSolution/shaderFunctions.hpp>

bool useTeacherSolution = false;

TaskFunctions taskFunctions_teacher;
TaskFunctions taskFunctions_student = {
  student_GPU_run                 ,
  student_prepareModel            ,
  student_drawModel_vertexShader  ,
  student_drawModel_fragmentShader,
  student_read_texture            ,
  student_read_textureClamp       ,
  student_texelFetch              ,
};
TaskFunctions taskFunctions_impl = taskFunctions_student;

void switchToStudentSolution(){
  useTeacherSolution = false;
  taskFunctions_impl = taskFunctions_student;
}

void switchToTeacherSolution(){
  useTeacherSolution = true;
  taskFunctions_impl = taskFunctions_teacher;
}

void gpuRun(GPUMemory&mem,CommandBuffer const&cb){
  if(!taskFunctions_impl.gpu_run)return;
  taskFunctions_impl.gpu_run(mem,cb);
}

void prepareModel(GPUMemory&mem,CommandBuffer&cb,Model const&model){
  if(!taskFunctions_impl.prepareModel)return;
  taskFunctions_impl.prepareModel(mem,cb,model);
}

void drawModel_vertexShader(OutVertex&outVertex,InVertex const&inVertex,ShaderInterface const&si){
  if(!taskFunctions_impl.drawModel_vertexShader)return;
  taskFunctions_impl.drawModel_vertexShader(outVertex,inVertex,si);
}

void drawModel_fragmentShader(OutFragment&outFragment,InFragment const&inFragment,ShaderInterface const&si){
  if(!taskFunctions_impl.drawModel_fragmentShader)return;
  taskFunctions_impl.drawModel_fragmentShader(outFragment,inFragment,si);
}

glm::vec4 magentaTexture(glm::vec2 const&uv){
  auto m = (uv.x>0.5f) != (uv.y>0.5f);
  return glm::vec4(m,0.f,m,1.f);
}

glm::vec4 magentaTexture(glm::uvec2 const&uv){
  auto m = uv.x%2 != uv.y%2;
  return glm::vec4(m,0.f,m,1.f);
}

glm::vec4 read_texture(Texture const&texture,glm::vec2 const&uv){
  if(!taskFunctions_impl.read_texture)return magentaTexture(uv);
  return taskFunctions_impl.read_texture(texture,uv);
}

glm::vec4 read_textureClamp(Texture const&texture,glm::vec2 const&uv){
  if(!taskFunctions_impl.read_textureClamp)return magentaTexture(uv);
  return taskFunctions_impl.read_textureClamp(texture,uv);
}

glm::vec4 texelFetch       (Texture const&texture,glm::uvec2 const&uv){
  if(!taskFunctions_impl.texelFetch)return magentaTexture(uv);
  return taskFunctions_impl.texelFetch(texture,uv);
}
