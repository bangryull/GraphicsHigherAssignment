#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
/*----------------------------
png 파일을 불러오기 위한 라이브러리로 stb 라이브러리를 선택해서 불러왔습니다.
파일위치는 GL,glm과 같이 include 파일안에 존재합니다.
라이브러리를 찾는 과정에서 GPT-5의 도움으로 stb의 존재를 확인했습니다.
----------------------------*/

#include "header.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <GL/glew.h>
#include <GL/glut.h>

static std::string joinPath(const std::string& base, const std::string& rel) {
    if (rel.empty()) return rel;
    if (!base.empty() && base.back() != '/' && base.back() != '\\')
        return base + "/" + rel;
    return base + rel;
}

static GLuint createGLTextureFromImage(const unsigned char* data, int w, int h, int comp, bool srgb = false)
{
    GLuint id = 0;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    GLenum srcFmt = GL_RGB;
    GLenum internalFmt = GL_RGB;

    if (comp == 1) {
        srcFmt = GL_RED;
        internalFmt = GL_R8;
        GLint swizzleMask[] = { GL_RED, GL_RED, GL_RED, GL_ONE };
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
    }
    else if (comp == 2) {
        srcFmt = GL_RG;
        internalFmt = GL_RG8;
    }
    else if (comp == 3) {
        srcFmt = GL_RGB;
        internalFmt = srgb ? GL_SRGB8 : GL_RGB8;
    }
    else if (comp == 4) {
        srcFmt = GL_RGBA;
        internalFmt = srgb ? GL_SRGB8_ALPHA8 : GL_RGBA8;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, internalFmt, w, h, 0, srcFmt, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);
    return id;
}

void load_mtl(const std::string& filePath) {
    std::ifstream fin(filePath);
    if (!fin.is_open()) {
        std::cerr << "ERROR: Cannot open MTL file " << filePath << std::endl;
        return;
    }

    auto dirOf = [](const std::string& path)->std::string {
        size_t p = path.find_last_of("/\\");
        return (p == std::string::npos) ? std::string() : path.substr(0, p + 1);
        };
    const std::string mtlBaseDir = dirOf(filePath);

    std::string line;
    Material current;
    bool hasCurrent = false;

    stbi_set_flip_vertically_on_load(true);

    while (std::getline(fin, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::istringstream iss(line);
        std::string tag; iss >> tag;

        if (tag == "newmtl") {
            if (hasCurrent) gMaterials.push_back(current);
            current = Material();
            iss >> current.name;
            hasCurrent = true;
        }
        else if (tag == "Ka") { iss >> current.Ka[0] >> current.Ka[1] >> current.Ka[2]; }
        else if (tag == "Kd") { iss >> current.Kd[0] >> current.Kd[1] >> current.Kd[2]; }
        else if (tag == "Ks") { iss >> current.Ks[0] >> current.Ks[1] >> current.Ks[2]; }
        else if (tag == "Ns") { iss >> current.Ns; }
        else if (tag == "Ni") { iss >> current.Ni; }
        else if (tag == "d") { iss >> current.d; }
        else if (tag == "illum") { iss >> current.illum; }
        else if (tag == "map_Kd") {
            std::vector<std::string> tokens;
            std::string tok;
            while (iss >> tok) tokens.push_back(tok);

            if (!tokens.empty()) {
                std::string texRel;
                for (int i = (int)tokens.size() - 1; i >= 0; --i) {
                    if (!tokens[i].empty() && tokens[i][0] != '-') { texRel = tokens[i]; break; }
                }

                if (!texRel.empty()) {
                    current.map_Kd = texRel;

                    int w = 0, h = 0, comp = 0;
                    std::string full = joinPath(mtlBaseDir.empty() ? gOBJBaseDir : mtlBaseDir, texRel);
                    unsigned char* data = stbi_load(full.c_str(), &w, &h, &comp, 0);
                    if (data) {
                        current.texId = createGLTextureFromImage(data, w, h, comp, /*srgb=*/false);
                        stbi_image_free(data);
                        std::cout << "Loaded texture '" << full << "' (id=" << current.texId
                            << ", " << w << "x" << h << ", comp=" << comp << ")\n";
                    }
                    else {
                        std::cerr << "WARN: failed to load map_Kd image: " << full << "\n";
                    }
                }
            }
        }
    }
    if (hasCurrent) gMaterials.push_back(current);

    fin.close();
    std::cout << "Loaded " << gMaterials.size() << " materials from " << filePath << std::endl;
}
