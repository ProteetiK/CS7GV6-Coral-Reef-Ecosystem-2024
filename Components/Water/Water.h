//GL imports
#include <glm/glm.hpp>

class Water {
public:
    //constructor
    Water(float water_width, float water_height);
    //initualizer
    void init();
    //rendering method
    void render(const glm::mat4 viewMatrix, const glm::mat4 projectionMatrix, float y);

private:
    //dimensions of water overlay
    float water_width;
    float water_height;

    //shader function
    GLuint shaderProgramID_water;
    GLuint water_vao, water_vbo;
    GLuint textureMap;
    //position variables
    GLint water_loc_model, water_loc_view, water_loc_proj, water_loc_time;

    //main implementation
    GLuint loadWaterTexture();
};