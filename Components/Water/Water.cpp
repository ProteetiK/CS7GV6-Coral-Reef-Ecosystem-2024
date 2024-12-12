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

//definitions
GLuint shaderProgramID_water;
GLuint water_vao, water_vbo;
GLuint textureMap;
GLint water_loc_model, water_loc_view, water_loc_proj, water_loc_time;

using namespace std;

//load amd map water texture
GLuint loadWaterTexture() {
    GLuint textureID;
    //generate a texture
    glGenTextures(1, &textureID);

    int width, height, nrChannels;

    //load texture data
    unsigned char* data = stbi_load(waterTextureFilePath.c_str(), &width, &height, &nrChannels, 0);

    if (data) {
        //select RGB channel based on available channels
        GLenum format = nrChannels == 3 ? GL_RGB : GL_RGBA;
        //bind the texture
        glBindTexture(GL_TEXTURE_2D, textureID);
        //upload the texture data
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        //generate mipmaps for the texture
        glGenerateMipmap(GL_TEXTURE_2D);
        //free data
        stbi_image_free(data);
    }
    else {
        //handle error
        cerr << "Failed to load texture: " << waterTextureFilePath << endl;
        stbi_image_free(data);
    }

    return textureID;
}
//main rendering implementation
void waterRender(const mat4 viewMatrix, const mat4 projectionMatrix, float y, int water_width, int water_height) {
    //compile and enable the terrain shader program
    shaderProgramID_water = CompileShader(waterTexVertexShader, waterTexFragmentShader);

    //load the water texture
    textureMap = loadWaterTexture();

    //activate the shader program
    glUseProgram(shaderProgramID_water);

    //define verticese for the water texture expanse
    float quadVertices[] = {
        -water_width, 0.0f, -water_height,  0.0f, 0.0f,  //bottom-left
         water_width, 0.0f, -water_height,  1.0f, 0.0f,  //bottom-right
         water_width, 0.0f,  water_height,  1.0f, 1.0f,  //top-right
        -water_width, 0.0f,  water_height,  0.0f, 1.0f   //top-left
    };

    //generate vertex array object and vertex buffer object
    glGenVertexArrays(1, &water_vao);
    glGenBuffers(1, &water_vbo);

    //bind the vertex array and vertex buffer objects
    glBindVertexArray(water_vao);
    glBindBuffer(GL_ARRAY_BUFFER, water_vbo);

    //link the vertex data to the vbo
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    //define and enable the positions and texture coordinates
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //unbind the vao
    glBindVertexArray(0);

    //get the location of the model, view, projection and time matrix uniforms
    water_loc_model = glGetUniformLocation(shaderProgramID_water, "model");
    water_loc_view = glGetUniformLocation(shaderProgramID_water, "view");
    water_loc_proj = glGetUniformLocation(shaderProgramID_water, "projection");
    water_loc_time = glGetUniformLocation(shaderProgramID_water, "time");

    //activate water shader program
    glUseProgram(shaderProgramID_water);

    //create model matrix and translate along the y-axis
    mat4 water_model = identity_mat4();
    water_model = translate(water_model, vec3(0.0f, y, 0.0f));

    //pass transformation matrices to the shader
    glUniformMatrix4fv(water_loc_model, 1, GL_FALSE, water_model.m);
    glUniformMatrix4fv(water_loc_view, 1, GL_FALSE, viewMatrix.m);
    glUniformMatrix4fv(water_loc_proj, 1, GL_FALSE, projectionMatrix.m);

    //get current time and pass it to the shader for animation
    float timeValue = static_cast<float>(timeGetTime());
    glUniform1f(water_loc_time, timeValue);

    //activate the texture and bind the water texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureMap);

    //pass the texture index to the shader for mapping the normals
    glUniform1i(glGetUniformLocation(shaderProgramID_water, "normalMap"), 0);

    //bind the vao and draw the water
    glBindVertexArray(water_vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    //unbind the vao
    glBindVertexArray(0);

    //deactivate the shader program
    glUseProgram(0);
}
