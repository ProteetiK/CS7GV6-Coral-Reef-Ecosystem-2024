//system imports
#include <iostream>

//GL imports
#include <GL/glew.h>

//project imports
#include "Shaders.h"
#include "../Globals.h"

using namespace std;

//read contents of shader files
char* readShaderSource(const char* shaderFile) {
    FILE* fp;
    fopen_s(&fp, shaderFile, "rb");
    
    //handle error
    if (fp == NULL) { return NULL; }

    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);

    fseek(fp, 0L, SEEK_SET);
    char* buf = new char[size + 1];
    fread(buf, 1, size, fp);
    //add null value at the end of the file
    buf[size] = '\0';

    fclose(fp);
    return buf;
}

//add the contents of the shader file to a shader program
static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType) {
    //create shader object
    GLuint ShaderObj = glCreateShader(ShaderType);

    //handle error
    if (ShaderObj == 0) {
        cerr << "Error creating shader..." << endl;
        cerr << "Press enter/return to exit..." << endl;
        cin.get();
        exit(1);
    }
    //read shader code from file
    const char* pShaderSource = readShaderSource(pShaderText);

    //set source code, compile the shader and check for success
    glShaderSource(ShaderObj, 1, (const GLchar**)&pShaderSource, NULL);
    glCompileShader(ShaderObj);
    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

    //handle error
    if (!success) {
        GLchar InfoLog[1024] = { '\0' };
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        cerr << "Error compiling " << (ShaderType == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader program: " << InfoLog << endl;
        cerr << "Press enter/return to exit..." << endl;
        cin.get();
        exit(1);
    }
    //attach shader to shader program
    glAttachShader(ShaderProgram, ShaderObj);
}

//link vertex and fragment shaders to the shader program
GLuint CompileShader(const char* vertexShader, const char* fragmentShader) {
    
    //create shader program
    GLuint ShaderProgramID = glCreateProgram();
    
    //handle error
    if (ShaderProgramID == 0) {
        cerr << "Error creating shader program..." << endl;
        cerr << "Press enter/return to exit..." << endl;
        cin.get();
        exit(1);
    }

    //add vertex shader to shader program
    AddShader(ShaderProgramID, vertexShader, GL_VERTEX_SHADER); // Add the vertex shader to the program
    //add fragment shader to shader program
    AddShader(ShaderProgramID, fragmentShader, GL_FRAGMENT_SHADER);

    //link the shader program and check for success
    GLint Success = 0;
    GLchar ErrorLog[1024] = { '\0' };
    glLinkProgram(ShaderProgramID);
    glGetProgramiv(ShaderProgramID, GL_LINK_STATUS, &Success);

    //handle error
    if (Success == 0) { // If linking failed
        glGetProgramInfoLog(ShaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
        cerr << "Error linking shader program: " << ErrorLog << endl;
        cerr << "Press enter/return to exit..." << endl;
        cin.get();
        exit(1);
    }

    //validate shader program
    glValidateProgram(ShaderProgramID);
    glGetProgramiv(ShaderProgramID, GL_VALIDATE_STATUS, &Success);
    
    //handle error
    if (!Success) { 
        glGetProgramInfoLog(ShaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
        cerr << "Invalid shader program: " << ErrorLog << endl;
        cerr << "Press enter/return to exit..." << endl;
        cin.get();
        exit(1);
    }

    //set shader program to use and return the program Id
    glUseProgram(ShaderProgramID);
    return ShaderProgramID;
}
