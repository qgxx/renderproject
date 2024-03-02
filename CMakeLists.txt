cmake_minimum_required(VERSION 3.17)
project(TinyQGameEngine)

set(CMAKE_CXX_STANDARD 11)

include(CMakeLists.txt.glfw)
include(sources/render/CMakeLists.txt.render)

include_directories(depends)
include_directories(depends/Assimp/include)
include_directories(sources)

add_executable(TinyQGameEngine sources/main.cpp ${glfw_sources} ${render_sources})

target_link_libraries(TinyQGameEngine ${PROJECT_BINARY_DIR}/libassimp-5.dll)