#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include "header.h"

static bool  gDragging = false;
static int   gLastX = 0, gLastY = 0;
static float gYawDeg = 0.0f;    // Y축 회전 (좌우 드래그)
static float gPitchDeg = 0.0f;  // X축 회전 (상하 드래그, 클램프)
static float gDistance = 7.0f; // 카메라 거리

// 마우스 버튼
void OnMouseButton(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) { gDragging = true;  gLastX = x; gLastY = y; }
        else { gDragging = false; }
    }
}

// 드래그(좌클릭 드래그 중)
void OnMouseDrag(int x, int y)
{
    if (!gDragging) return;
    const float sens = 0.3f; // 감도
    gYawDeg += (x - gLastX) * sens;   // 좌우 → Yaw
    gPitchDeg += (y - gLastY) * sens;   // 상하 → Pitch
    // 피치 클램프(뒤집힘 방지)
    if (gPitchDeg > 89.0f) gPitchDeg = 89.0f;
    if (gPitchDeg < -89.0f) gPitchDeg = -89.0f;
    gLastX = x; gLastY = y;
    glutPostRedisplay();
}

static inline Vector3 normalize(const Vector3& v) {
    float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (len == 0.0f) return { 0,0,0 };
    return { v.x / len, v.y / len, v.z / len };
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, gDistance, centerX, centerY, centerZ, 0, 1, 0);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    {
        const GLfloat Lpos[4] = { 4,4,10,1 };
        const GLfloat La[4] = { 0.2f,0.2f,0.2f,1 };
        const GLfloat Ld[4] = { 1,1,1,1 };
        const GLfloat Ls[4] = { 1,1,1,1 };
        glLightfv(GL_LIGHT0, GL_POSITION, Lpos);
        glLightfv(GL_LIGHT0, GL_AMBIENT, La);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, Ld);
        glLightfv(GL_LIGHT0, GL_SPECULAR, Ls);
    }

    glPushMatrix();
    glTranslatef(centerX, centerY, centerZ);
    glRotatef(gYawDeg, 0, 1, 0);
    glRotatef(gPitchDeg, 1, 0, 0);
    glTranslatef(-centerX, -centerY, -centerZ);

    // --- Squares (GL_POLYGON) ---
    for (size_t i = 0; i < gSquares.size(); ++i) {
        const Square& sq = gSquares[i];
        const bool hasUV = sq.hasUV;

        applyMaterial_Lit(sq.material, hasUV);

        glBegin(GL_POLYGON);
        const Vector3& n = gQuadNormals[i];
        glNormal3f(n.x, n.y, n.z);
        for (int k = 0; k < 4; ++k) {
            if (hasUV) {
                const Vector2& uv = gTextures[sq.textures[k]];
                glTexCoord2f(uv.u, uv.v);
            }
            const Vector3& p = gPositions[sq.indices[k]];
            glVertex3f(p.x, p.y, p.z);
        }
        glEnd();
    }

    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);

    glutSwapBuffers();
    glutPostRedisplay();
}
