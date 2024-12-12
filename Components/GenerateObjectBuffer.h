//generate random 3-axis point values for green plant animation
void randomize(float positionRangeX, float positionRangeY, float positionRangeZ);

//scatter algorithm for static red plant
float scatterFormula(float x, float a, float b);

//formulate translation for multiple instances of static red plant
vec3 translationModel(int j);

//display 3D model of reef supplication
void displayBase();

//display yellow school of 3D fish
void displayFish();

//display pink school of 3D fish
void displayPinkFish();

//display swaying 3D green plant
void displayPlant();

//display static 3D red plant
void displayRedPlant();

//display animated water texture
void displayWaterTexture();
