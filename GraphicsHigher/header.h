#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <GL/glew.h>
#include <vector>
#include <string>

// ---------- Material ----------
struct Material {
    std::string name;
    float Ka[3] = { 0,0,0 };
    float Kd[3] = { 0,0,0 };
    float Ks[3] = { 0,0,0 };
    float Ns = 0.0f;
    float Ni = 1.0f;
    float d = 1.0f;
    int   illum = 0;

    std::string map_Kd;   // 디퓨즈 텍스처 경로
    GLuint      texId = 0;
};

extern std::vector<Material> gMaterials;
extern std::string gOBJBaseDir;

// ---------- Geometry ----------
struct Vector2 { float u, v; };
struct Vector3 { float x, y, z; };

struct Triangle {
    unsigned int indices[3];
    unsigned int textures[3] = { 0,0,0 }; // vt
    int  material = -1;
    bool hasUV = false;
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
extern std::vector<Square>   gSquare;

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
