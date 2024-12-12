#include <vector>

//definitions
const float scaleUp = 0.1f;
const int terrainSize = 1000;

using namespace std;

//generate terrain height-map
vector<float> generateHeightMap(int size);

//generate vertices and indices for the main terrain mesh
void generateTerrain(vector<float>& heightMap, vector<float>& vertices, vector<unsigned int>& indices);

//generate the terrain texture
GLuint loadTerrainTexture();

//display the created terrain
void displayTerrain();
