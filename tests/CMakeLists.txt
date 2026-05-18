cmake_minimum_required(VERSION 3.12.0)

project(tests)

set(TESTS_SOURCES 
  src/tests/str.hpp
  src/tests/str.cpp
  src/tests/testCommon.hpp
  src/tests/testCommon.cpp
  src/tests/memDeepCopy.hpp
  src/tests/memDeepCopy.cpp
  src/tests/isMemDifferent.hpp
  src/tests/isMemDifferent.cpp
  src/tests/renderMethodFrame.hpp
  src/tests/renderMethodFrame.cpp


  src/tests/takeScreenShot.hpp
  src/tests/takeScreenShot.cpp
  src/tests/conformanceTests.hpp
  src/tests/conformanceTests.cpp
  src/tests/performanceTest.hpp
  src/tests/performanceTest.cpp

  src/tests/getNofExamples.cpp
  src/tests/getNofExamples.hpp

  # Set commands tests
  src/tests/commands/bindFramebufferTests.cpp
  src/tests/commands/bindProgramTests.cpp
  src/tests/commands/bindVertexArrayTests.cpp
  src/tests/commands/setBlockWrites.cpp
  src/tests/commands/setBackfaceCulling.cpp
  src/tests/commands/setFrontFace.cpp
  src/tests/commands/setStencil.cpp
  src/tests/commands/setDrawID.cpp

  # Clear tests
  src/tests/commands/clear.cpp
  src/tests/commands/clear_multiple_framebuffers.cpp

  # Other commands tests
  src/tests/commands/user.cpp
  src/tests/commands/drawID_no_program.cpp
  src/tests/commands/subCommandTests.cpp

  # draw vector stage
  src/tests/draw_vector/gl_VertexID_no_indexing.cpp
  src/tests/draw_vector/drawID_empty_program.cpp
  src/tests/draw_vector/clearAndDrawMixed.cpp
  src/tests/draw_vector/vs_interface.cpp
  src/tests/draw_vector/gl_VertexID_indexing.cpp
  src/tests/draw_vector/vertexArrayTests.cpp

  # RASTERIZATION
  src/tests/draw_raster/rasterization.cpp
  src/tests/draw_raster/perspectiveDivision.cpp
  src/tests/draw_raster/backfaceCulling.cpp
  src/tests/draw_raster/barycentrics.cpp

  # EARLY PFO
  src/tests/draw_raster/stencil_test.cpp
  src/tests/draw_raster/stencil_writes_sfail.cpp
  src/tests/draw_raster/depth_test.cpp
  src/tests/draw_raster/stencil_writes_dpfail.cpp

  # LATE PFO
  src/tests/draw_raster/discardTest.cpp
  src/tests/draw_raster/stencil_writes_dppass.cpp
  src/tests/draw_raster/depth_writes.cpp
  src/tests/draw_raster/color_writes.cpp
  src/tests/draw_raster/blending.cpp

  # CLIPPING
  src/tests/draw_raster/clippingTests.cpp

  src/tests/draw_raster/util_raster_part_test.hpp
  src/tests/draw_raster/util_raster_part_test.cpp

  # Model tests
  src/tests/model/traverseModelTests.cpp
  src/tests/model/drawModelTests.cpp
  src/tests/model/vertexShader.cpp
  src/tests/model/fragmentShader.cpp
  src/tests/model/finalImageTest.cpp
  src/tests/model/createModel.hpp
  src/tests/model/createModel.cpp
  )

add_library(${PROJECT_NAME} OBJECT ${TESTS_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC src/)

target_link_libraries(${PROJECT_NAME} PUBLIC 
  framework
  Catch2::Catch2
  examples
  )
