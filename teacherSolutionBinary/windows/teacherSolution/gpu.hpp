#pragma once

#include<solutionInterface/gpu.hpp>
#include<teacherSolution/teachersolution_export.hpp>

extern "C"{
  TEACHERSOLUTION_EXPORT void teacher_gpu_run(GPUMemory&mem,CommandBuffer const&cb);
  TEACHERSOLUTION_EXPORT void breakable_gpu_run(GPUMemory&mem,CommandBuffer const&cb);
  TEACHERSOLUTION_EXPORT extern size_t idToBreak;
  TEACHERSOLUTION_EXPORT extern size_t lineToBreak;
}
