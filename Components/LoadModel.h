#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <vector>
#include "maths_funcs.h"

#pragma once

using namespace std;

//define the basic model structure for the 3D mesh models loaded into the project
typedef struct ModelData
	{
		size_t mPointCount = 0;
		vector<vec3> mVertices;
		vector<vec3> mNormals;
		vector<vec2> mTextureCoords;
	} ModelData;

//load 3D models into the project
ModelData loadModelMesh(static const char* meshName);
