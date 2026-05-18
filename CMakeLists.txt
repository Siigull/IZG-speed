cmake_minimum_required(VERSION 3.12.0)

PROJECT(izgProject)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD   99)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1 ) 

# This section sets #define CMAKE_ROOT_DIR in source codes
# It's purpose is to be used in loading of models and teacher solution
option(CLEAR_CMAKE_ROOT_DIR "if this is set, #define CMAKE_ROOT_DIR will be .")
set(PROJECT_ROOT_DIR ".")
if(NOT CLEAR_CMAKE_ROOT_DIR)
  set(PROJECT_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
endif()

option(SDL_X11_XSCRNSAVER "" OFF)
option(SDL_SHARED "" OFF)
option(SDL_STATIC "" ON )

add_subdirectory(libs/SDL-release-3.4.4)
add_subdirectory(libs/MealyMachine     )
add_subdirectory(libs/TxtUtils         )
add_subdirectory(libs/ArgumentViewer   )
add_subdirectory(solutionInterface     )
add_subdirectory(libs/BasicCamera      )
add_subdirectory(libs/Catch2-3.3.1     )
add_subdirectory(framework             )
add_subdirectory(examples              )
add_subdirectory(tests                 )
add_subdirectory(studentSolution       )

add_executable(${PROJECT_NAME}
  src/main.cpp
  )

target_compile_definitions(${PROJECT_NAME} PUBLIC CMAKE_ROOT_DIR="${PROJECT_ROOT_DIR}")

include(dealWithTeacherSolution.cmake)

target_link_libraries(${PROJECT_NAME} 
  framework
  examples
  tests
  )

if (CMAKE_CROSSCOMPILING)
  target_link_libraries(${PROJECT_NAME}  
    -lmingw32
    -static-libgcc 
    -static-libstdc++
    )
endif()

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
