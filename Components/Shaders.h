#include <GL/glew.h>
    
//read contents of shader files
char* readShaderSource(const char* shaderFile);

//add the contents of the shader file to a shader program
static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);

//link vertex and fragment shaders to the shader program
GLuint CompileShader(const char* vertexShader, const char* fragmentShader);
