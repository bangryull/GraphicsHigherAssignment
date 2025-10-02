#define _CRT_SECURE_NO_WARNINGS
#include <float.h>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <limits>
#include "header.h"

/* ------------------------------------------------------------------
obj파일을 불러오기 위한 이 코드는 지속된 AI GPT-5 모델 사용으로 기존 코드의 원형이 남지 않았습니다..
https://github.com/bangryull/CGAssignment8-1/blob/master/EmptyViewer/load_mesh.cpp
위는 저번 학기 그래픽스 과제에서 만들었던 obj파일 로더 코드입니다.
저 코드를 GPT에게 주고 고치는 방향으로 수정했지만 0번부터 차례대로 적용하며 수정시키다보니
밑의 코드처럼 원형이 남지 않게되었습니다.
------------------------------------------------------------------ */

static std::string dirOf(const std::string& path) {
    size_t p = path.find_last_of("/\\");
    return (p == std::string::npos) ? std::string() : path.substr(0, p + 1);
}

struct FaceVert { int v = 0, vt = 0, vn = 0; };

static FaceVert parseFaceToken(const std::string& s)
{
    FaceVert out;
    size_t p1 = s.find('/');
    if (p1 == std::string::npos) {// "v"
        out.v = std::stoi(s);
        return out;
    }
    out.v = std::stoi(s.substr(0, p1));

    size_t p2 = s.find('/', p1 + 1);
    if (p2 == std::string::npos) {// "v/vt"
        if (p1 + 1 < s.size())
            out.vt = std::stoi(s.substr(p1 + 1));
        return out;
    }

    // "v//vn" or "v/vt/vn"
    if (p2 > p1 + 1) {
        out.vt = std::stoi(s.substr(p1 + 1, p2 - (p1 + 1)));
    }
    if (p2 + 1 < s.size()) {
        out.vn = std::stoi(s.substr(p2 + 1));
    }
    return out;
}

// OBJ 1-based/음수 인덱스를 0-based로 변환
static int toZeroBased(int idxOBJ, int count)
{
    if (idxOBJ > 0) return idxOBJ - 1;     // 1..N  -> 0..N-1
    if (idxOBJ < 0) return count + idxOBJ; // -1    -> count-1
    return -1;                             // 0은 비정상
}

int findMaterialIndex(const std::string& name) {
    for (size_t i = 0; i < gMaterials.size(); ++i)
        if (gMaterials[i].name == name) return (int)i;
    return -1;
}
// ────────────────────────────────────────────────────────────────────────────


static Vector3 faceNormal3(const Vector3& a, const Vector3& b, const Vector3& c) {
    Vector3 u{ b.x - a.x, b.y - a.y, b.z - a.z };
    Vector3 v{ c.x - a.x, c.y - a.y, c.z - a.z };
    Vector3 n{
        u.y * v.z - u.z * v.y,
        u.z * v.x - u.x * v.z,
        u.x * v.y - u.y * v.x
    };
    float l = std::sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
    if (l > 0) { n.x /= l; n.y /= l; n.z /= l; }
    else { n = { 0,0,1 }; }
    return n;
}

void load_mesh(const std::string& fileName)
{
    gTextures.clear();
    gPositions.clear();
    gTriangles.clear();
    gSquares.clear();
    gTriNormals.clear();
    gQuadNormals.clear();

    std::ifstream fin(fileName.c_str());
    if (!fin.is_open()) {
        printf("ERROR: Unable to load mesh from %s!\n", fileName.c_str());
        exit(0);
    }

    gOBJBaseDir = dirOf(fileName);

    float xmin = FLT_MAX, xmax = -FLT_MAX, ymin = FLT_MAX, ymax = -FLT_MAX, zmin = FLT_MAX, zmax = -FLT_MAX;
    int currentMat = -1;

    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty() || line[0] == '#') continue;

        size_t first = line.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) continue;
        if (first > 0) line = line.substr(first);

        std::istringstream iss(line);
        std::string tag; iss >> tag;

        if (tag == "mtllib") {
            std::string mtlName; iss >> mtlName;
            std::string full = gOBJBaseDir + mtlName;
            load_mtl(full);
        }
        else if (tag == "usemtl") {
            std::string mname; iss >> mname;
            currentMat = findMaterialIndex(mname);
        }
        else if (tag == "v") {
            Vector3 p{}; iss >> p.x >> p.y >> p.z;
            xmin = std::min(xmin, p.x); xmax = std::max(xmax, p.x);
            ymin = std::min(ymin, p.y); ymax = std::max(ymax, p.y);
            zmin = std::min(zmin, p.z); zmax = std::max(zmax, p.z);
            gPositions.push_back(p);
        }
        else if (tag == "vt") {
            Vector2 t{}; iss >> t.u >> t.v;
            gTextures.push_back(t);
        }
        else if (tag == "f") {
            std::vector<std::string> fTok; std::string t;
            while (iss >> t) fTok.push_back(t);
            if (fTok.size() < 3) continue;

            std::vector<unsigned> vind, tind;
            vind.reserve(fTok.size()); tind.reserve(fTok.size());

            for (auto& ft : fTok) {
                FaceVert fv = parseFaceToken(ft);
                int vi = toZeroBased(fv.v, (int)gPositions.size());
                if (vi < 0 || vi >= (int)gPositions.size()) { vind.clear(); break; }
                vind.push_back((unsigned)vi);

                if (fv.vt != 0) {
                    int ti = toZeroBased(fv.vt, (int)gTextures.size());
                    if (ti < 0 || ti >= (int)gTextures.size()) { vind.clear(); break; }
                    tind.push_back((unsigned)ti);
                }
                else {
                    tind.push_back(0); // vt 미지정
                }
            }
            if (vind.empty()) continue;

            // === 폴리곤 분기 ===
            if (vind.size() == 3) {
                Triangle tri{};
                tri.indices[0] = vind[0]; tri.indices[1] = vind[1]; tri.indices[2] = vind[2];
                tri.textures[0] = tind[0]; tri.textures[1] = tind[1]; tri.textures[2] = tind[2];
                tri.material = currentMat;
                tri.hasUV = (tind[0] < gTextures.size() && tind[1] < gTextures.size() && tind[2] < gTextures.size());
                gTriangles.push_back(tri);
                // 면법선 캐시
                const Vector3 n = faceNormal3(gPositions[tri.indices[0]], gPositions[tri.indices[1]], gPositions[tri.indices[2]]);
                gTriNormals.push_back(n);
            }
            else if (vind.size() == 4) {
                // 사각형은 '폴리곤(4점)' 전제 유지
                Square sq{};
                for (int i = 0; i < 4; ++i) { sq.indices[i] = vind[i]; sq.textures[i] = tind[i]; }
                sq.material = currentMat;
                sq.hasUV = (tind[0] < gTextures.size() && tind[1] < gTextures.size() &&
                    tind[2] < gTextures.size() && tind[3] < gTextures.size());
                gSquares.push_back(sq);
                // 면법선 캐시(첫 3점으로)
                const Vector3 n = faceNormal3(gPositions[sq.indices[0]], gPositions[sq.indices[1]], gPositions[sq.indices[2]]);
                gQuadNormals.push_back(n);
            }
            else if (vind.size() > 4) {
                // 5각 이상은 삼각 팬으로 유지(렌더 편의상). 필요하면 Polygon으로도 확장 가능.
                for (size_t i = 1; i + 1 < vind.size(); ++i) {
                    Triangle tri{ {vind[0], vind[i], vind[i + 1]},
                                  {tind[0], tind[i], tind[i + 1]},
                                  currentMat, false };
                    tri.hasUV = (tri.textures[0] < gTextures.size() &&
                        tri.textures[1] < gTextures.size() &&
                        tri.textures[2] < gTextures.size());
                    gTriangles.push_back(tri);
                    const Vector3 n = faceNormal3(gPositions[tri.indices[0]], gPositions[tri.indices[1]], gPositions[tri.indices[2]]);
                    gTriNormals.push_back(n);
                }
            }
        }
    }

    centerX = 0.5f * (xmin + xmax);
    centerY = 0.5f * (ymin + ymax);
    centerZ = 0.5f * (zmin + zmax);
    radius = 0.5f * std::max({ xmax - xmin, ymax - ymin, zmax - zmin });

    fin.close();
}
