cmake_minimum_required(VERSION 3.12.0)

project(framework)

set(FRAMEWORK_SOURCES
  src/framework/arguments.hpp
  src/framework/arguments.cpp
  src/framework/window.hpp
  src/framework/window.cpp
  src/framework/method.hpp
  src/framework/programContext.hpp
  src/framework/programContext.cpp
  src/framework/application.cpp
  src/framework/application.hpp
  src/framework/timer.hpp
  src/framework/bunny.hpp
  src/framework/bunny.cpp
  src/framework/textureData.hpp
  src/framework/textureData.cpp
  src/framework/model.hpp
  src/framework/model.cpp
  src/framework/systemSpecific.hpp
  src/framework/systemSpecific.cpp
  src/framework/systemSpecificWindows.inl
  src/framework/systemSpecificLinux.inl
  src/framework/colorPrinting.hpp
  src/framework/colorPrinting.cpp
  src/framework/switchSolution.hpp
  src/framework/switchSolution.cpp
  )

set(LIBS_SOURCES
  ../libs/tiny_gltf/tiny_gltf.h
  ../libs/stb_image/stb_image.h
  ../libs/stb_image/stb_image_write.h
  ../libs/json/json.hpp
  ../libs/impl.cpp
  )

add_library(
  ${PROJECT_NAME}
  ${FRAMEWORK_SOURCES}
  ${LIBS_SOURCES}
  )
target_include_directories(${PROJECT_NAME} PUBLIC 
  src/
  ..
  ../libs/json
  ../libs/stb_image
  )

target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC
  SDL3::SDL3
  ArgumentViewer::ArgumentViewer
  BasicCamera::BasicCamera
  studentSolution
  )

target_compile_definitions(
  ${PROJECT_NAME}
  PUBLIC
  CMAKE_ROOT_DIR="${PROJECT_ROOT_DIR}"
  )


