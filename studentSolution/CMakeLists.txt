cmake_minimum_required(VERSION 3.12.0)

PROJECT(studentSolution)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD   99)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1 ) 

add_library(${PROJECT_NAME} OBJECT 
  src/studentSolution/gpu.cpp
  src/studentSolution/gpu.hpp
  src/studentSolution/prepareModel.cpp
  src/studentSolution/prepareModel.hpp
  src/studentSolution/shaderFunctions.cpp
  src/studentSolution/shaderFunctions.hpp
  )
target_include_directories(${PROJECT_NAME} PUBLIC .)
target_link_libraries     (${PROJECT_NAME} PUBLIC gpuInterface)
target_include_directories(${PROJECT_NAME} PUBLIC src/)
