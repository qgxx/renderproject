cmake_minimum_required(VERSION 3.17)
project(TinyQGameEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CMakeLists.txt.glfw)
include(CMakeLists.txt.imgui)
include(sources/render/CMakeLists.txt.render)
include(sources/message/CMakeLists.txt.message)

include_directories(depends)
include_directories(depends/Assimp/include)
include_directories(depends/googletest-distribution/include)
include_directories(sources)

add_executable(TinyQGameEngine sources/main.cpp ${glfw_sources} ${imgui_sources} ${render_sources} ${message_sources})

# copy dll to build directory
file(COPY ${PROJECT_BINARY_DIR}/../depends/Assimp/bin/libassimp-5.dll DESTINATION ${PROJECT_BINARY_DIR}/)

target_link_libraries(TinyQGameEngine ${PROJECT_BINARY_DIR}/../depends/Assimp/bin/libassimp-5.dll)
target_link_libraries(TinyQGameEngine ${PROJECT_BINARY_DIR}/../depends/googletest-distribution/lib/libgtest.a 
${PROJECT_BINARY_DIR}/../depends/googletest-distribution/lib/libgtest_main.a 
${PROJECT_BINARY_DIR}/../depends/googletest-distribution/lib/libgmock.a 
${PROJECT_BINARY_DIR}/../depends/googletest-distribution/lib/libgmock_main.a)