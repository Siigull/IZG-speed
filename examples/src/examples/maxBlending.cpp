/*!
 * @file
 * @brief This file contains implementation of max blending
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <framework/programContext.hpp>

namespace maxBlending{

/**
 * @brief This class represents 3D texture rendering method
 */
class Method: public ::Method{
  public:
    Method(GPUMemory&m,MethodConstructionData const*mcd);
    virtual ~Method();
    virtual void onDraw(SceneParam const&sceneParam) override;
    CommandBuffer commandBuffer;
};


/**
 * @brief This function represents vertex shader of 3D triangle rendering method.
 *
 * @param outVertex output vertex
 * @param inVertex input vertex
 * @param uniforms uniform variables
 */
void vertexShader(OutVertex&outVertex,InVertex const&inVertex,ShaderInterface const&si){
  outVertex.gl_Position = glm::vec4(0.f,0.f,0.f,1.f);

  struct Vertex{
    glm::vec4 pos;
    glm::vec4 col;
  };
  Vertex const verts[] = {
    Vertex{glm::vec4(0,0,0,1),glm::vec4(1.0,0.0,0.0,1.)},
    Vertex{glm::vec4(1,0,0,1),glm::vec4(0.0,1.0,0.0,1.)},
    Vertex{glm::vec4(0,1,0,1),glm::vec4(0.0,0.0,1.0,1.)},
  };

  if(inVertex.gl_VertexID < sizeof(verts)/sizeof(Vertex)){
    outVertex.gl_Position      = verts[inVertex.gl_VertexID].pos;
    outVertex.attributes[0].v4 = verts[inVertex.gl_VertexID].col;
  }
}

/**
 * @brief This functionrepresents fragment shader of 3D triangle rendering method.
 *
 * @param outFragment output fragment
 * @param inFragment input fragment
 * @param uniforms uniform variables
 */
void fragmentShader(OutFragment&outFragment,InFragment const&inFragment,ShaderInterface const&){
  outFragment.gl_FragColor = inFragment.attributes[0].v4;
}

/**
 * @brief Constructor
 */
Method::Method(GPUMemory&m,MethodConstructionData const*): ::Method(m){
  mem.programs[0].vertexShader   = vertexShader;
  mem.programs[0].fragmentShader = fragmentShader;
  mem.programs[0].vs2fs[0] = AttribType::VEC4;

  pushClearColorCommand (commandBuffer,glm::vec4(.8,.8,.8,1));
  pushClearDepthCommand (commandBuffer,10e10f);
  pushBindProgramCommand(commandBuffer,0);
  BlendingSettings bs;
  bs.enabled  = true;
  bs.sFactor  = BlendFunc::ONE;
  bs.dFactor  = BlendFunc::ONE;
  bs.equation = BlendEquation::MAX;
  pushSetBlendingCommand(commandBuffer,bs);
  pushDrawCommand       (commandBuffer,3 );
}

/**
 * @brief This function is called every frame and should render 3D triangle
 *
 * @param proj projection matrix
 * @param view view matrix
 * @param light light position
 * @param camera camera position
 */
void Method::onDraw(SceneParam const&){
  gpuRun(mem,commandBuffer);
}

/**
 * @brief Descturctor
 */
Method::~Method(){}

REGISTER_METHOD("Max Blending");

}
