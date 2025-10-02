#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <GL/glew.h>
#include <vector>
#include <string>

/*--------------------
구조체 선언을 헤더에서 일괄적으로 처리하기 위해 만들었습니다.
이후 GPT-5에게 exturn을 이용한 선언방식을 피드백 받은 후에 
globals.cpp에 있는 정보를 끌어올 때의 코드는 gpt의 도움을 받았습니다.
--------------------*/

// ---------- Material ----------
struct Material{
    std::string name;
    float Ka[3] = { 0,0,0 };  // Ambient  (환경광)
    float Kd[3] = { 0,0,0 };  // Diffuse  (확산광)
    float Ks[3] = { 0,0,0 };  // Specular (반사광)
    float Ns = 0.0f;          // Shininess(밝기)
    float Ni = 1.0f;          // Index of Refraction (굴절률, 사용 안 해도 OK)
    float d = 1.0f;           // Dissolve(불투명도, 1=불투명)
    int   illum = 0;          // MTL의 조명 모델(0/1=diffuse만, >=2 specular 포함)

    //텍스쳐 경로와 텍스쳐 핸들은 GPT-5에게 mtl파일을 불러오는 코드를 받을 때 추가되었습니다.
    std::string map_Kd;       // 알베도(디퓨즈) 텍스처 경로
    GLuint      texId = 0;    // OpenGL 텍스처 핸들(0이면 바인딩 없음)
};

extern std::vector<Material> gMaterials;
extern std::string gOBJBaseDir;

// ---------- Geometry ----------
struct Vector2 { float u, v; };  // 텍스처 좌표
struct Vector3 { float x, y, z; }; // 정점 위치(월드/오브젝트)

//loadmtlfile.cpp를 만들면서 삼각형과 사각형에 hasUV와 메터리얼 인덱스를 추가하라는 
//GPT-5의 피드백을 받고 추가하였습니다.
struct Triangle {
    unsigned int indices[3]; // gPositions 인덱스(0-based)
    unsigned int textures[3] = { 0,0,0 }; // gTextures 인덱스(0-based)
    int  material = -1; // gMaterials 인덱스(-1=없음)
    bool hasUV = false; // 이 면이 유효한 UV를 갖는지
};

struct Square {
    unsigned int indices[4];
    unsigned int textures[4] = { 0,0,0 };
    int  material = -1;
    bool hasUV = false;
};

extern std::vector<Vector2>  gTextures; // OBJ vt 목록
extern std::vector<Vector3>  gPositions; // OBJ v 목록
extern std::vector<Triangle> gTriangles; // 삼각형 면들
extern std::vector<Square>   gSquares; // 사각형 면들

/*
Normalize된 법선 벡터를 따로 저장하는 공간은 GPT-5에게 loadobjfile.cpp를 구현요청했을 당시의
법선 벡터 캐싱 피드백으로 만들게 되었습니다.
*/
extern std::vector<Vector3>  gTriNormals; // gTriangles와 1:1 매칭(면 법선)
extern std::vector<Vector3>  gQuadNormals; // gSquares와 1:1 매칭(면 법선)

// ---------- Bounds ----------
/*
PiggyBank를 렌더링 할 때 오브젝트 자체가 원점에 위치하지 않아서 GPT-5에게 도움을 청했을 때,
모델 중심을 계산하고 카메라를 이동시키는 방식을 추천했고, obj로드할 때 중심점을 계산해서
가져오기 위하여 만든 변수들입니다.
*/
extern float centerX, centerY, centerZ, radius;

// ---------- APIs ----------
void load_mesh(const std::string& fileName); // OBJ 파서(정점/UV/면/재질명) + 면 법선 캐시 + 경계 계산.
void load_mtl(const std::string& filePath); // MTL 파서 + 텍스처 로딩(stb_image) + OpenGL 텍스처 생성.
int  findMaterialIndex(const std::string& name); //findMaterialIndex : gMaterials에서 이름으로 재질 인덱스 찾기.

// lighting/material helper
void applyMaterial_Lit(int matIndex, bool useTexture); //고정 파이프라인용 재질 바인딩 함수.
