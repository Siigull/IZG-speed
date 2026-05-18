
if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/teacherSolution")
  if (CMAKE_CROSSCOMPILING)
    set(CMD_WINDOWS_CONFIG_CMAKE
      COMMAND cmake 
        -S "${CMAKE_CURRENT_SOURCE_DIR}/teacherSolution"
        -B "${CMAKE_CURRENT_BINARY_DIR}/teacherSolutionWindowsBuild"
        -D "CMAKE_BUILD_TYPE=Release"
        -D "CMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/teacherSolutionBinary/windows"
        -D "CMAKE_TOOLCHAIN_FILE=${CMAKE_CURRENT_SOURCE_DIR}/mingw_toolchain.txt"
      )
    set(CMD_WINDOWS_BUILD
      COMMAND cmake
        --build "${CMAKE_CURRENT_BINARY_DIR}/teacherSolutionWindowsBuild"
        --target install
      )
    execute_process(${CMD_WINDOWS_CONFIG_CMAKE})
    execute_process(${CMD_WINDOWS_BUILD}       )
    add_custom_command(
      OUTPUT
        "${CMAKE_CURRENT_SOURCE_DIR}/teacherSolutionBinary/windows/lib/cmake/teacherSolution/teacherSolutionConfig.cmake"
      ${CMD_WINDOWS_CONFIG_CMAKE}
      ${CMD_WINDOWS_BUILD}
      DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/teacherSolution/src/teacherSolution/gpu.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/teacherSolution/src/teacherSolution/gpu.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/teacherSolution/src/teacherSolution/shaderFunctions.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/teacherSolution/src/teacherSolution/shaderFunctions.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/teacherSolution/src/teacherSolution/prepareModel.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/teacherSolution/src/teacherSolution/prepareModel.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/teacherSolution/CMakeLists.txt"
    )
    add_custom_target(
      buildWindowsFakeGPU ALL
      DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/teacherSolutionBinary/windows/lib/cmake/teacherSolution/teacherSolutionConfig.cmake"
    )
  else()
    set(CMD_LINUX_CONFIG_CMAKE
      COMMAND cmake 
        -S "${CMAKE_CURRENT_SOURCE_DIR}/teacherSolution"
        -B "${CMAKE_CURRENT_BINARY_DIR}/teacherSolutionLinuxBuild"
        -D "CMAKE_BUILD_TYPE=Release"
        -D "CMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/teacherSolutionBinary/linux" 
      )
    set(CMD_LINUX_BUILD
      COMMAND cmake
        --build "${CMAKE_CURRENT_BINARY_DIR}/teacherSolutionLinuxBuild"
        --target install
      )
    execute_process(${CMD_LINUX_CONFIG_CMAKE})
    execute_process(${CMD_LINUX_BUILD}       )
    add_custom_command(
      OUTPUT
        "${CMAKE_CURRENT_SOURCE_DIR}/teacherSolutionBinary/linux/lib/cmake/teacherSolution/teacherSolutionConfig.cmake"
      ${CMD_LINUX_CONFIG_CMAKE}
      ${CMD_LINUX_BUILD}
      DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/teacherSolution/src/teacherSolution/gpu.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/teacherSolution/src/teacherSolution/gpu.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/teacherSolution/src/teacherSolution/shaderFunctions.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/teacherSolution/src/teacherSolution/shaderFunctions.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/teacherSolution/src/teacherSolution/prepareModel.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/teacherSolution/src/teacherSolution/prepareModel.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/teacherSolution/CMakeLists.txt"
    )
    add_custom_target(
      buildLinuxFakeGPU ALL
      DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/teacherSolutionBinary/linux/lib/cmake/teacherSolution/teacherSolutionConfig.cmake"
    )
  endif()
endif()

if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/teacherSolution")
  target_include_directories(${PROJECT_NAME} PUBLIC teacherSolution/src/)
  if (CMAKE_CROSSCOMPILING)
    target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/teacherSolutionWindowsBuild")
    add_dependencies(${PROJECT_NAME} buildWindowsFakeGPU)
  else()
    target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/teacherSolutionLinuxBuild")
    add_dependencies(${PROJECT_NAME} buildLinuxFakeGPU)
  endif()
else()
  if (CMAKE_CROSSCOMPILING)
    target_include_directories(${PROJECT_NAME} PUBLIC teacherSolutionBinary/linux/)
  else()
    target_include_directories(${PROJECT_NAME} PUBLIC teacherSolutionBinary/windows/)
  endif()
endif()
