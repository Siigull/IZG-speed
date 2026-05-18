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

#include <examples/shadowModel.hpp>

namespace mirrorMethod{

struct Vertex{
  glm::vec3 data ;
  glm::vec3 color;
};

void drawMirror_vs(OutVertex&out,InVertex const&in,ShaderInterface const&si){
  auto vID = in.gl_VertexID;
  auto&gl_Position = out.gl_Position;
  auto pv = si.uniforms[getUniformLocation(si.gl_DrawID,PROJECTION_VIEW_MATRIX)].m4;

  float width = 100.f;
  float height = 200.f;
  float wh = width/2.f;
  float hh = height/2.f;
  if(vID==0)gl_Position = pv*glm::vec4(0.f,-1.f*hh,+1.f*wh,1.f);
  if(vID==1)gl_Position = pv*glm::vec4(0.f,-1.f*hh,-1.f*wh,1.f);
  if(vID==2)gl_Position = pv*glm::vec4(0.f,+1.f*hh,+1.f*wh,1.f);
  if(vID==3)gl_Position = pv*glm::vec4(0.f,+1.f*hh,+1.f*wh,1.f);
  if(vID==4)gl_Position = pv*glm::vec4(0.f,-1.f*hh,-1.f*wh,1.f);
  if(vID==5)gl_Position = pv*glm::vec4(0.f,+1.f*hh,-1.f*wh,1.f);
}

void drawMirror_fs(OutFragment&out,InFragment const&,ShaderInterface const&){
  out.gl_FragColor = glm::vec4(0.1f,0.1f,0.1f,1.f);
}

void drawMirrorBlue_fs(OutFragment&out,InFragment const&,ShaderInterface const&){
  out.gl_FragColor = glm::vec4(0.5f,0.5f,1.0f,0.1f);
}

class Method: public ::Method{
  public:
    Method(GPUMemory&m,MethodConstructionData const*cd = nullptr);
    virtual ~Method();
    virtual void onDraw(SceneParam const&sceneParam) override;
    virtual void onUpdate(float dt) override;
    float time = 0;

    ModelData     modelData;
    Model         model;

    CommandBuffer modelCB;
    CommandBuffer drawCB;
    struct UserData{
      GPUMemory*mem = nullptr;
    }userData;
};

using namespace glm;

#ifndef CMAKE_ROOT_DIR
#define CMAKE_ROOT_DIR "."
#endif

const float stepSize = 80;

void moveModelLeft(void*data){
  auto&uData=*(Method::UserData*)data;
  auto m = glm::translate(glm::vec3(-stepSize,0,0));
  auto&pv = uData.mem->uniforms[getUniformLocation(0,PROJECTION_VIEW_MATRIX)].m4;
  pv = pv * m;
}

void moveModelRight(void*data){
  auto&uData=*(Method::UserData*)data;
  auto m = glm::translate(glm::vec3(+stepSize,0,0));
  auto&pv = uData.mem->uniforms[getUniformLocation(0,PROJECTION_VIEW_MATRIX)].m4;
  pv = pv * m;
}

void mirrorModel(void*data){
  auto&uData=*(Method::UserData*)data;
  auto m = glm::scale(glm::vec3(-1.f,1.f,1.f));
  auto&pv = uData.mem->uniforms[getUniformLocation(0,PROJECTION_VIEW_MATRIX)].m4;
  pv = pv * m;
}

void flipMirror(void*data){
  auto&uData=*(Method::UserData*)data;
  auto m = glm::scale(glm::vec3(1.f,-1.f,-1.f));
  uData.mem->uniforms[0].m4 = uData.mem->uniforms[0].m4 * m;
}

Method::Method(GPUMemory&m,MethodConstructionData const*): ::Method(m){

  modelData.load(CMAKE_ROOT_DIR "/resources/models/young_priestess/scene.gltf");
  modelData.createModelView(model);
  prepareModel(mem,modelCB,model);

  auto&prg0 = m.programs[0];
  prg0.vertexShader   = drawMirror_vs;
  prg0.fragmentShader = drawMirror_fs;

  auto&prg1 = m.programs[1];
  prg1.vertexShader   = drawModel_vertexShader;
  prg1.fragmentShader = drawModel_fragmentShader;
  prg1.vs2fs[0] = AttribType::VEC3;
  prg1.vs2fs[1] = AttribType::VEC3;
  prg1.vs2fs[2] = AttribType::VEC2;
  prg1.vs2fs[3] = AttribType::VEC4;

  auto&prg2 = m.programs[2];
  prg2.vertexShader   = drawMirror_vs;
  prg2.fragmentShader = drawMirrorBlue_fs;

  StencilSettings ignoreStencil;
  ignoreStencil.frontOps. sfail = StencilOp::KEEP;
  ignoreStencil.frontOps.dpfail = StencilOp::KEEP;
  ignoreStencil.frontOps.dppass = StencilOp::KEEP;
  ignoreStencil. backOps. sfail = StencilOp::KEEP;
  ignoreStencil. backOps.dpfail = StencilOp::KEEP;
  ignoreStencil. backOps.dppass = StencilOp::KEEP;
  ignoreStencil.enabled         = false;
  ignoreStencil.func            = StencilFunc::ALWAYS;
  ignoreStencil.refValue        = 0;

  StencilSettings drawToStencil;
  drawToStencil.frontOps. sfail = StencilOp::INCR;
  drawToStencil.frontOps.dpfail = StencilOp::INCR;
  drawToStencil.frontOps.dppass = StencilOp::INCR;
  drawToStencil. backOps. sfail = StencilOp::INCR;
  drawToStencil. backOps.dpfail = StencilOp::INCR;
  drawToStencil. backOps.dppass = StencilOp::INCR;
  drawToStencil.enabled         = true;
  drawToStencil.func            = StencilFunc::ALWAYS;
  drawToStencil.refValue        = 0;

  StencilSettings useStencil;
  useStencil.frontOps. sfail = StencilOp::KEEP;
  useStencil.frontOps.dpfail = StencilOp::KEEP;
  useStencil.frontOps.dppass = StencilOp::KEEP;
  useStencil. backOps. sfail = StencilOp::KEEP;
  useStencil. backOps.dpfail = StencilOp::KEEP;
  useStencil. backOps.dppass = StencilOp::KEEP;
  useStencil.enabled         = true;
  useStencil.func            = StencilFunc::EQUAL;
  useStencil.refValue        = 1;

  userData.mem = &this->mem;

  BlendingSettings settings;
  settings.enabled = true                          ;
  settings.sFactor = BlendFunc::SRC_ALPHA          ;
  settings.dFactor = BlendFunc::ONE_MINUS_SRC_ALPHA;
  pushSetBlendingCommand(drawCB,settings);

  //clear frame
  pushBindFramebufferCommand   (drawCB,0);
  pushClearColorCommand        (drawCB,glm::vec4(0x00,0x20,0x20,0xff)/255.f);
  pushClearDepthCommand        (drawCB);
  pushClearStencilCommand      (drawCB);

  //draw scene
  pushUserCommand              (drawCB,moveModelRight,&userData);
  pushBindProgramCommand       (drawCB,1);
  pushSetDrawIdCommand         (drawCB,0);
  pushBlockWritesCommand       (drawCB,false,false,true);
  pushSetFrontFaceCommand      (drawCB,true);
  pushSetStencilCommand        (drawCB,ignoreStencil);
  pushSubCommand               (drawCB,&modelCB);

  //draw mirror
  pushUserCommand              (drawCB,moveModelLeft,&userData);
  pushBindProgramCommand       (drawCB,0);
  pushBlockWritesCommand       (drawCB,true,true,false);
  pushSetStencilCommand        (drawCB,drawToStencil);
  pushSetBackfaceCullingCommand(drawCB,true);
  pushDrawCommand              (drawCB,6);

  //draw mirror image
  pushUserCommand              (drawCB,mirrorModel,&userData);
  pushUserCommand              (drawCB,moveModelRight,&userData);
  pushBindProgramCommand       (drawCB,1);
  pushSetDrawIdCommand         (drawCB,0);
  pushBlockWritesCommand       (drawCB,false,false,true);
  pushSetFrontFaceCommand      (drawCB,false);
  pushSetStencilCommand        (drawCB,useStencil);
  pushSubCommand               (drawCB,&modelCB);

  //draw back side of the mirror
  pushUserCommand              (drawCB,moveModelLeft,&userData);
  pushBindProgramCommand       (drawCB,0);
  pushBlockWritesCommand       (drawCB,false,false,true);
  pushSetStencilCommand        (drawCB,ignoreStencil);
  pushSetBackfaceCullingCommand(drawCB,true);
  pushSetFrontFaceCommand      (drawCB,false);
  pushDrawCommand              (drawCB,6);

  //draw blue tint over the mirror
  pushBindProgramCommand       (drawCB,2);
  pushBlockWritesCommand       (drawCB,false,false,true);
  pushSetStencilCommand        (drawCB,ignoreStencil);
  pushSetFrontFaceCommand      (drawCB,true);
  pushDrawCommand              (drawCB,6);

}

Method::~Method(){
  free(model);
}

void Method::onUpdate(float dt){
  time += dt;
}

void Method::onDraw(SceneParam const&sceneParam){

  auto view = sceneParam.view;

  mem.uniforms[getUniformLocation(0,PROJECTION_VIEW_MATRIX)].m4 = sceneParam.proj*view*glm::scale(glm::vec3(.1f));
  mem.uniforms[getUniformLocation(0,LIGHT_POSITION        )].v3 = glm::vec3(10.f);
  mem.uniforms[getUniformLocation(0,CAMERA_POSITION       )].v3 = glm::vec3(glm::inverse(view)*glm::vec4(0.f,0.f,0.f,1.f));
  mem.uniforms[getUniformLocation(0,SHADOWMAP_ID          )].i1 = -1;
  mem.uniforms[getUniformLocation(0,AMBIENT_LIGHT_COLOR   )].v3 = glm::vec3(1.f,1.f,1.0f);
  mem.uniforms[getUniformLocation(0,LIGHT_COLOR           )].v3 = glm::vec3(0.f);

  mem.uniforms[getUniformLocation(0,FREE_UNIFORMS_START+0 )].v1 = time;
  gpuRun(mem,drawCB);
}

REGISTER_METHOD("mirror");

}
