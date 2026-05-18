/*!
 * @file
 * @brief This file contains application class.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#pragma once

#include <memory>
#include <vector>

#include <BasicCamera/OrbitCamera.h>
#include <BasicCamera/PerspectiveCamera.h>

#include <solutionInterface/taskFunctions.hpp>
#include <framework/window.hpp>
#include <framework/programContext.hpp>
#include <framework/timer.hpp>

/**
 * @brief Application class
 */
class Application: protected Window{
  public:
    Application(int32_t width,int32_t height);
    ~Application();
    void start();
    void setMethod(uint32_t m);
  private:
    void idle();
    void resize(SDL_Event const&event);
    void mouseMotionLMask(uint32_t mState,float xrel,float yrel);
    void mouseMotionRMask(uint32_t mState,float yrel);
    void mouseMotionMMask(uint32_t mState,float xrel,float yrel);
    void mouseMotion(SDL_Event const&event);
    void keyDown(SDL_Event const&event);
    void switchToNextMethodIfCorrectKeyWasPressed(uint32_t key);
    void switchToPrevMethodIfCorrectKeyWasPressed(uint32_t key);
    void quitIfCorrectKeyWasPressed              (uint32_t key);
    void switchSolutionIfCorrectKeyWasPressed    (uint32_t key);
    void moveCameraUsingWSADQE                   (SDL_Event const&event);
    void createMethodIfItDoesNotExist();
    void swap();
    void resize();
    void updateWindowTitle();
    void computeUpdateFlags();
    void conditionalUpdateWindowTitle();
    void conditionalClearSDLSurface();
    bool shouldUpdateTitle     = true;
    bool shouldClearSDLSurface = true;
    bool teacherSolutionSeen   = true;


    basicCamera::OrbitCamera       orbitCamera                                        ;
    basicCamera::PerspectiveCamera perspectiveCamera                                  ;
    glm::vec3                      light                = glm::vec3(100.f,100.f,100.f);
    glm::uvec2                     windowSize                                         ;
    float                          sensitivity          = 0.01f                       ;
    float                          orbitZoomSpeed       = 0.1f                        ;
    Timer<float>                   timer                                              ;
    std::vector<float  >           depthMemoryBacking                                 ;
    std::vector<uint8_t>           stencilMemoryBacking                               ;
    Framebuffer                    defaultFramebuffer                                 ;
    GPUMemory                      mem                                                ;
};

/**
 * @brief This function sets default scene parameters for rendering
 *
 * @param orbit output orbit camera
 * @param proj output projection camera
 * @param light output light position
 * @param width width of the window
 * @param height height of the window
 */
void defaultSceneParameters(
    basicCamera::OrbitCamera&orbit,
    basicCamera::PerspectiveCamera&proj,
    glm::vec3&light,
    uint32_t width,uint32_t height);

