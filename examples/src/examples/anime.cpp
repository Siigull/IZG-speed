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

namespace animeMethod{

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

  auto&prg1 = m.programs[0];
  prg1.vertexShader   = drawModel_vertexShader;
  prg1.fragmentShader = drawModel_fragmentShader;
  prg1.vs2fs[0] = AttribType::VEC3;
  prg1.vs2fs[1] = AttribType::VEC3;
  prg1.vs2fs[2] = AttribType::VEC2;
  prg1.vs2fs[3] = AttribType::VEC4;

  //draw scene
  pushBindFramebufferCommand(drawCB,0);
  pushBindProgramCommand    (drawCB,0);
  pushClearColorCommand     (drawCB,glm::vec4(0xd4,0x6d,0x63,0xff)/255.f);
  pushClearDepthCommand     (drawCB,10e10f);
  pushSetDrawIdCommand      (drawCB,0);
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
  gpuRun(mem,drawCB);
}

REGISTER_METHOD("anime - somebody suggested I should include it...");

}
