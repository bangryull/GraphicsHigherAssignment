#include "header.h"

/*---------------------------
GPT-5 �𵨿��� �ϼ��� �ڵ带 �־��ְ� ����ϰ� �����ϰ� �Ͽ����ϴ�.
����� ������ globals.cpp�� ����� ��� ������ ���� �����Ͽ� 
exturn���� �������� ����� ��õ�Ͽ��� �� ��Ĵ�� ���� �߽��ϴ�.
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
