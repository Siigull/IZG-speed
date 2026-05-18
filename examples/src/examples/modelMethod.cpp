/*!
 * @file
 * @brief This file contains implementation of model visualizer
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <framework/model.hpp>
#include <framework/programContext.hpp>
#include <solutionInterface/taskFunctions.hpp>
#include <solutionInterface/uniformLocations.hpp>
#include <examples/modelMethod.hpp>

namespace modelMethod{

/**
 * @brief Constructor
 */
Method::Method(GPUMemory&m,MethodConstructionData const*): ::Method(m){
  modelData.load(ProgramContext::get().args.modelFile);
  modelData.createModelView(model);

  prepareModel(mem,modelCB,model);

  mem.programs[0].vertexShader   = drawModel_vertexShader;
  mem.programs[0].fragmentShader = drawModel_fragmentShader;
  mem.programs[0].vs2fs[0]       = AttribType::VEC3;
  mem.programs[0].vs2fs[1]       = AttribType::VEC3;
  mem.programs[0].vs2fs[2]       = AttribType::VEC2;

  BlendingSettings settings;
  settings.enabled = true                          ;
  settings.sFactor = BlendFunc::SRC_ALPHA          ;
  settings.dFactor = BlendFunc::ONE_MINUS_SRC_ALPHA;
  pushSetBlendingCommand(drawCB,settings);

  pushClearColorCommand(drawCB,glm::vec4(0.1,0.15,0.1,1.));
  pushClearDepthCommand(drawCB,10e10f);
  pushBindProgramCommand(drawCB,0);
  pushSubCommand(drawCB,&modelCB);

}


/**
 * @brief This function is called every frame and should render a model
 *
 * @param proj projection matrix
 * @param view view matrix
 * @param light light position
 * @param camera camera position
 */
void Method::onDraw(SceneParam const&sceneParam){
  mem.uniforms[getUniformLocation(0,PROJECTION_VIEW_MATRIX)].m4 = sceneParam.proj * sceneParam.view;
  mem.uniforms[getUniformLocation(0,LIGHT_POSITION        )].v3 = sceneParam.light;
  mem.uniforms[getUniformLocation(0,CAMERA_POSITION       )].v3 = sceneParam.camera;
  mem.uniforms[getUniformLocation(0,SHADOWMAP_ID          )].i1 = -1;
  mem.uniforms[getUniformLocation(0,AMBIENT_LIGHT_COLOR   )].v3 = glm::vec3(0.2f);
  mem.uniforms[getUniformLocation(0,LIGHT_COLOR           )].v3 = glm::vec3(1.f);

  gpuRun(mem,drawCB);
}

REGISTER_METHOD("model loader");

}
