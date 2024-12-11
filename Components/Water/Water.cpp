//system imports
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <math.h>
#include <vector>

//gl imports
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include "../maths_funcs.h"

//project imports
#include "Water.h"
#include "../Shaders.h"
#include "../../Globals.h"


Water::Water(float width, float height)
    : water_width(width), water_height(height) {
    textureMap = loadWaterTexture();
}
void Water::init() {
    shaderProgramID_water = CompileShader(waterTexVertexShader, waterTexFragmentShader);
    glUseProgram(shaderProgramID_water);
    float quadVertices[] = {
        -water_width, 0.0f, -water_height,  0.0f, 0.0f,
         water_width, 0.0f, -water_height,  1.0f, 0.0f,
         water_width, 0.0f,  water_height,  1.0f, 1.0f,
        -water_width, 0.0f,  water_height,  0.0f, 1.0f,
    };

    glGenVertexArrays(1, &water_vao);
    glGenBuffers(1, &water_vbo);

    glBindVertexArray(water_vao);
    glBindBuffer(GL_ARRAY_BUFFER, water_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    water_loc_model = glGetUniformLocation(shaderProgramID_water, "model");
    water_loc_view = glGetUniformLocation(shaderProgramID_water, "view");
    water_loc_proj = glGetUniformLocation(shaderProgramID_water, "projection");
    water_loc_time = glGetUniformLocation(shaderProgramID_water, "time");
}

void Water::render(const glm::mat4 viewMatrix, const glm::mat4 projectionMatrix, float y) {
    glUseProgram(shaderProgramID_water);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, y, 0.0f));
    glUniformMatrix4fv(water_loc_model, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(water_loc_view, 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(water_loc_proj, 1, GL_FALSE, &projectionMatrix[0][0]);

    float timeValue = static_cast<float>(timeGetTime());
    glUniform1f(water_loc_time, timeValue);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureMap);
    glUniform1i(glGetUniformLocation(shaderProgramID_water, "normalMap"), 0);

    glBindVertexArray(water_vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);

    glUseProgram(0);
}
GLuint Water::loadWaterTexture() {
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(waterTextureFilePath.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = nrChannels == 3 ? GL_RGB : GL_RGBA;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }
    else {
        std::cerr << "Failed to load texture: " << waterTextureFilePath << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}