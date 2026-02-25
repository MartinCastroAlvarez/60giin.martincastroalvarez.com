cmake_minimum_required(VERSION 3.31)

project(60GIIN CXX C)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Config types" FORCE)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#Files for dependencies and platform specific config
include(cmake/deps_files.cmake)
include(cmake/platform.cmake)
include(cmake/compiler.cmake)

#****************************************************
# Engine Library
#****************************************************
#Engine Source Files
include(cmake/engine_files.cmake)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/engine/ PREFIX "Source Files" FILES ${ENGINE_SRC_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include/engine PREFIX "Header Files" FILES ${ENGINE_HEADER_FILES})

add_library(EngineLib STATIC ${ENGINE_SRC_FILES} ${ENGINE_HEADER_FILES} ${DEPS_SRC_FILES_COMMON})

target_include_directories(EngineLib PRIVATE ${INCLUDE_PATHS})
target_compile_definitions(EngineLib PRIVATE -D_GLFW_USE_OPENGL=1)
target_compile_features(EngineLib PRIVATE cxx_std_17)

configure_platform(EngineLib)
configure_compiler(EngineLib)

#****************************************************
# Projects
#****************************************************
include(cmake/projects.cmake)

#Root path of the projects
set(PROJECTS_PATH ${CMAKE_CURRENT_LIST_DIR}/projects)
set(ASSETS_PATH ${CMAKE_CURRENT_LIST_DIR}/assets)
#Configure All Projects
foreach(PROJECT ${PROJECT_LIST})
  message("Generating Project " ${PROJECT})

  #Create Directory and main.cpp
  file(MAKE_DIRECTORY ${PROJECTS_PATH}/${PROJECT})
  file(TOUCH ${PROJECTS_PATH}/${PROJECT}/main.cpp)

  #Create Project and configure
  add_executable(${PROJECT} ${PROJECTS_PATH}/${PROJECT}/main.cpp ${DEPS_SRC_FILES})
  target_include_directories(${PROJECT} PRIVATE ${INCLUDE_PATHS})
  target_compile_definitions(${PROJECT} PRIVATE -D_GLFW_USE_OPENGL=1 PROJECT_PATH="${PROJECTS_PATH}/${PROJECT}/" ASSETS_PATH="${ASSETS_PATH}/")
  target_link_libraries(${PROJECT} PRIVATE EngineLib)
  target_compile_features(${PROJECT} PRIVATE cxx_std_17)
  configure_platform(${PROJECT})
  configure_compiler(${PROJECT})

  #Shaders
  file(GLOB SHADERS_LIST ${PROJECTS_PATH}/${PROJECT}/*.vert ${PROJECTS_PATH}/${PROJECT}/*.frag)
  target_sources(${PROJECT} PRIVATE ${SHADERS_LIST})
  source_group(Shaders FILES ${SHADERS_LIST})
endforeach()