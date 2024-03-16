#include "skybox.h"

#include <stb/stb_image.h>

SkyBox::SkyBox(const char* path, std::vector<float>& ver) {
    directory = std::string(path);
    cubemapTexture = loadCubemap();
    vertexs = ver;
    setupSkyBox();
}

void SkyBox::Draw(Shader& Shader) {
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS); // set depth function back to default
}

void SkyBox::setupSkyBox() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexs.size() * sizeof(float), &vertexs[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

#define LOAD_FACE(path, i)\
    do {\
        int width, height, nrChannels;\
        unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);\
        if (data) {\
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);\
            stbi_image_free(data);\
        }\
        else {\
            std::cout << "Cubemap texture failed to load at path: " << path << std::endl;\
            stbi_image_free(data);\
        }\
    } while(0)

unsigned int SkyBox::loadCubemap() {
    unsigned int TexID;
    glGenTextures(1, &TexID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, TexID);
    LOAD_FACE((directory + "\\right.jpg").c_str(), 0);
    LOAD_FACE((directory + "\\left.jpg").c_str(), 1);
    LOAD_FACE((directory + "\\top.jpg").c_str(), 2);
    LOAD_FACE((directory + "\\bottom.jpg").c_str(), 3);
    LOAD_FACE((directory + "\\front.jpg").c_str(), 4);
    LOAD_FACE((directory + "\\back.jpg").c_str(), 5);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return TexID;
}