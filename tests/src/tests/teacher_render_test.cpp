#include <tests/testCommon.hpp>
#include <tests/performanceTest.hpp>
#include <examples/izg2026.hpp>
#include <iostream>
#include <iomanip>
#include <cstring>

// Standalone test to verify teacher solution actually renders something
int main(int argc, char** argv){
    (void)argc;(void)argv;
    
    uint32_t width  = 500;
    uint32_t height = 500;
    auto aframe = tests::createFramebuffer(width,height);
    GPUMemory mem;
    mem.framebuffers[mem.defaultFramebuffer] = aframe.frame;
    
    auto method = std::make_shared<izg2026::Method>(mem);
    
    basicCamera::PerspectiveCamera perspectiveCamera;
    basicCamera::OrbitCamera orbitCamera;
    orbitCamera.addDistance(1.f);
    perspectiveCamera.setNear(0.1f);
    perspectiveCamera.setAspect(static_cast<float>(width)/static_cast<float>(height));
    
    SceneParam sceneParam;
    sceneParam.view   = orbitCamera.getView();
    sceneParam.proj   = perspectiveCamera.getProjection();
    sceneParam.camera = glm::vec3(glm::inverse(sceneParam.view)*glm::vec4(0.f,0.f,0.f,1.f));
    sceneParam.light  = glm::vec3(10.f,10.f,10.f);
    
    // Clear frame to black first
    for(uint32_t y=0;y<height;++y)
        for(uint32_t x=0;x<width;++x)
            tests::setColorU(aframe.frame,x,y,glm::uvec4(0,0,0,255));
    
    // Render one frame
    method->onDraw(sceneParam);
    
    // Count non-black pixels
    size_t nonBlack = 0;
    size_t totalPixels = width*height;
    for(uint32_t y=0;y<height;++y){
        for(uint32_t x=0;x<width;++x){
            auto c = tests::getColorU(aframe.frame.color,x,y);
            if(c.r > 10 || c.g > 10 || c.b > 10) nonBlack++;
        }
    }
    
    std::cout << "Total pixels: " << totalPixels << std::endl;
    std::cout << "Non-black pixels: " << nonBlack << std::endl;
    std::cout << "Percentage non-black: " << (100.0*nonBlack/totalPixels) << "%" << std::endl;
    
    // Also time it properly with a simple loop
    Timer<float> timer;
    timer.reset();
    const int N = 100;
    for(int i=0;i<N;++i){
        method->onDraw(sceneParam);
    }
    float elapsed = timer.elapsedFromStart();
    std::cout << "Total time for " << N << " frames: " << elapsed << " s" << std::endl;
    std::cout << "Seconds per frame: " << std::scientific << std::setprecision(10) << (elapsed/N) << std::endl;
    
    return (nonBlack > 100) ? 0 : 1;
}
