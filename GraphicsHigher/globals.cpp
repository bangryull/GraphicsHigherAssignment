#include "header.h"

/*---------------------------
GPT-5 모델에게 완성된 코드를 주어주고 깔끔하게 정리는 방법을 물어보았습니다.
헤더와 별개로 globals.cpp를 만들어 모든 값들을 따로 저장하여 
exturn으로 가져오는 방법을 추천하였고 그 방식대로 구현 했습니다.
---------------------------*/

std::vector<Material> gMaterials; //메터리얼 테이블
std::string gOBJBaseDir; //텍스처 기본 경로

std::vector<Vector2>  gTextures; //vt(u,v) 좌표
std::vector<Vector3>  gPositions; //v(x,y,z) 좌표
std::vector<Triangle> gTriangles; //삼각형저장, 3개의 vertex
std::vector<Square>   gSquares; //사각형 저장, 4개의 vertex

std::vector<Vector3>  gTriNormals; //gTriangles[i]의 면 법선
std::vector<Vector3>  gQuadNormals; //gSquares[i]의 면 법선

float centerX = 0, centerY = 0, centerZ = 0, radius = 1;
