#include <vector>

const float scaleUp = 0.1f;
const int terrainSize = 1000;

std::vector<float> generateHeightMap(int size);

void generateTerrain(std::vector<float>& heightMap, std::vector<float>& vertices, std::vector<unsigned int>& indices);

void displayTerrain();