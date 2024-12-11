#include <GL/glew.h>
    
    char* readShaderSource(const char* shaderFile);

    static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);

    GLuint CompileShader(const char* vertexShader, const char* fragmentShader);
