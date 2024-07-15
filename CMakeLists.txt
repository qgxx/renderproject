cmake_minimum_required(VERSION 3.17)
project(renderproject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CMakeLists.txt.imgui)
include(sources/function/render/CMakeLists.txt.render)
include(sources/function/animation/CMakeLists.txt.animation)
include(sources/core/message/CMakeLists.txt.message)
include(sources/core/CMakeLists.txt.core)

include_directories(depends)
include_directories(depends/Assimp/include)
include_directories(depends/googletest-distribution/include)
include_directories(depends/JoltPhysics/include)
include_directories(depends/glad/include)
include_directories(depends/GLFW/include)
include_directories(depends/easy_profiler/include)
include_directories(sources)
include_directories(sources/function)

add_executable(renderproject sources/main.cpp ${imgui_sources} ${render_sources} ${message_sources} 
${core_sources} ${animation_sources} depends/glad/src/glad.c sources/resource/resource.cpp)

# copy dll to build directory
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
file(COPY ./depends/Assimp/bin/libassimp-5.dll DESTINATION ./Debug)
file(COPY ./depends/Assimp/bin/libassimp-5.dll DESTINATION ./)
file(COPY ./asserts DESTINATION ./)
file(COPY ./asserts DESTINATION ../)
target_link_libraries(renderproject ${PROJECT_SOURCE_DIR}/depends/Assimp/lib/assimp-vc143-mt.lib)
target_link_libraries(renderproject ${PROJECT_SOURCE_DIR}/depends/GLFW/lib/glfw3.lib)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
file(COPY ${PROJECT_BINARY_DIR}/../depends/Assimp/bin/libassimp-5.dll DESTINATION ${PROJECT_BINARY_DIR}/)
target_link_libraries(renderproject ${PROJECT_BINARY_DIR}/../depends/Assimp/bin/libassimp-5.dll)
target_link_libraries(renderproject ${PROJECT_BINARY_DIR}/../depends/googletest-distribution/lib/libgtest.a 
${PROJECT_BINARY_DIR}/../depends/googletest-distribution/lib/libgtest_main.a 
${PROJECT_BINARY_DIR}/../depends/googletest-distribution/lib/libgmock.a 
${PROJECT_BINARY_DIR}/../depends/googletest-distribution/lib/libgmock_main.a)
target_link_libraries(renderproject ${PROJECT_BINARY_DIR}/../depends/JoltPhysics/lib/libJolt.a)
target_link_libraries(renderproject ${PROJECT_BINARY_DIR}/../depends/GLFW/lib/libglfw3.a)
endif()