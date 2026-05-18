#include<iostream>

#include<framework/systemSpecific.hpp>
#include<solutionInterface/gpu.hpp>
#include<solutionInterface/taskFunctions.hpp>

#ifndef CMAKE_ROOT_DIR
#define CMAKE_ROOT_DIR "."
#endif//CMAKE_ROOT_DIR

#include<framework/systemSpecificWindows.inl>
#include<framework/systemSpecificLinux.inl>

extern TaskFunctions taskFunctions_teacher;

namespace functionNames{
  auto gpu_run                  = "teacher_gpu_run"                 ;
  auto prepareModel             = "teacher_prepareModel"            ;
  auto drawModel_vertexShader   = "teacher_drawModel_vertexShader"  ;
  auto drawModel_fragmentShader = "teacher_drawModel_fragmentShader";
  auto read_texture             = "teacher_read_texture"            ;
  auto read_textureClamp        = "teacher_read_textureClamp"       ;
  auto texelFetch               = "teacher_texelFetch"              ;
  auto breakable_gpu_run        = "breakable_gpu_run"               ;
  auto idToBreak                = "idToBreak"                       ;
  auto lineToBreak              = "lineToBreak"                     ;
}

auto loadFunctionWithErrorChecking(char const*n){
  auto fce = loadFunction(n);
  if(!fce){
    std::cerr << "ERROR: could not load function: " << n << "!" << std::endl;
    std::cerr << "Reference implementation will not be available!" << std::endl;
  }
  return fce;
}

System::System(){
  loadLibrary(libPath);
  if(!library){
    std::cerr << "ERROR: cannot open library: " << libPath << std::endl;
    std::cerr << "Reference solution will not be available!" << std::endl;
    return;
  }
  taskFunctions_teacher.gpu_run                  = (GPU_Run          )loadFunctionWithErrorChecking(functionNames::gpu_run                 );
  taskFunctions_teacher.prepareModel             = (PrepareModel     )loadFunctionWithErrorChecking(functionNames::prepareModel            );
  taskFunctions_teacher.drawModel_vertexShader   = (VertexShader     )loadFunctionWithErrorChecking(functionNames::drawModel_vertexShader  );
  taskFunctions_teacher.drawModel_fragmentShader = (FragmentShader   )loadFunctionWithErrorChecking(functionNames::drawModel_fragmentShader);
  taskFunctions_teacher.read_texture             = (Read_Texture     )loadFunctionWithErrorChecking(functionNames::read_texture            );
  taskFunctions_teacher.read_textureClamp        = (Read_TextureClamp)loadFunctionWithErrorChecking(functionNames::read_textureClamp       );
  taskFunctions_teacher.texelFetch               = (TexelFetch       )loadFunctionWithErrorChecking(functionNames::texelFetch              );
  taskFunctions_teacher.breakable_gpu_run        = (GPU_Run          )loadFunctionWithErrorChecking(functionNames::breakable_gpu_run       );
  taskFunctions_teacher.idToBreak                = (size_t*          )loadFunctionWithErrorChecking(functionNames::idToBreak               );
  taskFunctions_teacher.lineToBreak              = (size_t*          )loadFunctionWithErrorChecking(functionNames::lineToBreak             );
  generalSystemSettings();
}

System::~System(){
  unloadLibrary();
}
