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

typedef struct ModelData
	{
		size_t mPointCount = 0;
		std::vector<vec3> mVertices;
		std::vector<vec3> mNormals;
		std::vector<vec2> mTextureCoords;
	} ModelData;

ModelData loadModelMesh(static const char* meshName);