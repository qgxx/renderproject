#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#define STBI_WINDOWS_UTF8
#include <stb/stb_image.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <gtest/gtest.h>
#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include <chrono>
#include <Jolt/Jolt.h>
#include <gtest/gtest.h>

#include "render/shader.h"
#include "render/camera.h"
#include "render/model.h"
#include "render/skybox.h"
#include "render/terrain.h"
#include "render/ocean.h"
#include "core/qgetime.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void processInput(GLFWwindow* window);
void Do_Movement();
unsigned int loadTexture(char* path);
void RenderScene(Shader &shader);
void RenderCube();
void RenderQuad();

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

Camera camera(glm::vec3(0.0f, 10.0f, 20.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool m_isWireframe = false;
bool m_showImgui = false;
bool m_useCursor = false;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

GLboolean shadows = true;

GLuint woodTexture;
GLuint planeVAO;

int gShowPoints;

int main() {
    #ifdef _WIN32
    _CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_DEBUG );
    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
    #endif

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GameEngine", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);  
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext(nullptr);
    const ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsLight();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    Shader ourShader("..\\asserts\\shaders\\model_loading.vs", "..\\asserts\\shaders\\model_loading.fs");
    Model ourModel("..\\asserts\\models\\Elysia_maid\\Elysia.pmx");

    SkyBox skybox("..\\asserts\\images\\skybox");
    Shader skyboxShader("..\\asserts\\shaders\\skybox.vs", "..\\asserts\\shaders\\skybox.fs");
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    Terrain terrain(2.0f, 33);
    terrain.setTexScale(4.0f);
    Shader terrainShader("..\\asserts\\shaders\\terrain.vs", "..\\asserts\\shaders\\terrain.fs");
    terrainShader.use();
    terrainShader.setVec3("gReversedLightDir", glm::vec3(0.0f, 1.0f, 0.0f));
    std::vector<std::pair<std::string, std::string>> Tiles;
    Tiles.push_back({"..\\asserts\\images\\tile1.jpg", "tile1"});
    Tiles.push_back({"..\\asserts\\images\\tile2.jpg", "tile2"});
    Tiles.push_back({"..\\asserts\\images\\tile3.png", "tile3"});
    Tiles.push_back({"..\\asserts\\images\\tile4.png", "tile4"});
    terrain.setMinMAxHeight(0.0f, 256.0f);
    terrain.loadTiles(Tiles);
    Shader terrainNormal("..\\asserts\\shaders\\tn.vs", "..\\asserts\\shaders\\tn.fs", "..\\asserts\\shaders\\tn.gs");

    printf("Camera: %f %f\n", camera.getPos()[0], camera.getPos()[2]);
    printf("Terrain: %f %f\n", terrain.getCenterPos()[0], terrain.getCenterPos()[1]);
    printf("Terrain's WorldScale: %f\n", terrain.GetWorldScale());

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();
        glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); 
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);

        glDepthFunc(GL_LEQUAL);  
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); 
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        skybox.Draw(skyboxShader);

        terrainShader.use();
        view = camera.GetViewMatrix();
        terrainShader.setMat4("view", view);
        projection = glm::perspective(glm::radians(camera.fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 5000.0f);
        terrainShader.setMat4("projection", projection);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-512.0f, -300.0f, -512.0f)); 
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        terrainShader.setMat4("model", model);
        static float foo = 0.0f;
        foo += 0.002f;
        float y = std::min(-0.4f, cosf(foo));
        glm::vec3 LightDir(sinf(foo * 5.0f), -y, cosf(foo * 5.0f));
        terrainShader.setVec3("gReversedLightDir", LightDir);
        terrain.Draw(terrainShader, camera.getPos() + glm::vec3(512.0f, 300.0f, 512.0f));  // Terrain'Local Space
        terrainNormal.use();
        terrainNormal.setMat4("model", model);  
        terrainNormal.setMat4("view", view);
        terrainNormal.setMat4("projection", projection);
        // terrain.Draw(terrainNormal, camera.getPos() + glm::vec3(512.0f, 300.0f, 512.0f));

        if (m_showImgui) {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            static float maxHeight = 256.0f;
            static float Roughness = 1.0f;
            ImGui::Begin("Terrain"); 
            ImGui::SliderFloat("MaxHeight", &maxHeight, 0.0f, 1000.0f);
            ImGui::SliderFloat("Roughness", &Roughness, 0.0f, 1.0f);

            static float Height0 = 64.0f;
            static float Height1 = 128.0f;
            static float Height2 = 192.0f;
            static float Height3 = 256.0f;
            ImGui::SliderFloat("Height0", &Height0, 0.0f, 64.0f);
            ImGui::SliderFloat("Height1", &Height1, 64.0f, 128.0f);
            ImGui::SliderFloat("Height2", &Height2, 128.0f, 192.0f);
            ImGui::SliderFloat("Height3", &Height3, 192.0f, 256.0f);

            if (ImGui::Button("Generate")) {
                terrain.destroy();
                int Size = 513;
                float MinHeight = 0.0f;
                terrain.CreateMidpointDisplacement(Size, 33, Roughness, MinHeight, maxHeight);
            }
            if (ImGui::Button("Save")) {
                terrain.saveHeightMap("..\\asserts\\others\\heightmap.save");
            }

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.increaseMovementSpeed();
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        camera.resetMovementSpeed();

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(cLEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(cRIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    if (m_useCursor) return;
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) m_showImgui = !m_showImgui;

    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
        m_isWireframe = !m_isWireframe;
        if (m_isWireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
        m_useCursor = !m_useCursor;
        if (m_useCursor) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else {
            glfwSetCursorPos(window, lastX, lastY);
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
}