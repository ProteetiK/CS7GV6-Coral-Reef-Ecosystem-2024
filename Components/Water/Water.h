//GL imports
#include <glm/glm.hpp>

//main rendering implementation
void waterRender(const mat4 viewMatrix, const mat4 projectionMatrix, float y, int water_width, int water_height);
   
//load amd map water texture
GLuint loadWaterTexture();
