cmake_minimum_required(VERSION 3.12.0)

PROJECT(gpuInterface)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 99)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1 ) 

add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ../libs/glm-0.9.9.8)

add_library(${PROJECT_NAME} STATIC
  src/solutionInterface/gpu.cpp
  src/solutionInterface/gpu.hpp
  src/solutionInterface/modelFwd.cpp
  src/solutionInterface/modelFwd.hpp
  src/solutionInterface/taskFunctions.cpp
  src/solutionInterface/taskFunctions.hpp
  )
target_include_directories(${PROJECT_NAME} PUBLIC .)
target_link_libraries(${PROJECT_NAME} PUBLIC glm)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC src/)
