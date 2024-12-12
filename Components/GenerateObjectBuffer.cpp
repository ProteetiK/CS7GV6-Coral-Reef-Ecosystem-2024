//system imports
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <random>
#include <vector>

//GL imports
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/ext/matrix_clip_space.hpp>
#include "maths_funcs.h"

//project imports
#include "Shaders.h"
#include "LoadModel.h"
#include "../Globals.h"
#include "GenerateObjectBuffer.h"
#include "../Components/Water/Water.h"


GLuint shaderProgramID;
GLuint shaderProgramID_fish;
GLuint shaderProgramID_plant;
GLuint shaderProgramID_redPlant;
GLuint shaderProgramID_pinkFish;

ModelData base_mesh_data;
ModelData fish_mesh_data;
ModelData plant_mesh_data;
ModelData redPlant_mesh_data;

using namespace std;

#pragma region definition
unsigned int mesh_vao = 0;

GLuint loc1, loc2;
GLuint loc1_fish, loc2_fish;
GLuint loc1_plant, loc2_plant;
GLuint loc1_redPlant, loc2_redPlant;
float randX;
float randY;
float randZ;
#pragma endregion

#pragma region utility
//generate random 3-axis point values for green plant animation
void randomize(float positionRangeX, float positionRangeY, float positionRangeZ)
{
	//random value within specified range for x-axis
	randX = (static_cast<float>(rand()) / RAND_MAX) * positionRangeX - (positionRangeX / 2);
	//random value within specified range for y-axis
	randY = (static_cast<float>(rand()) / RAND_MAX) * positionRangeY - (positionRangeY / 2);
	//random value within specified range for z-axis
	randZ = (static_cast<float>(rand()) / RAND_MAX) * positionRangeZ - (positionRangeZ / 2);
}
//formulate translation for multiple instances of static red plant
float scatterFormula(float x, float a, float b) {
	//sine-wave scatter with scaling
	float sineScatter = a * sin(-10 * x + 2.0);
	//cos-wave scatter with scaling
	float cosineScatter = b * cos(-5 * x * x + 2.0);
	//noise simulation of scatter with scaling
	float noiseScatter = 0.2 * sin(sin(x));
	//composito of all three variables
	return fmodf(sineScatter + cosineScatter + noiseScatter, 60);
}
//scatter algorithm for static red plant
vec3 translationModel(int j)
{
	vec3 translation;
	float compositeScatter = 0.0f;
	//use even-odd and mod-4 split to simulate more entropy while building the translation variable
	if (j % 4 == 0) {
		if (j % 2 == 0)
			compositeScatter = scatterFormula(j, 10, 30) * 10;
		translation = vec3(-100.0f + compositeScatter + base_x, -30.0f + fmodf((j - 60), 400) + base_y, -700.0f + base_z);
	}
	else {
		if (j % 2 != 0)
			compositeScatter = scatterFormula(j, 50, 20) * 10;
		translation = vec3((-100.0f + compositeScatter * -1.0f) + base_x, (60.0f + fmodf((j * 5), 400) * -1.0f) + base_y, -700.0f + compositeScatter + base_z);
	}
	return translation;
}
#pragma endregion

#pragma region models
//display 3D model of reef supplication
void displayBase() {
	//compile and enable the base shader program
	shaderProgramID = CompileShader(simpleVertexShader, simpleFragmentShader);
	//activate the shader program
	glUseProgram(shaderProgramID);
	//load base model data
	base_mesh_data = loadModelMesh(terrainMeshName);

	//initialize vertex buffer object
	unsigned int vp_vbo = 0;

	//retrieve the position and normal locations from the shaders
	loc1 = glGetAttribLocation(shaderProgramID, "vertex_position_base");
	loc2 = glGetAttribLocation(shaderProgramID, "vertex_normal_base");

	//generate, bind and link the vbo for the base positions and normals
	glGenBuffers(1, &vp_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glBufferData(GL_ARRAY_BUFFER, base_mesh_data.mPointCount * sizeof(vec3), &base_mesh_data.mVertices[0], GL_STATIC_DRAW);
	unsigned int vn_vbo = 0;
	glGenBuffers(1, &vn_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glBufferData(GL_ARRAY_BUFFER, base_mesh_data.mPointCount * sizeof(vec3), &base_mesh_data.mNormals[0], GL_STATIC_DRAW);

	//initialize vertex array object
	unsigned int vao = 0; 
	glBindVertexArray(vao);

	//enable and bind the vertex attributes for vertex positions and normals
	glEnableVertexAttribArray(loc1);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc2);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glVertexAttribPointer(loc2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	//activate the shader program
	glUseProgram(shaderProgramID);

	//get the model, view and projection uniform location matrices
	int model_location_base = glGetUniformLocation(shaderProgramID, "base_model");
	int view_mat_location_base = glGetUniformLocation(shaderProgramID, "base_view");
	int proj_mat_location_base = glGetUniformLocation(shaderProgramID, "base_proj");

	//create base model, view and projection matrices
	mat4 base_model = identity_mat4();
	mat4 base_view = identity_mat4();
	mat4 persp_proj_base = perspective(20.0f, (float)width / (float)height, 0.1f, 1000.0f);	
	//apply rotations and translations 
	if (toggleTopView)
	{
		base_view = look_at(vec3(0.0f, 1000.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f));
	}
	else
	{
		base_model = rotate_x_deg(base_model, 10.0f + rotateX);
		base_model = rotate_y_deg(base_model, rotateY);
		base_view = translate(base_view, vec3(0.0f + base_x, -10.0f + base_y, base_z));
	}
	
	//link the model, view and projection matrix uniforms
	glUniformMatrix4fv(model_location_base, 1, GL_FALSE, base_model.m);
	glUniformMatrix4fv(view_mat_location_base, 1, GL_FALSE, base_view.m);
	glUniformMatrix4fv(proj_mat_location_base, 1, GL_FALSE, persp_proj_base.m);

	//render the base
	glDrawArrays(GL_TRIANGLES, 0, base_mesh_data.mPointCount);

	//unbind the vao
	glBindVertexArray(0);

}
//display yellow school of 3D fish
void displayFish() {
	//compile and enable the yellow fish shader program
	shaderProgramID_fish = CompileShader(fishVertexShader, simpleFragmentShader);
	//load yellow fish model data
	fish_mesh_data = loadModelMesh(fishMeshName);
	
	//retrieve the position and normal locations from the shaders
	loc1_fish = glGetAttribLocation(shaderProgramID_fish, "vertex_position_fish");
	loc2_fish = glGetAttribLocation(shaderProgramID_fish, "vertex_normal_fish");
	
	//generate, bind and link the vbo for the fish positions and normals
	unsigned int vp_vbo = 0;
	glGenBuffers(1, &vp_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glBufferData(GL_ARRAY_BUFFER, fish_mesh_data.mPointCount * sizeof(vec3), &fish_mesh_data.mVertices[0], GL_STATIC_DRAW);
	unsigned int vn_vbo = 0;
	glGenBuffers(1, &vn_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glBufferData(GL_ARRAY_BUFFER, fish_mesh_data.mPointCount * sizeof(vec3), &fish_mesh_data.mNormals[0], GL_STATIC_DRAW);

	//initialize vertex array object
	unsigned int vao_fish = 0;
	glBindVertexArray(vao_fish);

	//enable and bind the vertex attributes for vertex positions and normals
	glEnableVertexAttribArray(loc1_fish);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glVertexAttribPointer(loc1_fish, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc2_fish);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glVertexAttribPointer(loc2_fish, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	
	//activate the shader program
	glUseProgram(shaderProgramID_fish);
	
	//get the model, view and projection uniform location matrices
	int model_mat_location = glGetUniformLocation(shaderProgramID_fish, "fish_model");
	int view_mat_location = glGetUniformLocation(shaderProgramID_fish, "fish_view");
	int proj_mat_location = glGetUniformLocation(shaderProgramID_fish, "fish_proj");

	//create fish model, view and projection matrices
	mat4 fish_model = identity_mat4();
	mat4 fish_view = identity_mat4();
	mat4 persp_proj_fish = perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);

	//apply rotations and translations
	if (toggleTopView)
	{
		fish_view = look_at(vec3(0.0f, 70.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f));
		fish_model = translate(fish_model, vec3(10.0f, 5.0f, 0.0f));
	}
	else {
		fish_model = rotate_z_deg(fish_model, 150.0f);
		fish_model = rotate_y_deg(fish_model, -90.0f);
		fish_model = translate(fish_model, vec3(move_fish_x, move_fish_y, -40.0f));
		fish_model = rotate_x_deg(fish_model, rotateX);
		fish_model = rotate_y_deg(fish_model, rotateY);
	}

	//link the model, view and projection matrix uniforms
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj_fish.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, fish_view.m);
	glUniformMatrix4fv(model_mat_location, 1, GL_FALSE, fish_model.m);

	//render the yellow school of fish
	glDrawArrays(GL_TRIANGLES, 0, fish_mesh_data.mPointCount);

	if (!toggleTopView)
	{
		//define child matrix for the fish
		mat4 fishModelChild = identity_mat4();
		//run loop to create 10 instances
		for (int i = 0; i < 10; i++)
		{
			//apply rotations and translations
			fishModelChild = identity_mat4();
			fishModelChild = rotate_z_deg(fishModelChild, 180);
			fishModelChild = rotate_y_deg(fishModelChild, rotateY);
			fishModelChild = translate(fishModelChild, vec3(0.0f, (5.9f + i), (0.0f + i / 2)));
			fish_model = translate(fish_model, vec3(0.0f, -move_fish_y, -40.0f));

			//link model child to the parent model
			fishModelChild = fish_model * fishModelChild;

			//link the matrix locations to the fish model
			glUniformMatrix4fv(model_mat_location, 1, GL_FALSE, fishModelChild.m);

			//draw the baby fish
			glDrawArrays(GL_TRIANGLES, 0, fish_mesh_data.mPointCount);
		}
	}

	//unbind the vao
	glBindVertexArray(0);
}
//display pink school of 3D fish
void displayPinkFish() {
	//compile and enable the pink fish shader program
	shaderProgramID_pinkFish = CompileShader(pinkFishVertexShader, simpleFragmentShader);
	//load pink fish model data
	fish_mesh_data = loadModelMesh(fishMeshName);

	//retrieve the position and normal locations from the shaders
	loc1_fish = glGetAttribLocation(shaderProgramID_pinkFish, "vertex_position_pinkFish");
	loc2_fish = glGetAttribLocation(shaderProgramID_pinkFish, "vertex_normal_pinkFish");

	//generate, bind and link the vbo for the fish positions and normals
	unsigned int vp_vbo = 0;
	glGenBuffers(1, &vp_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glBufferData(GL_ARRAY_BUFFER, fish_mesh_data.mPointCount * sizeof(vec3), &fish_mesh_data.mVertices[0], GL_STATIC_DRAW);
	unsigned int vn_vbo = 0;
	glGenBuffers(1, &vn_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glBufferData(GL_ARRAY_BUFFER, fish_mesh_data.mPointCount * sizeof(vec3), &fish_mesh_data.mNormals[0], GL_STATIC_DRAW);

	//initialize vertex array object
	unsigned int vao_pinkFish = 0;
	glBindVertexArray(vao_pinkFish);

	//enable and bind the vertex attributes for vertex positions and normals
	glEnableVertexAttribArray(loc1_fish);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glVertexAttribPointer(loc1_fish, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc2_fish);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glVertexAttribPointer(loc2_fish, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	
	//activate the shader program
	glUseProgram(shaderProgramID_pinkFish);

	//get the model, view and projection uniform location matrices
	int model_mat_location = glGetUniformLocation(shaderProgramID_pinkFish, "pinkFish_model");
	int view_mat_location = glGetUniformLocation(shaderProgramID_pinkFish, "pinkFish_view");
	int proj_mat_location = glGetUniformLocation(shaderProgramID_pinkFish, "pinkFish_proj");

	//create pink fish model, view and projection matrices
	mat4 pinkFish_model = identity_mat4();
	mat4 fish_view = identity_mat4();
	mat4 persp_proj_pinkFish = perspective(-45.0f, (float)width / (float)height, 0.1f, 500.0f);
	if (toggleTopView)
	{
		fish_view = look_at(vec3(0.0f, 70.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f));
	}
	else
	{
		//apply rotations and translations
		pinkFish_model = rotate_z_deg(pinkFish_model, 150.0f);
		pinkFish_model = rotate_y_deg(pinkFish_model, -90.0f);
		pinkFish_model = translate(pinkFish_model, vec3(move_fish_x + 6, move_fish_y, -40.0f));
		pinkFish_model = rotate_x_deg(pinkFish_model, rotateX);
		pinkFish_model = rotate_y_deg(pinkFish_model, rotateY);
	}

	//link the model, view and projection matrix uniforms
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj_pinkFish.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, fish_view.m);
	glUniformMatrix4fv(model_mat_location, 1, GL_FALSE, pinkFish_model.m);

	//render the pink school of fish
	glDrawArrays(GL_TRIANGLES, 0, fish_mesh_data.mPointCount);

	if (!toggleTopView)
	{
		//define child matrix for the fish
		mat4 pinkFishModelChild = identity_mat4();
		//run loop to create 15 instances
		for (int i = 0; i < 15; i++)
		{
			//apply rotations and translations
			pinkFishModelChild = identity_mat4();
			pinkFishModelChild = rotate_z_deg(pinkFishModelChild, 180);
			pinkFishModelChild = rotate_y_deg(pinkFishModelChild, rotateY);
			pinkFishModelChild = translate(pinkFishModelChild, vec3(6.0f, (6.9f + i), (0.0f + i / 2)));
			pinkFish_model = translate(pinkFish_model, vec3(0.0f, -move_fish_y, -40.0f));
			pinkFishModelChild = pinkFish_model * pinkFishModelChild;

			//link the matrix locations to the fish model
			glUniformMatrix4fv(model_mat_location, 1, GL_FALSE, pinkFishModelChild.m);

			//draw the baby fish
			glDrawArrays(GL_TRIANGLES, 0, fish_mesh_data.mPointCount);
		}
	}

	//unbind the vao
	glBindVertexArray(0);
}
//display swaying 3D green plant
void displayPlant() {
	//compile and enable the green plant shader program
	shaderProgramID_plant = CompileShader(plantVertexShader, simpleFragmentShader);
	//load green plant model data
	plant_mesh_data = loadModelMesh(plantMeshName);

	//retrieve the position and normal locations from the shaders
	loc1_plant = glGetAttribLocation(shaderProgramID_plant, "vertex_position_plant");
	loc2_plant = glGetAttribLocation(shaderProgramID_plant, "vertex_normal_plant");

	//generate, bind and link the vbo for the plant positions and normals
	unsigned int vp_vbo = 0;
	glGenBuffers(1, &vp_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glBufferData(GL_ARRAY_BUFFER, plant_mesh_data.mPointCount * sizeof(vec3), &plant_mesh_data.mVertices[0], GL_STATIC_DRAW);
	unsigned int vn_vbo = 0;
	glGenBuffers(1, &vn_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glBufferData(GL_ARRAY_BUFFER, plant_mesh_data.mPointCount * sizeof(vec3), &plant_mesh_data.mNormals[0], GL_STATIC_DRAW);

	//initialize vertex array object
	unsigned int vao_plant = 0;
	glBindVertexArray(vao_plant);

	//enable and bind the vertex attributes for vertex positions and normals
	glEnableVertexAttribArray(loc1_plant);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glVertexAttribPointer(loc1_plant, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc2_plant);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glVertexAttribPointer(loc2_plant, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	
	//activate the shader program
	glUseProgram(shaderProgramID_plant);

	//get the model, view and projection uniform location matrices
	int model_mat_location = glGetUniformLocation(shaderProgramID_plant, "plant_model");
	int view_mat_location = glGetUniformLocation(shaderProgramID_plant, "plant_view");
	int proj_mat_location = glGetUniformLocation(shaderProgramID_plant, "plant_proj");

	//create plant model, view and projection matrices
	mat4 plant_model = identity_mat4();
	mat4 plant_view = identity_mat4();
	mat4 persp_proj_plant = perspective(45.0f, (float)width / (float)height, 0.1f, 600.0f);
	
	//apply rotations and translations
	if (toggleTopView)
	{
		plant_view = look_at(vec3(0.0f, 500.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f));
		plant_model = translate(plant_model, vec3(20.0f, 5.0f, 16.0f));
	}
	else
	{
		plant_model = rotate_z_deg(plant_model, 150.0f);
		plant_model = rotate_y_deg(plant_model, 90.0f);
		plant_model = translate(plant_model, vec3(-100.0f + base_x, -50.0f + base_y, -600.0f + base_z));
		plant_model = rotate_x_deg(plant_model, rotateX);
		plant_model = rotate_y_deg(plant_model, rotateY);
		plant_view = translate(plant_view, vec3(base_x, base_y, base_z));
		plant_model = rotate_x_deg(plant_model, rotateX);
		plant_model = rotate_y_deg(plant_model, rotateY);
	}
	//link the model, view and projection matrix uniforms
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj_plant.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, plant_view.m);
	glUniformMatrix4fv(model_mat_location, 1, GL_FALSE, plant_model.m);

	//render the green plant
	glDrawArrays(GL_TRIANGLES, 0, plant_mesh_data.mPointCount);

	if (!toggleTopView) {
		//run loop to create 10 child instances
		for (int i = 0; i < 10; ++i) {
			//create random position values
			randomize(70.0f, 70.0f, 10.0f);

			//define child matrix for the plant
			mat4 plantModelChild = identity_mat4();

			//apply rotations and translations
			plantModelChild = rotate_z_deg(plantModelChild, 150.0f);
			plantModelChild = rotate_y_deg(plantModelChild, 90.0f);
			plantModelChild = translate(plantModelChild, vec3(base_x + randX, base_y + randY, base_z + randZ));
			plantModelChild = plant_model * plantModelChild;

			//link the matrix locations to the plant model
			glUniformMatrix4fv(model_mat_location, 1, GL_FALSE, plantModelChild.m);

			//draw the plant animation instances
			glDrawArrays(GL_TRIANGLES, 0, plant_mesh_data.mPointCount);
		}
	}
	//unbind the vao
	glBindVertexArray(0);
}
//display static 3D red plant
void displayRedPlant() {
	//compile and enable the red plant shader program
	shaderProgramID_redPlant = CompileShader(redPlantVertexShader, simpleFragmentShader);
	//load red plant model data
	redPlant_mesh_data = loadModelMesh(plantMeshName);

	//retrieve the position and normal locations from the shaders
	loc1_redPlant = glGetAttribLocation(shaderProgramID_redPlant, "vertex_position_redPlant");
	loc2_redPlant = glGetAttribLocation(shaderProgramID_redPlant, "vertex_normal_redPlant");

	//generate, bind and link the vbo for the plant positions and normals
	unsigned int vp_vbo = 0;
	glGenBuffers(1, &vp_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glBufferData(GL_ARRAY_BUFFER, redPlant_mesh_data.mPointCount * sizeof(vec3), &redPlant_mesh_data.mVertices[0], GL_STATIC_DRAW);
	unsigned int vn_vbo = 0;
	glGenBuffers(1, &vn_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glBufferData(GL_ARRAY_BUFFER, redPlant_mesh_data.mPointCount * sizeof(vec3), &redPlant_mesh_data.mNormals[0], GL_STATIC_DRAW);

	//initialize vertex array object
	unsigned int vao_redPlant = 0;
	glBindVertexArray(vao_redPlant);

	//get the model, view and projection uniform location matrices
	glEnableVertexAttribArray(loc1_redPlant);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glVertexAttribPointer(loc1_redPlant, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc2_redPlant);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glVertexAttribPointer(loc2_redPlant, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	
	//activate the shader program
	glUseProgram(shaderProgramID_redPlant);

	//get the model, view and projection uniform location matrices
	int model_mat_location = glGetUniformLocation(shaderProgramID_redPlant, "redPlant_model");
	int view_mat_location = glGetUniformLocation(shaderProgramID_redPlant, "redPlant_view");
	int proj_mat_location = glGetUniformLocation(shaderProgramID_redPlant, "redPlant_proj");

	//run loop to create 20 instances
	for (int j = 0; j < 20; j++) {
		//create red plant model, view and projection matrices
		mat4 redPlant_view = identity_mat4();
		mat4 persp_proj_redPlant = perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
		mat4 redPlant_model = identity_mat4();

		//apply rotations and translations
		if (toggleTopView)
		{
			redPlant_view = look_at(vec3(0.0f, 10.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f));
			redPlant_model = translate(redPlant_model, translationModel(j));
		}
		else {
			redPlant_model = rotate_z_deg(redPlant_model, 150.0f);
			redPlant_model = rotate_y_deg(redPlant_model, 20.0f);
			redPlant_model = translate(redPlant_model, translationModel(j));
		}

		//link the model, view and projection matrix uniforms
		glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj_redPlant.m);
		glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, redPlant_view.m);
		glUniformMatrix4fv(model_mat_location, 1, GL_FALSE, redPlant_model.m);

		//draw the red plant instances
		glDrawArrays(GL_TRIANGLES, 0, redPlant_mesh_data.mPointCount);
	}

	//unbind the vao
	glBindVertexArray(0);
}
#pragma endregion

//display animated water texture
void displayWaterTexture() {
	//create view and projection matrices for the water texture
	const mat4 viewMatrix = identity_mat4();
	const mat4 projMatrix = perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
	//disable depth mask
	glDepthMask(GL_FALSE);
	//render the water texture
	waterRender(viewMatrix, projMatrix, 0.05f, width, height);
	waterRender(viewMatrix, projMatrix, -0.05f, width, height);
}
