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

unsigned int mesh_vao = 0;

GLuint loc1, loc2;
GLuint loc1_fish, loc2_fish;
GLuint loc1_plant, loc2_plant;
GLuint loc1_redPlant, loc2_redPlant;

float randX;
float randY;
float randZ;


void randomize(float positionRangeX, float positionRangeY, float positionRangeZ)
{
	randX = (static_cast<float>(std::rand()) / RAND_MAX) * positionRangeX - (positionRangeX / 2);
	randY = (static_cast<float>(std::rand()) / RAND_MAX) * positionRangeY - (positionRangeY / 2);
	randZ = (static_cast<float>(std::rand()) / RAND_MAX) * positionRangeZ - (positionRangeZ / 2);
}

float noise(float x) {
	return sin(sin(x)); // Simple noise-like behavior
}

float scatterFormula(float x, float a, float b) {
	//float base = x * x; // Base function
	float sineScatter = a * sin(-10 * x + 2.0);
	float cosineScatter = b * cos(-5 * x * x + 2.0);
	float noiseScatter = 0.2 * noise(x);
	return fmodf(sineScatter + cosineScatter + noiseScatter,60);
}

vec3 translationModel(int j)
{
	vec3 translation;
	float complexPeriodic = 0.0f;
	if (j % 2 == 0) {
		if (j % 2 == 0)
			complexPeriodic = scatterFormula(j, 10, 30) * 10;
		translation = vec3(-100.0f + complexPeriodic + base_x, -30.0f + fmodf((j - 60), 400) + base_y, -700.0f + base_z);
	}
	else {
		if (j % 2 != 0)
			complexPeriodic = scatterFormula(j, 50, 20) * 10;
		translation = vec3((-100.0f + complexPeriodic * -1.0f) + base_x, (60.0f + fmodf((j * 5), 400) * -1.0f) + base_y, -700.0f + complexPeriodic + base_z);
	}
	return translation;
}

void displayBase() {

	shaderProgramID = CompileShader(simpleVertexShader, simpleFragmentShader);

	glUseProgram(shaderProgramID);

	base_mesh_data = loadModelMesh(terrainMeshName);

	unsigned int vp_vbo = 0;
	loc1 = glGetAttribLocation(shaderProgramID, "vertex_position_base");
	loc2 = glGetAttribLocation(shaderProgramID, "vertex_normal_base");

	glGenBuffers(1, &vp_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glBufferData(GL_ARRAY_BUFFER, base_mesh_data.mPointCount * sizeof(vec3), &base_mesh_data.mVertices[0], GL_STATIC_DRAW);
	unsigned int vn_vbo = 0;
	glGenBuffers(1, &vn_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glBufferData(GL_ARRAY_BUFFER, base_mesh_data.mPointCount * sizeof(vec3), &base_mesh_data.mNormals[0], GL_STATIC_DRAW);
	unsigned int vao = 0;
	glBindVertexArray(vao);

	glEnableVertexAttribArray(loc1);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc2);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glVertexAttribPointer(loc2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glUseProgram(shaderProgramID);
	int matrix_location_base = glGetUniformLocation(shaderProgramID, "base_model");
	int view_mat_location_base = glGetUniformLocation(shaderProgramID, "base_view");
	int proj_mat_location_base = glGetUniformLocation(shaderProgramID, "base_proj");

	mat4 base_view = identity_mat4();
	mat4 persp_proj_base = perspective(20.0f, (float)width / (float)height, 0.1f, 1000.0f);

	mat4 base_model = identity_mat4();

	base_model = rotate_x_deg(base_model, 10.0f);
	base_model = rotate_x_deg(base_model, rotateX);
	base_model = rotate_y_deg(base_model, rotateY);

	base_view = translate(base_view, vec3(0.0f + base_x, -10.0f + base_y, base_z));

	glUniformMatrix4fv(proj_mat_location_base, 1, GL_FALSE, persp_proj_base.m);
	glUniformMatrix4fv(view_mat_location_base, 1, GL_FALSE, base_view.m);
	glUniformMatrix4fv(matrix_location_base, 1, GL_FALSE, base_model.m);
	glDrawArrays(GL_TRIANGLES, 0, base_mesh_data.mPointCount);
	glBindVertexArray(0);

}
void displayFish() {

	shaderProgramID_fish = CompileShader(fishVertexShader, simpleFragmentShader);
	fish_mesh_data = loadModelMesh(fishMeshName);
	loc1_fish = glGetAttribLocation(shaderProgramID_fish, "vertex_position_fish");
	loc2_fish = glGetAttribLocation(shaderProgramID_fish, "vertex_normal_fish");

	unsigned int vp_vbo = 0;
	glGenBuffers(1, &vp_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glBufferData(GL_ARRAY_BUFFER, fish_mesh_data.mPointCount * sizeof(vec3), &fish_mesh_data.mVertices[0], GL_STATIC_DRAW);
	unsigned int vn_vbo = 0;
	glGenBuffers(1, &vn_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glBufferData(GL_ARRAY_BUFFER, fish_mesh_data.mPointCount * sizeof(vec3), &fish_mesh_data.mNormals[0], GL_STATIC_DRAW);

	unsigned int vao_fish = 0;
	glBindVertexArray(vao_fish);

	glEnableVertexAttribArray(loc1_fish);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glVertexAttribPointer(loc1_fish, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc2_fish);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glVertexAttribPointer(loc2_fish, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glUseProgram(shaderProgramID_fish);
	int matrix_location = glGetUniformLocation(shaderProgramID_fish, "fish_model");
	int view_mat_location = glGetUniformLocation(shaderProgramID_fish, "fish_view");
	int proj_mat_location = glGetUniformLocation(shaderProgramID_fish, "fish_proj");

	mat4 fish_view = identity_mat4();
	mat4 persp_proj_fish = perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
	mat4 fish_model = identity_mat4();

	fish_model = rotate_z_deg(fish_model, 150.0f);
	fish_model = rotate_y_deg(fish_model, -90.0f);
	fish_model = translate(fish_model, vec3(move_fish_x, move_fish_y, -40.0f));
	fish_model = rotate_x_deg(fish_model, rotateX);
	fish_model = rotate_y_deg(fish_model, rotateY);

	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj_fish.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, fish_view.m);
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, fish_model.m);
	glDrawArrays(GL_TRIANGLES, 0, fish_mesh_data.mPointCount);

	mat4 fishModelChild = identity_mat4();
	for (int i = 0; i < 10; i++)
	{
		fishModelChild = identity_mat4();
		fishModelChild = rotate_z_deg(fishModelChild, 180);
		fishModelChild = rotate_y_deg(fishModelChild, rotateY);
		fishModelChild = translate(fishModelChild, vec3(0.0f, (5.9f + i), (0.0f + i / 2)));
		fish_model = translate(fish_model, vec3(0.0f, -move_fish_y, -40.0f));
		fishModelChild = fish_model * fishModelChild;

		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, fishModelChild.m);
		glDrawArrays(GL_TRIANGLES, 0, fish_mesh_data.mPointCount);
	}

	glBindVertexArray(0);
}
void displayPinkFish() {

	shaderProgramID_pinkFish = CompileShader(pinkFishVertexShader, simpleFragmentShader);
	fish_mesh_data = loadModelMesh(fishMeshName);
	loc1_fish = glGetAttribLocation(shaderProgramID_pinkFish, "vertex_position_pinkFish");
	loc2_fish = glGetAttribLocation(shaderProgramID_pinkFish, "vertex_normal_pinkFish");

	unsigned int vp_vbo = 0;
	glGenBuffers(1, &vp_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glBufferData(GL_ARRAY_BUFFER, fish_mesh_data.mPointCount * sizeof(vec3), &fish_mesh_data.mVertices[0], GL_STATIC_DRAW);
	unsigned int vn_vbo = 0;
	glGenBuffers(1, &vn_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glBufferData(GL_ARRAY_BUFFER, fish_mesh_data.mPointCount * sizeof(vec3), &fish_mesh_data.mNormals[0], GL_STATIC_DRAW);

	unsigned int vao_pinkFish = 0;
	glBindVertexArray(vao_pinkFish);

	glEnableVertexAttribArray(loc1_fish);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glVertexAttribPointer(loc1_fish, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc2_fish);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glVertexAttribPointer(loc2_fish, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glUseProgram(shaderProgramID_pinkFish);
	int matrix_location = glGetUniformLocation(shaderProgramID_pinkFish, "pinkFish_model");
	int view_mat_location = glGetUniformLocation(shaderProgramID_pinkFish, "pinkFish_view");
	int proj_mat_location = glGetUniformLocation(shaderProgramID_pinkFish, "pinkFish_proj");

	mat4 fish_view = identity_mat4();
	mat4 persp_proj_pinkFish = perspective(-45.0f, (float)width / (float)height, 0.1f, 500.0f);
	mat4 pinkFish_model = identity_mat4();

	pinkFish_model = rotate_z_deg(pinkFish_model, 150.0f);
	pinkFish_model = rotate_y_deg(pinkFish_model, -90.0f);
	pinkFish_model = translate(pinkFish_model, vec3(move_fish_x+6, move_fish_y, -40.0f));
	pinkFish_model = rotate_x_deg(pinkFish_model, rotateX);
	pinkFish_model = rotate_y_deg(pinkFish_model, rotateY);

	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj_pinkFish.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, fish_view.m);
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, pinkFish_model.m);
	glDrawArrays(GL_TRIANGLES, 0, fish_mesh_data.mPointCount);

	mat4 pinkFishModelChild = identity_mat4();
	for (int i = 0; i < 15; i++)
	{
		pinkFishModelChild = identity_mat4();
		pinkFishModelChild = rotate_z_deg(pinkFishModelChild, 180);
		pinkFishModelChild = rotate_y_deg(pinkFishModelChild, rotateY);
		pinkFishModelChild = translate(pinkFishModelChild, vec3(6.0f, (6.9f + i), (0.0f + i / 2)));
		pinkFish_model = translate(pinkFish_model, vec3(0.0f, -move_fish_y, -40.0f));
		pinkFishModelChild = pinkFish_model * pinkFishModelChild;

		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, pinkFishModelChild.m);
		glDrawArrays(GL_TRIANGLES, 0, fish_mesh_data.mPointCount);
	}

	glBindVertexArray(0);
}
void displayPlant() {

	shaderProgramID_plant = CompileShader(plantVertexShader, simpleFragmentShader);
	plant_mesh_data = loadModelMesh(plantMeshName);
	loc1_plant = glGetAttribLocation(shaderProgramID_plant, "vertex_position_plant");
	loc2_plant = glGetAttribLocation(shaderProgramID_plant, "vertex_normal_plant");

	unsigned int vp_vbo = 0;
	glGenBuffers(1, &vp_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glBufferData(GL_ARRAY_BUFFER, plant_mesh_data.mPointCount * sizeof(vec3), &plant_mesh_data.mVertices[0], GL_STATIC_DRAW);

	unsigned int vn_vbo = 0;
	glGenBuffers(1, &vn_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glBufferData(GL_ARRAY_BUFFER, plant_mesh_data.mPointCount * sizeof(vec3), &plant_mesh_data.mNormals[0], GL_STATIC_DRAW);

	unsigned int vao_plant = 0;
	glBindVertexArray(vao_plant);

	glEnableVertexAttribArray(loc1_plant);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glVertexAttribPointer(loc1_plant, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(loc2_plant);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glVertexAttribPointer(loc2_plant, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glUseProgram(shaderProgramID_plant);
	int matrix_location = glGetUniformLocation(shaderProgramID_plant, "plant_model");
	int view_mat_location = glGetUniformLocation(shaderProgramID_plant, "plant_view");
	int proj_mat_location = glGetUniformLocation(shaderProgramID_plant, "plant_proj");

	mat4 plant_view = identity_mat4();
	mat4 persp_proj_plant = perspective(45.0f, (float)width / (float)height, 0.1f, 600.0f);
	mat4 plant_model = identity_mat4();

	plant_model = rotate_z_deg(plant_model, 150.0f);
	plant_model = rotate_y_deg(plant_model, 90.0f);
	plant_model = translate(plant_model, vec3(-100.0f + base_x, -50.0f + base_y, -600.0f + base_z));
	plant_model = rotate_x_deg(plant_model, rotateX);
	plant_model = rotate_y_deg(plant_model, rotateY);
	plant_view = translate(plant_view, vec3(base_x, base_y, base_z));
	plant_model = rotate_x_deg(plant_model, rotateX);
	plant_model = rotate_y_deg(plant_model, rotateY);

	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj_plant.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, plant_view.m);
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, plant_model.m);

	glDrawArrays(GL_TRIANGLES, 0, plant_mesh_data.mPointCount);

	for (int i = 0; i < 10; ++i) {
		randomize(70.0f, 70.0f, 10.0f);

		mat4 plantModelChild = identity_mat4();

		plantModelChild = rotate_z_deg(plantModelChild, 150.0f);
		plantModelChild = rotate_y_deg(plantModelChild, 90.0f);

		plantModelChild = translate(plantModelChild, vec3(base_x+randX, base_y+randY, base_z+randZ));
		plantModelChild = plant_model * plantModelChild;

		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, plantModelChild.m);

		glDrawArrays(GL_TRIANGLES, 0, plant_mesh_data.mPointCount);
	}

	glBindVertexArray(0);
}

void displayRedPlant() {

	randomize(20.0f, 30.0f, 10.0f);
	
	shaderProgramID_redPlant = CompileShader(redPlantVertexShader, simpleFragmentShader);;
	redPlant_mesh_data = loadModelMesh(plantMeshName);
	loc1_redPlant = glGetAttribLocation(shaderProgramID_redPlant, "vertex_position_redPlant");
	loc2_redPlant = glGetAttribLocation(shaderProgramID_redPlant, "vertex_normal_redPlant");

	unsigned int vp_vbo = 0;
	glGenBuffers(1, &vp_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glBufferData(GL_ARRAY_BUFFER, redPlant_mesh_data.mPointCount * sizeof(vec3), &redPlant_mesh_data.mVertices[0], GL_STATIC_DRAW);

	unsigned int vn_vbo = 0;
	glGenBuffers(1, &vn_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glBufferData(GL_ARRAY_BUFFER, redPlant_mesh_data.mPointCount * sizeof(vec3), &redPlant_mesh_data.mNormals[0], GL_STATIC_DRAW);

	unsigned int vao_redPlant = 0;
	glBindVertexArray(vao_redPlant);

	glEnableVertexAttribArray(loc1_redPlant);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glVertexAttribPointer(loc1_redPlant, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(loc2_redPlant);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glVertexAttribPointer(loc2_redPlant, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glUseProgram(shaderProgramID_redPlant);
	int matrix_location = glGetUniformLocation(shaderProgramID_redPlant, "redPlant_model");
	int view_mat_location = glGetUniformLocation(shaderProgramID_redPlant, "redPlant_view");
	int proj_mat_location = glGetUniformLocation(shaderProgramID_redPlant, "redPlant_proj");
	
	for (int j = 0; j < 20; j++) {
		mat4 redPlant_view = identity_mat4();
		mat4 persp_proj_redPlant = perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
		mat4 redPlant_model = identity_mat4();

		redPlant_model = rotate_z_deg(redPlant_model, 150.0f);
		redPlant_model = rotate_y_deg(redPlant_model, 20.0f);		
		
		redPlant_model = translate(redPlant_model, translationModel(j));

		glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj_redPlant.m);
		glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, redPlant_view.m);
		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, redPlant_model.m);

		glDrawArrays(GL_TRIANGLES, 0, redPlant_mesh_data.mPointCount);
	}

	glBindVertexArray(0);
}

void displayWaterTexture() {
	Water water(width, height);
	const glm::mat4 viewMatrix = glm::mat4(1.0f);
	const glm::mat4 projMatrix = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 1000.0f);
	water.init();
	glDepthMask(GL_FALSE);
	water.render(viewMatrix, projMatrix, 0.05f);
	water.render(viewMatrix, projMatrix, -0.05f);
}