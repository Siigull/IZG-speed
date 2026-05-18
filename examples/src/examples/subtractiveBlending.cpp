/*!
 * @file
 * @brief This file contains implementation of subtractive blending
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <framework/programContext.hpp>

namespace subtractiveBlending{

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

  glm::vec4 const verts[] = {
    glm::vec4(0,0,0,1)+glm::vec4(0,0,0,0),
    glm::vec4(1,0,0,1)+glm::vec4(0,0,0,0),
    glm::vec4(0,1,0,1)+glm::vec4(0,0,0,0),

    glm::vec4(0,0,0,1)+glm::vec4(0.1,0.1,-0.1,0),
    glm::vec4(1,0,0,1)+glm::vec4(0.1,0.1,-0.1,0),
    glm::vec4(0,1,0,1)+glm::vec4(0.1,0.1,-0.1,0),

    glm::vec4(0,0,0,1)+glm::vec4(0.2,0.2,-0.2,0),
    glm::vec4(1,0,0,1)+glm::vec4(0.2,0.2,-0.2,0),
    glm::vec4(0,1,0,1)+glm::vec4(0.2,0.2,-0.2,0),
  };

  if(inVertex.gl_VertexID < sizeof(verts)/sizeof(glm::vec4))
    outVertex.gl_Position = verts[inVertex.gl_VertexID];
}

/**
 * @brief This functionrepresents fragment shader of 3D triangle rendering method.
 *
 * @param outFragment output fragment
 * @param inFragment input fragment
 * @param uniforms uniform variables
 */
void fragmentShader(OutFragment&outFragment,InFragment const&inFragment,ShaderInterface const&){
  outFragment.gl_FragColor = glm::vec4(0.1f,0.1f,0.1f,1.f);
}

/**
 * @brief Constructor
 */
Method::Method(GPUMemory&m,MethodConstructionData const*): ::Method(m){
  mem.programs[0].vertexShader   = vertexShader;
  mem.programs[0].fragmentShader = fragmentShader;

  pushClearColorCommand (commandBuffer,glm::vec4(1.,1.,1.,1));
  pushClearDepthCommand (commandBuffer,10e10f);
  pushBindProgramCommand(commandBuffer,0);
  BlendingSettings bs;
  bs.enabled  = true;
  bs.sFactor  = BlendFunc::ONE;
  bs.dFactor  = BlendFunc::ONE;
  bs.equation = BlendEquation::REVERSE_SUBTRACT;
  pushSetBlendingCommand(commandBuffer,bs);
  pushDrawCommand       (commandBuffer,9 );
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

REGISTER_METHOD("Subtractive Blending");

}
