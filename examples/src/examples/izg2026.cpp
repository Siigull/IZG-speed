#include <glm/ext/quaternion_common.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <framework/programContext.hpp>
#include <framework/model.hpp>

#include <solutionInterface/taskFunctions.hpp>
#include <solutionInterface/uniformLocations.hpp>
#include <examples/izg2026.hpp>

namespace izg2026{

using namespace glm;

void createShadowMap_vs(OutVertex&outVertex,InVertex const&inVertex,ShaderInterface const&si){
  auto const&vp    = si.uniforms[getUniformLocation(si.gl_DrawID,CREATE_SHADOW_MAP_MATRIX)].m4;
  auto const&model = si.uniforms[getUniformLocation(si.gl_DrawID,MODEL_MATRIX            )].m4;
  
  outVertex.gl_Position = vp*model*vec4(inVertex.attributes[0].v3,1.f);
}

void createShadowMap_fs(OutFragment&,InFragment const&,ShaderInterface const&){
}

#ifndef CMAKE_ROOT_DIR
#define CMAKE_ROOT_DIR "."
#endif

Method::Method(GPUMemory&m,MethodConstructionData const*): ::Method(m){
  shadowMap = TextureData(1024,1024,1,Image::F32);

  modelData.load(CMAKE_ROOT_DIR "/resources/models/izg2026.glb");
  modelData.createModelView(model);
  prepareModel(mem,modelCB,model);

  auto&prg0 = m.programs[0];
  prg0.vertexShader   = createShadowMap_vs;
  prg0.fragmentShader = createShadowMap_fs;

  auto&prg1 = m.programs[1];
  prg1.vertexShader   = drawModel_vertexShader;//scene_vs;
  prg1.fragmentShader = drawModel_fragmentShader;//scene_fs;
  prg1.vs2fs[0] = AttribType::VEC3;
  prg1.vs2fs[1] = AttribType::VEC3;
  prg1.vs2fs[2] = AttribType::VEC2;
  prg1.vs2fs[3] = AttribType::VEC4;

  m.textures[shadowMapId] = shadowMap.getTexture();
  m.framebuffers[1].depth  = m.textures[shadowMapId].img;
  m.framebuffers[1].width  = m.textures[shadowMapId].width;
  m.framebuffers[1].height = m.textures[shadowMapId].height;

  //draw shadow map
  pushBindFramebufferCommand(drawCB,1);
  pushBindProgramCommand    (drawCB,0);
  pushClearColorCommand     (drawCB);
  pushClearDepthCommand     (drawCB);
  pushSubCommand            (drawCB,&modelCB);

  //draw scene
  pushBindFramebufferCommand(drawCB,0);
  pushBindProgramCommand    (drawCB,1);
  pushClearColorCommand     (drawCB,glm::vec4(0.1f,0.1f,.1f,1.f));
  pushClearDepthCommand     (drawCB);
  pushSetDrawIdCommand      (drawCB,0);
  BlendingSettings bs;
  bs.enabled = true;
  bs.dFactor = BlendFunc::ONE_MINUS_SRC_ALPHA;
  bs.sFactor = BlendFunc::SRC_ALPHA;
  pushSetBlendingCommand    (drawCB,bs);
  pushSubCommand            (drawCB,&modelCB);

  lightProj = glm::ortho(-30.f,+30.f,-30.f,+30.f,0.f,1000.f);
  lightBias = glm::scale(glm::vec3(.5f,.5f,1.f))*glm::translate(glm::vec3(1,1,0));
}

Method::~Method(){
  free(model);
}

void Method::onUpdate(float dt){
  time += dt;
}

void Method::onDraw(SceneParam const&sceneParam){
  float atime = 2;
  auto lightPos = glm::vec3(30*glm::cos(atime),30,30*glm::sin(atime));
  auto lightView = glm::lookAt(lightPos,glm::vec3(0,0,0),glm::vec3(0,1,0));

  auto view = sceneParam.view;//cameraglm::lookAt(glm::vec3(69.f,51.f,127.f),glm::vec3(0.f),glm::vec3(0,1,0));

  mem.uniforms[getUniformLocation(0,PROJECTION_VIEW_MATRIX  )].m4 = sceneParam.proj*view;//sceneParam.view;
  mem.uniforms[getUniformLocation(0,LIGHT_POSITION          )].v3 = lightPos;
  mem.uniforms[getUniformLocation(0,CAMERA_POSITION         )].v3 = glm::vec3(glm::inverse(view)*glm::vec4(0.f,0.f,0.f,1.f));
  mem.uniforms[getUniformLocation(0,USE_SHADOW_MAP_MATRIX   )].m4 = lightBias*lightProj*lightView;
  mem.uniforms[getUniformLocation(0,CREATE_SHADOW_MAP_MATRIX)].m4 = lightProj*glm::translate(glm::vec3(0.f,0.f,-0.5f))*lightView;
  mem.uniforms[getUniformLocation(0,SHADOWMAP_ID            )].i1 = shadowMapId;
  mem.uniforms[getUniformLocation(0,AMBIENT_LIGHT_COLOR     )].v3 = glm::vec3(0.4f,0.f,0.2f);
  mem.uniforms[getUniformLocation(0,LIGHT_COLOR             )].v3 = glm::vec3(1.f);
  gpuRun(mem,drawCB);
}

REGISTER_METHOD("izg2026");

}
