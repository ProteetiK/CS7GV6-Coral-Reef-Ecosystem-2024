//system imports
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

//definitions
GLuint terrain_vao, terrain_vbo, terrain_ebo;
GLuint shaderProgramID_terrain;

using namespace std;

//generate terrain height-map
vector<float> generateHeightMap(int terrainSize) {
    //initialize vector of the size of the generated terrain to store height values
    vector<float> heights(terrainSize * terrainSize);
    //z-axis loop
    for (int z = 0; z < terrainSize; z++) {
        //x-axis loop
        for (int x = 0; x < terrainSize; x++) {
            //scale up on both axes
            float up_x = x * scaleUp;
            float up_z = z * scaleUp;
            //add perlin noise to each height
            float height = stb_perlin_noise3(up_x, up_z, 0.0f, 0, 0, 0);
            //scale again and store final value
            heights[z * terrainSize + x] = height * 10.0f;
        }
    }
    return heights;
}

//generate vertices and indices for the main terrain mesh
void generateTerrain(vector<float>& heightMap, vector<float>& terrain_vertices, vector<unsigned int>& terrain_indices) {
    //z-axis loop
    for (int z = 0; z < terrainSize; ++z) { 
        //x-axis loop
        for (int x = 0; x < terrainSize; ++x) {
            //extract height from height-map
            float height = heightMap[z * terrainSize + x];
            //add x-axis value to the center
            terrain_vertices.push_back(x - terrainSize / 2.0f);
            //add y-axis value (height)
            terrain_vertices.push_back(height);
            //add z-axis value to the center
            terrain_vertices.push_back(z - terrainSize / 2.0f);

            //add x, y and z component of the normal
            terrain_vertices.push_back(0.0f);
            terrain_vertices.push_back(1.0f);
            terrain_vertices.push_back(0.0f);
        }
    }

    //z-axis loop for triangle generation
    for (int z = 0; z < terrainSize - 1; ++z) { // Loop through rows for triangle generation
        //x-axis loop for triangle generation
        for (int x = 0; x < terrainSize - 1; ++x) {
            //calculate the starting index for the quad
            int start = z * terrainSize + x;
            //add first triangle indices
            terrain_indices.push_back(start);
            terrain_indices.push_back(start + 1);
            terrain_indices.push_back(start + terrainSize);
            //add second triangle indices
            terrain_indices.push_back(start + 1);
            terrain_indices.push_back(start + terrainSize + 1);
            terrain_indices.push_back(start + terrainSize);
        }
    }
}

//generate the terrain texture
GLuint loadTerrainTexture() {
    GLuint textureID;
    //generate texture object
    glGenTextures(1, &textureID);

    int terrainSize, nrChannels;
    //load the texture file
    unsigned char* data = stbi_load(terrainTextureFilePath.c_str(), &terrainSize, &terrainSize, &nrChannels, 0);

    if (data) {
        //select RGB channel based on available channels
        GLenum format = nrChannels == 3 ? GL_RGB : GL_RGBA;
        //bind the texture
        glBindTexture(GL_TEXTURE_2D, textureID);
        //upload the texture data
        glTexImage2D(GL_TEXTURE_2D, 0, format, terrainSize, terrainSize, 0, format, GL_UNSIGNED_BYTE, data);
        //generate mipmaps for the texture
        glGenerateMipmap(GL_TEXTURE_2D);
        //free data
        stbi_image_free(data);
    }
    //handle error
    else {
        cerr << "Failed to load texture: " << terrainTextureFilePath << endl;
        //free data
        stbi_image_free(data);
    }

    return textureID;
}

//display the created terrain
void displayTerrain() {
    //generate height-map
    vector<float> heightMap = generateHeightMap(terrainSize);

    //vector for the terrain vertices and indices
    vector<float> terrain_vertices;
    vector<unsigned int> terrain_indices;

    //generate the vertices and indices for the main terrain mesh
    generateTerrain(heightMap, terrain_vertices, terrain_indices);
    GLuint textureMap = loadTerrainTexture();

    //generate and bind the vertex array object
    glGenVertexArrays(1, &terrain_vao);
    glBindVertexArray(terrain_vao);

    //generate, bind and upload the vertex buffer object to the buffer
    glGenBuffers(1, &terrain_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, terrain_vbo);
    glBufferData(GL_ARRAY_BUFFER, terrain_vertices.size() * sizeof(float), terrain_vertices.data(), GL_STATIC_DRAW);

    //generate, bind and upload the element buffer object to the buffer
    glGenBuffers(1, &terrain_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, terrain_indices.size() * sizeof(unsigned int), terrain_indices.data(), GL_STATIC_DRAW);

    //define and enable the position and normal attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //compile and enable the terrain shader program
    shaderProgramID_terrain = CompileShader(terrainVertexShader, terrainFragmentShader);

    //activate and bind and link the normal map uniform to the terrain texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureMap);
    glUniform1i(glGetUniformLocation(shaderProgramID_terrain, "normalMap"), 0);

    //get the location of the model, view and projection matrix uniforms
    int matrix_location = glGetUniformLocation(shaderProgramID_terrain, "model");
    int view_mat_location = glGetUniformLocation(shaderProgramID_terrain, "view");
    int proj_mat_location = glGetUniformLocation(shaderProgramID_terrain, "projection");

    //create the model, view and projection matrices
    mat4 terrain_model = identity_mat4();
    mat4 terrain_view = identity_mat4();
    mat4 persp_proj_terrain = perspective(45.0f, (float)width / (float)height, 0.1f, 600.0f);

    //translate terrain according to the keyboard-input tracker variables
    if (toggleTopView)
    {
        terrain_view = look_at(vec3(0.0f, 1000.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f));
    }
    else
    {
        terrain_model = translate(terrain_model, vec3(base_x, base_y, base_z));
    }

    //link the model, view and projections matrices
    glUniformMatrix4fv(matrix_location, 1, GL_FALSE, terrain_model.m);
    glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, terrain_view.m);
    glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj_terrain.m);

    //activate the terrain shader program
    glUseProgram(shaderProgramID_terrain);

    //set the terrain colour, light position and light colour uniforms
    glUniform3f(glGetUniformLocation(shaderProgramID_terrain, "terrainColor"), 0.9f, 0.7f, 0.5f);
    glUniform3f(glGetUniformLocation(shaderProgramID_terrain, "lightPosition"), 0.0f, 10.0f, 0.0f);
    glUniform3f(glGetUniformLocation(shaderProgramID_terrain, "lightColor"), 1.0f, 1.0f, 1.0f);

    //bind the vertex array object
    glBindVertexArray(terrain_vao);

    //draw the terrain
    glDrawElements(GL_TRIANGLES, (terrainSize - 1) * (terrainSize - 1) * 6, GL_UNSIGNED_INT, 0);
}
