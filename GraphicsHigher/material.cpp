#include "header.h"
#include <algorithm>

/*
GPT-5를 통해 display.cpp에서 material 적용부분을 다른 cpp파일로 만들라는 피드백을 받았고
이를 통해 생긴 파일입니다.
*/

//범위를 고정하는 함수, GPT-5가 구현했습니다.
static float clampf(float v, float lo, float hi) { return (v < lo ? lo : (v > hi ? hi : v)); }

//재질, 블렌딩, 텍스쳐 세팅
void applyMaterial_Lit(int matIndex, bool useTexture)
{
    //재질을 못 찾았을 때 세팅 자동화, GPT-5를 통해 자동 생성되었습니다.
    if (matIndex < 0 || matIndex >= (int)gMaterials.size()) {
        const GLfloat Ka[4] = { 0.2f,0.2f,0.2f,1 };
        const GLfloat Kd[4] = { 0.8f,0.8f,0.8f,1 };
        const GLfloat Ks[4] = { 0,0,0,1 };
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, Ka);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, Kd);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, Ks);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
        return;
    }

    //mtl파일에서 Ns 범위가 GL보다 범위가 크다고 GPT-5에게 피드백 받은 후 적용했습니다.
    const Material& m = gMaterials[matIndex];
    float shin = clampf(m.Ns * (128.0f / 1000.0f), 0.0f, 128.0f);

    const GLfloat Ka[4] = { m.Ka[0], m.Ka[1], m.Ka[2], m.d };
    const GLfloat Kd[4] = { m.Kd[0], m.Kd[1], m.Kd[2], m.d };
    const GLfloat Ks[4] = { m.Ks[0], m.Ks[1], m.Ks[2], m.d };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, Ka);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, Kd);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, Ks);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shin);

    if (!(m.illum >= 2 && (m.Ks[0] > 0 || m.Ks[1] > 0 || m.Ks[2] > 0))) {
        const GLfloat Ks0[4] = { 0,0,0,m.d };
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, Ks0);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0);
    }

    if (m.d < 1.0f) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else {
        glDisable(GL_BLEND);
    }

    if (useTexture && m.texId != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, m.texId);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }
    else {
        glDisable(GL_TEXTURE_2D);
    }
}
