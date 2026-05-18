/*!
 * @file
 * @brief This file contains application class implementation
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <assert.h>
#include <framework/application.hpp>
#include <framework/switchSolution.hpp>



void defaultSceneParameters(
    basicCamera::OrbitCamera&orbit,
    basicCamera::PerspectiveCamera&proj,
    glm::vec3&light,
    uint32_t w,uint32_t h){
  orbit.addDistance(35.f);
  orbit.addYAngle(glm::radians(-20.f));
  proj.setNear(0.1f);
  proj.setFovy(glm::pi<float>()/4.f);
  auto const aspect = static_cast<float>(w) / static_cast<float>(h);
  proj.setAspect(aspect);
  light  = glm::vec3(100.f,100.f,100.f);
}


Framebuffer getDefaultFramebuffer(SDL_Surface*surface,float*depth,uint8_t*stencil){
  uint32_t const bitsPerByte = 8;

  auto f  = surface->format;
  auto fd = SDL_GetPixelFormatDetails(f);
  Framebuffer fbo;
  fbo.yReversed       = true      ;
  fbo.width           = surface->w;
  fbo.height          = surface->h;

  auto&c = fbo.color;
  c.bytesPerPixel   = fd->bytes_per_pixel;
  c.channels        = 3                                        ;
  c.channelTypes[0] = (Image::Channel)(fd->Rshift / bitsPerByte);
  c.channelTypes[1] = (Image::Channel)(fd->Gshift / bitsPerByte);
  c.channelTypes[2] = (Image::Channel)(fd->Bshift / bitsPerByte);
  c.data            = surface->pixels                          ;
  c.pitch           = surface->pitch                           ;
  c.format          = Image::U8                                ;

  auto&d = fbo.depth;
  d.bytesPerPixel = sizeof(float)            ;
  d.channels      = 1                        ;
  d.data          = depth                    ;
  d.pitch         = fbo.width*d.bytesPerPixel;
  d.format        = Image::F32               ;

  auto&s = fbo.stencil;
  s.bytesPerPixel = sizeof(uint8_t)          ;
  s.channels      = 1                        ;
  s.data          = stencil                  ;
  s.pitch         = fbo.width*s.bytesPerPixel;
  s.format        = Image::U8                ;
  return fbo;
}



/**
 * @brief Constructor
 *
 * @param width width of the window
 * @param height height of the window
 */
Application::Application(int32_t width,int32_t height):Window(width,height,"izgProject"),windowSize(width,height){
  setIdleCallback([&](){idle();});
  setCallback(SDL_EVENT_WINDOW_RESIZED,[&](SDL_Event const&event){resize     (event);});
  setCallback(SDL_EVENT_MOUSE_MOTION  ,[&](SDL_Event const&event){mouseMotion(event);});
  setCallback(SDL_EVENT_KEY_DOWN      ,[&](SDL_Event const&event){keyDown    (event);});
  defaultSceneParameters(orbitCamera,perspectiveCamera,light,width,height);
  timer.reset();
}

/**
 * @brief Destructor
 */
Application::~Application(){}

    
/**
 * @brief Starts the main loop
 */
void Application::start(){
  mainLoop();
}

/**
 * @brief This function selects a rendering method
 *
 * @param m method id
 */
void Application::setMethod(uint32_t m){
  auto&mr=ProgramContext::get().methods;
  auto const nofMethods = mr.methodFactories.size();
  if(m >= nofMethods) m = static_cast<uint32_t>(nofMethods - 1);
  mr.selectedMethod = m;
}

void Application::resize(){
  depthMemoryBacking  .resize(surface->w*surface->h);
  stencilMemoryBacking.resize(surface->w*surface->h);
  defaultFramebuffer = getDefaultFramebuffer(surface,depthMemoryBacking.data(),stencilMemoryBacking.data());
  mem.framebuffers[mem.defaultFramebuffer] = defaultFramebuffer;
}

extern bool useTeacherSolution;

void Application::updateWindowTitle(){
  auto&mr=ProgramContext::get().methods;
  std::stringstream title;
  if(useTeacherSolution)title << "TEACHER";
  else                  title << "STUDENT";
  title << ": " << mr.methodNames.at(mr.selectedMethod);
  SDL_SetWindowTitle(getWindow(),title.str().c_str());
}

void Application::computeUpdateFlags(){
  if(useTeacherSolution == teacherSolutionSeen)return;
  shouldUpdateTitle     = true;
  shouldClearSDLSurface = true;
  teacherSolutionSeen = useTeacherSolution;
}

void Application::conditionalUpdateWindowTitle(){
  if(!shouldUpdateTitle)return;
  updateWindowTitle();
  shouldUpdateTitle = false;
}

void Application::conditionalClearSDLSurface(){
  if(!shouldClearSDLSurface)return;
  SDL_FillSurfaceRect(surface, NULL, 0x000000);
  shouldClearSDLSurface = false;
}

void Application::createMethodIfItDoesNotExist(){
  auto&mr=ProgramContext::get().methods;
  if(mr.method)return;
  mem = GPUMemory();
  resize();
  mr.method = mr.methodFactories[mr.selectedMethod](mem,mr.methodConstructData[mr.selectedMethod].get());
  updateWindowTitle();
}

void Application::idle(){
  createMethodIfItDoesNotExist();

  computeUpdateFlags();
  conditionalUpdateWindowTitle();
  conditionalClearSDLSurface();

  SceneParam sp;
  sp.proj   = perspectiveCamera.getProjection();
  sp.view   = orbitCamera      .getView      ();
  sp.camera = glm::vec3(glm::inverse(sp.view)*glm::vec4(0.f,0.f,0.f,1.f));
  sp.light  = light;

  auto&mr=ProgramContext::get().methods;
  mr.method->onUpdate(timer.elapsedFromLast());
  mr.method->onDraw(sp);

  swap();
}

void Application::resize(SDL_Event const&event){
  reInitRenderer();
  auto const width  = event.window.data1;
  auto const height = event.window.data2;
  auto const aspect = static_cast<float>(width) / static_cast<float>(height);
  perspectiveCamera.setAspect(aspect);
  auto&mr=ProgramContext::get().methods;
  if(!mr.method)return;
  resize();
  mr.method->onResize(width,height);
}

void Application::mouseMotionLMask(uint32_t mState,float xrel,float yrel){
  if((mState & SDL_BUTTON_LMASK) == 0)return;
  orbitCamera.addXAngle(yrel * sensitivity);
  orbitCamera.addYAngle(xrel * sensitivity);
}

void Application::mouseMotionRMask(uint32_t mState,float yrel){
  if ((mState & SDL_BUTTON_RMASK) == 0)return;
  orbitCamera.addDistance(yrel * orbitZoomSpeed);
}

void Application::mouseMotionMMask(uint32_t mState,float xrel,float yrel){
  if ((mState & SDL_BUTTON_MMASK) == 0) return;
  orbitCamera.addXPosition(+orbitCamera.getDistance() * xrel /
                            float(windowSize.x) * 2.f);
  orbitCamera.addYPosition(-orbitCamera.getDistance() * yrel /
                            float(windowSize.y) * 2.f);
}

void Application::mouseMotion(SDL_Event const&event){
  auto const xrel = static_cast<float>(event.motion.xrel);
  auto const yrel = static_cast<float>(event.motion.yrel);
  auto const mState = event.motion.state;
  mouseMotionLMask(mState,xrel,yrel);
  mouseMotionRMask(mState,yrel);
  mouseMotionMMask(mState,xrel,yrel);
}


void Application::keyDown(SDL_Event const&event){
  auto key = event.key.key;
  switchToNextMethodIfCorrectKeyWasPressed(key);
  switchToPrevMethodIfCorrectKeyWasPressed(key);
  quitIfCorrectKeyWasPressed              (key);
  moveCameraUsingWSADQE                   (event);
  switchSolutionIfCorrectKeyWasPressed    (key);
}

void Application::switchToNextMethodIfCorrectKeyWasPressed(uint32_t key){
  if (key != SDLK_N)return;
  auto&mr=ProgramContext::get().methods;
  auto const nofMethods = mr.methodFactories.size();
  mr.selectedMethod++;
  if(mr.selectedMethod >= nofMethods)mr.selectedMethod=0;
  mr.method = nullptr;
}

void Application::switchToPrevMethodIfCorrectKeyWasPressed(uint32_t key){
  if (key != SDLK_P)return;
  auto&mr=ProgramContext::get().methods;
  auto const nofMethods = mr.methodFactories.size();
  if(mr.selectedMethod > 0)mr.selectedMethod--;
  else mr.selectedMethod = nofMethods-1;
  mr.method = nullptr;
}

void Application::quitIfCorrectKeyWasPressed(uint32_t key){
  if (key != SDLK_ESCAPE)return;
  running = false;
}

void Application::switchSolutionIfCorrectKeyWasPressed(uint32_t key){
  if(key == SDLK_F9 )switchToStudentSolution();
  if(key == SDLK_F10)switchToTeacherSolution();
  //if(key == SDLK_F11)switchToDifference();
}

void Application::moveCameraUsingWSADQE(SDL_Event const&event){
  auto key = event.key.key;
  float speed = 1;
  if(event.key.mod&SDL_KMOD_LSHIFT)speed = .1f;
  if(key == SDLK_A)orbitCamera.addXPosition(+speed);
  if(key == SDLK_D)orbitCamera.addXPosition(-speed);
  if(key == SDLK_W)orbitCamera.addZPosition(+speed);
  if(key == SDLK_S)orbitCamera.addZPosition(-speed);
  if(key == SDLK_E)orbitCamera.addYPosition(-speed);
  if(key == SDLK_Q)orbitCamera.addYPosition(+speed);
}

void Application::swap(){
}


