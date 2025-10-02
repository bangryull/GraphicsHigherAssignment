#include "header.h"
#include <algorithm>

static float clampf(float v, float lo, float hi) { return (v < lo ? lo : (v > hi ? hi : v)); }

void applyMaterial_Lit(int matIndex, bool useTexture)
{
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
