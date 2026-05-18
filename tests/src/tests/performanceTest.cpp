#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <vector>
#include <memory>

#include <BasicCamera/OrbitCamera.h>
#include <BasicCamera/PerspectiveCamera.h>
#include <examples/izg2026.hpp>
#include <framework/timer.hpp>
#include <tests/performanceTest.hpp>
#include <tests/testCommon.hpp>
#include <framework/switchSolution.hpp>

#define ___ std::cerr << __FILE__ << "/" << __LINE__ << std::endl

static float measure(GPUMemory&mem, izg2026::Method*method, size_t frames){
  basicCamera::PerspectiveCamera perspectiveCamera;
  basicCamera::OrbitCamera orbitCamera;
  orbitCamera.addDistance(1.f);
  perspectiveCamera.setNear(0.1f);
  auto const aspect = static_cast<float>(500) / static_cast<float>(500);
  perspectiveCamera.setAspect(aspect);

  auto const light  = glm::vec3(10.f,10.f,10.f);
  auto const view   = orbitCamera.getView();
  auto const proj   = perspectiveCamera.getProjection();
  auto const camera = glm::vec3(glm::inverse(view)*glm::vec4(0.f,0.f,0.f,1.f));

  Timer<float>timer;
  timer.reset();
  SceneParam sceneParam;
  sceneParam.view   = view;
  sceneParam.proj   = proj;
  sceneParam.camera = camera;
  sceneParam.light  = light;

  for (size_t i = 0; i < frames; ++i){
    method->onDraw(sceneParam);
  }
  return timer.elapsedFromStart() / static_cast<float>(frames);
}

static size_t countNonBlack(tests::AllocatedFramebuffer&aframe){
  size_t nonBlack = 0;
  for(uint32_t y=0;y<aframe.frame.height;++y)
    for(uint32_t x=0;x<aframe.frame.width;++x){
      auto c = tests::getColorU(aframe.frame.color,x,y);
      if(c.r>20 || c.g>20 || c.b>20) nonBlack++;
    }
  return nonBlack;
}

void runPerformanceTest(size_t framesPerMeasurement) {
  uint32_t width  = 500;
  uint32_t height = 500;

  // ---------- Student ----------
  {
    switchToStudentSolution();
    auto aframe = tests::createFramebuffer(width,height);
    GPUMemory mem;
    mem.framebuffers[mem.defaultFramebuffer] = aframe.frame;
    auto method = std::make_shared<izg2026::Method>(mem);

    // Warmup + verify it renders
    SceneParam dummy; dummy.view=glm::mat4(1.f);
    dummy.proj=glm::perspective(glm::radians(45.f),1.f,0.1f,1000.f);
    dummy.camera=glm::vec3(0,0,10); dummy.light=glm::vec3(10,10,10);
    method->onDraw(dummy);
    size_t nb = countNonBlack(aframe);
    std::cerr << "[Student] Non-black pixels: " << nb << std::endl;

    float t = measure(mem, method.get(), framesPerMeasurement);
    std::cout << "[Student] Seconds per frame: " << std::scientific << std::setprecision(10) << t << std::endl;
  }

  // ---------- Teacher ----------
  {
    switchToTeacherSolution();
    auto aframe = tests::createFramebuffer(width,height);
    GPUMemory mem;
    mem.framebuffers[mem.defaultFramebuffer] = aframe.frame;
    auto method = std::make_shared<izg2026::Method>(mem);

    SceneParam dummy; dummy.view=glm::mat4(1.f);
    dummy.proj=glm::perspective(glm::radians(45.f),1.f,0.1f,1000.f);
    dummy.camera=glm::vec3(0,0,10); dummy.light=glm::vec3(10,10,10);
    method->onDraw(dummy);
    size_t nb = countNonBlack(aframe);
    std::cerr << "[Teacher] Non-black pixels: " << nb << std::endl;

    float t = measure(mem, method.get(), framesPerMeasurement);
    std::cout << "[Teacher] Seconds per frame: " << std::scientific << std::setprecision(10) << t << std::endl;
  }
}
