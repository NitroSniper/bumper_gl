##############################
#   Created By Nitro Sniper  #
#            CMake           #
##############################
cmake_minimum_required(VERSION 3.28)
project(App)
set(CMAKE_CXX_STANDARD 23)
include(FetchContent)


##############################
#             GLM            #
##############################

FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG bf71a834948186f4097caa076cd2663c69a10e1e #refs/tags/1.0.1
)
FetchContent_MakeAvailable(glm)
##############################
#            Glad            #
##############################

# Generate your glad folder via https://gen.glad.sh/
# I'm using OpenGL 4.6 Compatible

set(glad_SOURCE_DIR glad)
add_library(
        glad
        ${glad_SOURCE_DIR}/src/gl.c
        ${glad_SOURCE_DIR}/include/glad/gl.h
        ${glad_SOURCE_DIR}/include/KHR/khrplatform.h
)
target_include_directories(glad PUBLIC ${glad_SOURCE_DIR}/include)
##############################
#            GLFW            #
##############################

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 7b6aead9fb88b3623e3b3725ebb42670cbe4c579 #refs/tags/3.4
)
FetchContent_MakeAvailable(glfw)
##############################
#            IMGUI           #
##############################

FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG 231cbee0fc4f59dbe5b8b853a11b08dc84e57c65
)
FetchContent_MakeAvailable(imgui)
file(GLOB imgui_SRC ${imgui_SOURCE_DIR}/*.cpp ${imgui_SOURCE_DIR}/*.h)
add_library(
        imgui
        ${imgui_SRC}
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.h
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.h
)
target_link_libraries(imgui PUBLIC glad glfw)
target_include_directories(
        imgui
        PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)

##############################
#             APP            #
##############################

add_executable(App main.cpp)
target_link_libraries(App glm::glm glfw glad imgui)
