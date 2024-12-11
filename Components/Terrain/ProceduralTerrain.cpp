//system imports
#include <vector>
#include <cmath>
#include <windows.h>
#include <mmsystem.h>
#include <math.h>
#include <iostream>

//GL imports
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "../../stb_perlin.h"
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

//project includes
#include "../../Globals.h"
#include "../Shaders.h"
#include "ProceduralTerrain.h"
#include "../../maths_funcs.h"

GLuint terrain_vao, terrain_vbo, terrain_ebo;
GLuint shaderProgramID_terrain;

std::vector<float> generateHeightMap(int terrainSize) {
    std::vector<float> heights(terrainSize * terrainSize);
    for (int z = 0; z < terrainSize; z++) {
        for (int x = 0; x < terrainSize; x++) {
            float up_x = x * scaleUp;
            float up_z = z * scaleUp;
            float height = stb_perlin_noise3(up_x, up_z, 0.0f, 0, 0, 0);
            heights[z * terrainSize + x] = height * 10.0f;
        }
    }
    return heights;
}

void generateTerrain(std::vector<float>& heightMap, std::vector<float>& terrain_vertices, std::vector<unsigned int>& terrain_indices) {
    for (int z = 0; z < terrainSize; ++z) {
        for (int x = 0; x < terrainSize; ++x) {
            float height = heightMap[z * terrainSize + x];
            terrain_vertices.push_back(x - terrainSize / 2.0f);
            terrain_vertices.push_back(height);
            terrain_vertices.push_back(z - terrainSize / 2.0f);

            terrain_vertices.push_back(0.0f);
            terrain_vertices.push_back(1.0f);
            terrain_vertices.push_back(0.0f);
        }
    }

    for (int z = 0; z < terrainSize - 1; ++z) {
        for (int x = 0; x < terrainSize - 1; ++x) {
            int start = z * terrainSize + x;

            terrain_indices.push_back(start);
            terrain_indices.push_back(start + 1);
            terrain_indices.push_back(start + terrainSize);

            terrain_indices.push_back(start + 1);
            terrain_indices.push_back(start + terrainSize + 1);
            terrain_indices.push_back(start + terrainSize);
        }
    }
}

GLuint loadTerrainTexture() {
    GLuint textureID;
    glGenTextures(1, &textureID);

    int terrainSize, nrChannels;
    unsigned char* data = stbi_load(terrainTextureFilePath.c_str(), &terrainSize, &terrainSize, &nrChannels, 0);
    if (data) {
        GLenum format = nrChannels == 3 ? GL_RGB : GL_RGBA;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, terrainSize, terrainSize, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }
    else {
        std::cerr << "Failed to load texture: " << terrainTextureFilePath << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void displayTerrain() {
    std::vector<float> heightMap = generateHeightMap(terrainSize);
    std::vector<float> terrain_vertices;
    std::vector<unsigned int> terrain_indices;

    generateTerrain(heightMap, terrain_vertices, terrain_indices);
    GLuint textureMap = loadTerrainTexture();

    glGenVertexArrays(1, &terrain_vao);
    glBindVertexArray(terrain_vao);

    glGenBuffers(1, &terrain_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, terrain_vbo);
    glBufferData(GL_ARRAY_BUFFER, terrain_vertices.size() * sizeof(float), terrain_vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &terrain_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, terrain_indices.size() * sizeof(unsigned int), terrain_indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    shaderProgramID_terrain = CompileShader(terrainVertexShader, terrainFragmentShader);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureMap);
    glUniform1i(glGetUniformLocation(shaderProgramID_terrain, "normalMap"), 0);

    int matrix_location = glGetUniformLocation(shaderProgramID_terrain, "model");
    int view_mat_location = glGetUniformLocation(shaderProgramID_terrain, "view");
    int proj_mat_location = glGetUniformLocation(shaderProgramID_terrain, "projection");

    mat4 terrain_view = identity_mat4();
    mat4 persp_proj_terrain = perspective(45.0f, (float)width / (float)height, 0.1f, 600.0f);
    mat4 terrain_model = identity_mat4();

    terrain_model = translate(terrain_model, vec3(0.0f + base_x, 0.0f + base_y, 0.0f + base_z));

    glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj_terrain.m);
    glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, terrain_view.m);
    glUniformMatrix4fv(matrix_location, 1, GL_FALSE, terrain_model.m);

    glUseProgram(shaderProgramID_terrain);
    glUniform3f(glGetUniformLocation(shaderProgramID_terrain, "terrainColor"), 0.9f, 0.7f, 0.5f);
    glUniform3f(glGetUniformLocation(shaderProgramID_terrain, "lightPosition"), 0.0f, 10.0f, 0.0f);
    glUniform3f(glGetUniformLocation(shaderProgramID_terrain, "lightColor"), 1.0f, 1.0f, 1.0f);

    glUseProgram(shaderProgramID_terrain);
    glBindVertexArray(terrain_vao);
    glDrawElements(GL_TRIANGLES, (terrainSize - 1) * (terrainSize - 1) * 6, GL_UNSIGNED_INT, 0);
}