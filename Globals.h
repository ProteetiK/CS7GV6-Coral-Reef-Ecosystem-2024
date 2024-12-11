#include <GL/glew.h>
#include <string>

//defines the dimensions of the output window
extern int width;
extern int height;

//variables to control the sine-wave movement of the fish schools
extern GLfloat move_fish_x;
extern GLfloat move_fish_y;
extern GLfloat move_fish_z;

//variables to help translate keyboard input into scene movement
extern GLfloat base_x;
extern GLfloat base_y;
extern GLfloat base_z;

//variables to control the rotation of the scene on both keyboard and trackpad inputs
extern float rotateX ;
extern float rotateY;

//variables to read and process mouse/trackpad input
extern int mouseX;
extern int mouseY;

//file path for all distinct 3D models that are used
static const char* fishMeshName = "D:/Computer Graphics/Lab1/Project0/Project0/Models/pez_amarillo.obj";
static const char* terrainMeshName = "D:/Computer Graphics/Lab1/Project0/Project0/Models/terrain.obj";
static const char* plantMeshName = "D:/Computer Graphics/Lab1/Project0/Project0/Models/10010_Coral_v1_L3.obj";

//file path for dynamic water texture
static const std::string& waterTextureFilePath = "D:/Computer Graphics/Lab1/Project0/Project0/Models/water_textures_2k.png";

//file path for coral reef texture
static const std::string& terrainTextureFilePath = "D:/Computer Graphics/Lab1/Project0/Project0/Models/closeup-beige-natural-coral-texture.jpg";

//file path for base vertex and fragment shaders
static const char* simpleVertexShader = "D:/Computer Graphics/Lab1/Project0/Project0/Shaders/simpleVertexShader.txt";
static const char* simpleFragmentShader = "D:/Computer Graphics/Lab1/Project0/Project0/Shaders/simpleFragmentShader.txt";

//file path for terrain vertex and fragment shaders
static const char* terrainVertexShader = "D:/Computer Graphics/Lab1/Project0/Project0/Shaders/terrainVertexShader.txt";
static const char* terrainFragmentShader = "D:/Computer Graphics/Lab1/Project0/Project0/Shaders/terrainFragmentShader.txt";

//file path for water texture vertex and fragment shaders
static const char* waterTexVertexShader = "D:/Computer Graphics/Lab1/Project0/Project0/Shaders/waterTexVertexShader.txt";
static const char* waterTexFragmentShader = "D:/Computer Graphics/Lab1/Project0/Project0/Shaders/waterTexFragmentShader.txt";

//file path for fish vertex shaders
static const char* fishVertexShader = "D:/Computer Graphics/Lab1/Project0/Project0/Shaders/fishVertexShader.txt";
static const char* pinkFishVertexShader = "D:/Computer Graphics/Lab1/Project0/Project0/Shaders/pinkFishVertexShader.txt";

//file path for plant vertex shaders
static const char* plantVertexShader = "D:/Computer Graphics/Lab1/Project0/Project0/Shaders/plantVertexShader.txt";
static const char* redPlantVertexShader = "D:/Computer Graphics/Lab1/Project0/Project0/Shaders/redPlantVertexShader.txt";