//system imports
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <vector>
//GL imports
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <assimp/cimport.h>
#include <assimp/scene.h> 
#include <assimp/postprocess.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
//project imports
#include "maths_funcs.h"
#include "Components/Terrain/ProceduralTerrain.h"
#include "Components/GenerateObjectBuffer.h"
#include "Globals.h"
#include <GLFW/glfw3.h>

//screen dimensions
int width = 800;
int height = 600;
//initialize variables to control the sine-wave movement of the fish schools
GLfloat move_fish_x = 20.0f;
GLfloat move_fish_y = 0.0f;
GLfloat move_fish_z = -10.0f;

//initialize variables to help translate keyboard input into scene movement
GLfloat base_x = 160.0f;
GLfloat base_y = -60.0f;
GLfloat base_z = -20.0f;

//initalize variables to control the rotation of the scene on both keyboard and trackpad inputs
float rotateX = 0;
float rotateY = 0;

//initalize variables to read and process mouse/trackpad input
int mouseX = 0;
int mouseY = 0;

void createUnderWaterScene() {
	//display procedural terrain
	displayTerrain();
	//display swaying 3D green plant
	displayPlant();
	//display 3D model of reef supplication
	displayBase();
	//display school of 3D yellow fish
	displayFish();
	//display school of 3D pink fish
	displayPinkFish();
	//display static 3D red plant
	displayRedPlant();
	//display animated water texture
	glDisable(GL_DEPTH_TEST);
	displayWaterTexture();
}

void display() {
	//main display method
	//enable depth and blend modes
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LESS);
	//set sand colour to background
	glClearColor(0.9f, 0.8f, 0.6f, 0.8f);
	//clear entire screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//build actual scene
	createUnderWaterScene();
	//enable depth mask
	glDepthMask(GL_TRUE);
	//swap buffers
	glutSwapBuffers();
}

void updateScene() {
	//update scene
	static DWORD last_time = 0;
	//measure time for aiding translation
	DWORD curr_time = timeGetTime();
	if (last_time == 0)
		last_time = curr_time;
	//define time delta to a short interval
	float delta = (curr_time - last_time) * 0.001f;
	last_time = curr_time;
	move_fish_y -= delta;
	//define main fsine-wave for fish motion
	move_fish_y = sin(move_fish_y*10+delta) + cos(3* move_fish_y* move_fish_y+delta);
	//move_fish_y = sin(move_fish_y * 10 + delta) * 1.26f;
	move_fish_x -= 2.0f * delta;
	//limit the variance due to sine curve
	move_fish_x = fmodf(move_fish_x, 50);
	//redraw the scene
	glutPostRedisplay();
}

//track mouse/trackpad movement
void mousePassive(int x, int y) {
	mouseX = x;
	mouseY = y;
}

void mouseMotion(int x, int y) {
	//define trackpad input sensitivity
	const float SPEED = 1.2f;
	//define base and fish translation and rotation as per trackpad input
	//base translation component
	base_x += fmodf((mouseX - x) / SPEED,2);
	base_y += fmodf((mouseY - y) / SPEED, 2);
	//fish translation component
	move_fish_x += fmodf((mouseX - x) / SPEED, 2);
	move_fish_y += fmodf((mouseY - y) / SPEED, 2);
	//base rotation component
	rotateX += fmodf((mouseX - x) / SPEED, 2);
	rotateY += fmodf((mouseY - y) / SPEED, 2);
	//retrace mouse position
	mousePassive(x, y);
	//redraw the scene
	glutPostRedisplay();
}

void specialKeypress(int key, int x, int y) {
	//translate/move entire scene as per keyboard input
	switch (key) {
	case GLUT_KEY_RIGHT:
		//translation component for right-key
		base_x -= 8.0f;
		move_fish_x -= 8.0f;
		break;
	case GLUT_KEY_LEFT:
		//translation component for left-key
		base_x += 8.0f;
		move_fish_x += 8.0f;
		break;
	case GLUT_KEY_UP:
		//translation component for up-key
		base_y -= 8.0f;
		move_fish_y -= 8.0f;
		//rotation component for up-key
		rotateX -= 2.0f;
		rotateY -= 2.0f;
		break;
	case GLUT_KEY_DOWN:
		//translation component for down-key
		base_y += 8.0f;
		move_fish_y += 8.0f;
		//rotation component for down-key
		rotateX += 2.0f;
		rotateY += 2.0f;
		break;
	}
}

int main(int argc, char** argv) {
	//initialize the instance
	glutInit(&argc, argv);
	//launch a window to display scene
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutCreateWindow("Coral Reef Ecosystem");
	//trigger loop of drawing and refreshing scene with motion components
	glutDisplayFunc(display);
	glutIdleFunc(updateScene);
	//process keyboard/mouse/trackpad inputs
	glutSpecialFunc(specialKeypress);
	glutMotionFunc(mouseMotion);
	glutPassiveMotionFunc(mousePassive);
	//handle scene building error
	GLenum res = glewInit();
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}
	glfwSwapInterval(1);
	//run infinite loop to display scene
	glutMainLoop();
	return 0;
}
