//system imports
#include <iostream>

//GL imports
#include <GL/glew.h>

//project imports
#include "Shaders.h"
#include "../Globals.h"

using namespace std;

	char* readShaderSource(const char* shaderFile) {
		FILE* fp;
		fopen_s(&fp, shaderFile, "rb");

		if (fp == NULL) { return NULL; }

		fseek(fp, 0L, SEEK_END);
		long size = ftell(fp);

		fseek(fp, 0L, SEEK_SET);
		char* buf = new char[size + 1];
		fread(buf, 1, size, fp);
		buf[size] = '\0';

		fclose(fp);

		return buf;
	}

	static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
	{
		GLuint ShaderObj = glCreateShader(ShaderType);

		if (ShaderObj == 0) {
			std::cerr << "Error creating shader..." << std::endl;
			std::cerr << "Press enter/return to exit..." << std::endl;
			std::cin.get();
			exit(1);
		}
		const char* pShaderSource = readShaderSource(pShaderText);

		glShaderSource(ShaderObj, 1, (const GLchar**)&pShaderSource, NULL);
		glCompileShader(ShaderObj);
		GLint success;
		glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
		if (!success) {
			GLchar InfoLog[1024] = { '\0' };
			glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
			std::cerr << "Error compiling "
				<< (ShaderType == GL_VERTEX_SHADER ? "vertex" : "fragment")
				<< " shader program: " << InfoLog << std::endl;
			std::cerr << "Press enter/return to exit..." << std::endl;
			std::cin.get();
			exit(1);
		}
		glAttachShader(ShaderProgram, ShaderObj);
	}

	GLuint CompileShader(const char* vertexShader, const char* fragmentShader) {
		GLuint ShaderProgramID = glCreateProgram();
		if (ShaderProgramID == 0) {
			std::cerr << "Error creating shader program..." << std::endl;
			std::cerr << "Press enter/return to exit..." << std::endl;
			std::cin.get();
			exit(1);
		}

		AddShader(ShaderProgramID, vertexShader, GL_VERTEX_SHADER);
		AddShader(ShaderProgramID, fragmentShader, GL_FRAGMENT_SHADER);

		GLint Success = 0;
		GLchar ErrorLog[1024] = { '\0' };
		glLinkProgram(ShaderProgramID);
		glGetProgramiv(ShaderProgramID, GL_LINK_STATUS, &Success);
		if (Success == 0) {
			glGetProgramInfoLog(ShaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
			std::cerr << "Error linking shader program: " << ErrorLog << std::endl;
			std::cerr << "Press enter/return tom exit..." << std::endl;
			std::cin.get();
			exit(1);
		}

		glValidateProgram(ShaderProgramID);
		glGetProgramiv(ShaderProgramID, GL_VALIDATE_STATUS, &Success);
		if (!Success) {
			glGetProgramInfoLog(ShaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
			std::cerr << "Invalid shader program: " << ErrorLog << std::endl;
			std::cerr << "Press enter/return to exit..." << std::endl;
			std::cin.get();
			exit(1);
		}
		glUseProgram(ShaderProgramID);
		return ShaderProgramID;
	}