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

namespace stencilTriangleMethod{

void drawTriangle_vs(OutVertex&out,InVertex const&in,ShaderInterface const&si){
  auto gl_VertexID = in.gl_VertexID;

  float time = si.uniforms[getUniformLocation(0,FREE_UNIFORMS_START+0 )].v1;
  float o = glm::radians(120.f);
  float a = time;
  if(gl_VertexID==0)out.gl_Position = glm::vec4(glm::cos(a+o*0.f),glm::sin(a+o*0.f),0,1);
  if(gl_VertexID==1)out.gl_Position = glm::vec4(glm::cos(a+o*1.f),glm::sin(a+o*1.f),0,1);
  if(gl_VertexID==2)out.gl_Position = glm::vec4(glm::cos(a+o*2.f),glm::sin(a+o*2.f),0,1);
}

void drawTriangle_fs(OutFragment&out,InFragment const&,ShaderInterface const&){
  out.gl_FragColor = glm::vec4(1.f);
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
};

using namespace glm;

#ifndef CMAKE_ROOT_DIR
#define CMAKE_ROOT_DIR "."
#endif

Method::Method(GPUMemory&m,MethodConstructionData const*): ::Method(m){

  modelData.load(CMAKE_ROOT_DIR "/resources/models/young_priestess/scene.gltf");
  modelData.createModelView(model);
  prepareModel(mem,modelCB,model);

  auto&prg0 = m.programs[0];
  prg0.vertexShader   = drawTriangle_vs;
  prg0.fragmentShader = drawTriangle_fs;

  auto&prg1 = m.programs[1];
  prg1.vertexShader   = drawModel_vertexShader;
  prg1.fragmentShader = drawModel_fragmentShader;
  prg1.vs2fs[0] = AttribType::VEC3;
  prg1.vs2fs[1] = AttribType::VEC3;
  prg1.vs2fs[2] = AttribType::VEC2;
  prg1.vs2fs[3] = AttribType::VEC4;

  StencilSettings drawToStencil;
  drawToStencil.frontOps. sfail = StencilOp::INCR;
  drawToStencil.frontOps.dpfail = StencilOp::INCR;
  drawToStencil.frontOps.dppass = StencilOp::INCR;
  drawToStencil. backOps. sfail = StencilOp::INCR;
  drawToStencil. backOps.dpfail = StencilOp::INCR;
  drawToStencil. backOps.dppass = StencilOp::INCR;
  drawToStencil.enabled = true;
  drawToStencil.func =  StencilFunc::ALWAYS;
  drawToStencil.refValue = 0;

  StencilSettings useStencil;
  useStencil.frontOps. sfail = StencilOp::KEEP;
  useStencil.frontOps.dpfail = StencilOp::KEEP;
  useStencil.frontOps.dppass = StencilOp::KEEP;
  useStencil. backOps. sfail = StencilOp::KEEP;
  useStencil. backOps.dpfail = StencilOp::KEEP;
  useStencil. backOps.dppass = StencilOp::KEEP;
  useStencil.enabled = true;
  useStencil.func =  StencilFunc::EQUAL;
  useStencil.refValue = 1;

  BlendingSettings settings;
  settings.enabled = true                          ;
  settings.sFactor = BlendFunc::SRC_ALPHA          ;
  settings.dFactor = BlendFunc::ONE_MINUS_SRC_ALPHA;
  pushSetBlendingCommand(drawCB,settings);

  //draw scene
  pushBindFramebufferCommand(drawCB,0);
  pushClearColorCommand     (drawCB,glm::vec4(0xd4,0x6d,0x63,0xff)/255.f);
  pushClearDepthCommand     (drawCB);
  pushClearStencilCommand   (drawCB);

  pushBindProgramCommand    (drawCB,0);
  pushBlockWritesCommand    (drawCB,true,true,false);
  pushSetStencilCommand     (drawCB,drawToStencil);
  pushDrawCommand           (drawCB,3);

  pushBindProgramCommand    (drawCB,1);
  pushSetDrawIdCommand      (drawCB,0);
  pushBlockWritesCommand    (drawCB,false,false,true);
  pushSetStencilCommand     (drawCB,useStencil);
  pushSubCommand            (drawCB,&modelCB);

}

Method::~Method(){
  free(model);
}

void Method::onUpdate(float dt){
  time += dt;
}

void Method::onDraw(SceneParam const&sceneParam){

  auto view = glm::lookAt(glm::vec3(140.f*cos(time),-50.f,140.f*sin(time)),glm::vec3(0.f,-50.f,0.f),glm::vec3(0,1,0));

  mem.uniforms[getUniformLocation(0,PROJECTION_VIEW_MATRIX)].m4 = sceneParam.proj*view;
  mem.uniforms[getUniformLocation(0,LIGHT_POSITION        )].v3 = glm::vec3(10.f);
  mem.uniforms[getUniformLocation(0,CAMERA_POSITION       )].v3 = glm::vec3(glm::inverse(view)*glm::vec4(0.f,0.f,0.f,1.f));//sceneParam.camera;
  mem.uniforms[getUniformLocation(0,SHADOWMAP_ID          )].i1 = -1;
  mem.uniforms[getUniformLocation(0,AMBIENT_LIGHT_COLOR   )].v3 = glm::vec3(1.f,1.f,1.0f);
  mem.uniforms[getUniformLocation(0,LIGHT_COLOR           )].v3 = glm::vec3(0.f);

  mem.uniforms[getUniformLocation(0,FREE_UNIFORMS_START+0 )].v1 = time;
  gpuRun(mem,drawCB);
}

REGISTER_METHOD("stencil triangle");

}
