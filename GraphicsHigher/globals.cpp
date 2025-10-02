#include "header.h"

/*---------------------------
GPT-5 모델에게 완성된 코드를 주어주고 깔끔하게 정리하게 하였습니다.
헤더와 별개로 globals.cpp를 만들어 모든 값들을 따로 저장하여 
exturn으로 가져오는 방법을 추천하였고 그 방식대로 구현 했습니다.
---------------------------*/

std::vector<Material> gMaterials;
std::string gOBJBaseDir;

std::vector<Vector2>  gTextures;
std::vector<Vector3>  gPositions;
std::vector<Triangle> gTriangles;
std::vector<Square>   gSquare;

std::vector<Vector3>  gTriNormals;
std::vector<Vector3>  gQuadNormals;

float centerX = 0, centerY = 0, centerZ = 0, radius = 1;
