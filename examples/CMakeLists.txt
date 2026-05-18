cmake_minimum_required(VERSION 3.12.0)

project(examples)

set(EXAMPLES_SOURCES
  src/examples/emptyMethod.cpp
  src/examples/triangleMethod.cpp
  src/examples/rotatingTriangles.cpp
  src/examples/triangleClip1Method.cpp
  src/examples/triangleClip2Method.cpp
  src/examples/triangle3DMethod.cpp
  src/examples/triangleBufferMethod.cpp
  src/examples/additiveBlending.cpp
  src/examples/subtractiveBlending.cpp
  src/examples/maxBlending.cpp
  src/examples/minBlending.cpp
  src/examples/czFlagMethod.cpp
  src/examples/angry.cpp
  src/examples/stairs.cpp
  src/examples/phongMethod.cpp
  src/examples/texturedQuadMethod.cpp
  src/examples/video.cpp
  src/examples/skFlagMethod.cpp
  src/examples/edgeDetect.cpp
  src/examples/shadowMapping.cpp
  src/examples/anime.cpp
  src/examples/modelMethod.cpp
  src/examples/modelMethod.hpp
  src/examples/stencilTriangle.cpp
  src/examples/mirror.cpp
  src/examples/china.cpp
  src/examples/nyra.cpp
  src/examples/shadowModel.hpp
  src/examples/shadowModel.cpp
  src/examples/parrots.hpp
  src/examples/parrots.cpp
  src/examples/izg2026.cpp
  )

add_library               (${PROJECT_NAME} OBJECT ${EXAMPLES_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC src/)
target_link_libraries     (${PROJECT_NAME} PUBLIC framework)
target_compile_definitions(${PROJECT_NAME} PUBLIC CMAKE_ROOT_DIR="${PROJECT_ROOT_DIR}")
