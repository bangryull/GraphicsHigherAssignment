#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <GL/glew.h>
#include <vector>
#include <string>

// ---------- Material ----------
struct Material{
    std::string name;
    float Ka[3] = { 0,0,0 };  // Ambient  (환경광)
    float Kd[3] = { 0,0,0 };  // Diffuse  (확산광)
    float Ks[3] = { 0,0,0 };  // Specular (반사광)
    float Ns = 0.0f;          // Shininess(0..1000, GL은 0..128로 스케일)
    float Ni = 1.0f;          // Index of Refraction (굴절률, 사용 안 해도 OK)
    float d = 1.0f;           // Dissolve(불투명도, 1=불투명)
    int   illum = 0;          // MTL의 조명 모델(0/1=diffuse만, >=2 specular 포함)

    std::string map_Kd;       // 알베도(디퓨즈) 텍스처 경로
    GLuint      texId = 0;    // OpenGL 텍스처 핸들(0이면 바인딩 없음)
};

extern std::vector<Material> gMaterials;
extern std::string gOBJBaseDir;

// ---------- Geometry ----------
struct Vector2 { float u, v; };  // 텍스처 좌표
struct Vector3 { float x, y, z; }; // 정점 위치(월드/오브젝트)

struct Triangle {
    unsigned int indices[3]; // gPositions 인덱스(0-based)
    unsigned int textures[3] = { 0,0,0 }; // gTextures 인덱스(0-based)
    int  material = -1; // gMaterials 인덱스(-1=없음)
    bool hasUV = false; // 이 면이 유효한 UV를 갖는지
};

struct Square {
    unsigned int indices[4];
    unsigned int textures[4] = { 0,0,0 }; // vt
    int  material = -1;
    bool hasUV = false;
};

extern std::vector<Vector2>  gTextures;
extern std::vector<Vector3>  gPositions;
extern std::vector<Triangle> gTriangles;
extern std::vector<Square>   gSquares;

extern std::vector<Vector3>  gTriNormals; 
extern std::vector<Vector3>  gQuadNormals;

// ---------- Bounds ----------
extern float centerX, centerY, centerZ, radius;

// ---------- APIs ----------
void load_mesh(const std::string& fileName);
void load_mtl(const std::string& filePath);
int  findMaterialIndex(const std::string& name);

// lighting/material helper
void applyMaterial_Lit(int matIndex, bool useTexture);
