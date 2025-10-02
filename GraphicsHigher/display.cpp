#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include "header.h"

/*-----------------------
화면을 렌더링하기 위한 코드입니다.
마우스 입력을 받는 부분에서 처음해보는 기능이 많았기에 GPT-5를 통해서 라이브러리 사용법과
피드백을 받았습니다.
-----------------------*/

static bool  gDragging = false;
static int   gLastX = 0, gLastY = 0;
static float gYawDeg = 0.0f;    // Y축 회전 (좌우 드래그)
static float gPitchDeg = 0.0f;  // X축 회전 (상하 드래그, 클램프)
static float gDistance = 7.0f; // 카메라 거리

// 클릭
void OnMouseButton(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) { 
            gDragging = true;  
            gLastX = x; gLastY = y;
        }
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
    // 피치 클램프(뒤집힘 방지)를 넣는 과정에서 개념이해와 구현에 GPT-5 도움을 받았습니다.
    if (gPitchDeg > 89.0f) gPitchDeg = 89.0f;
    if (gPitchDeg < -89.0f) gPitchDeg = -89.0f;
    gLastX = x; gLastY = y;
    glutPostRedisplay();
}

// normalize함수, GPT-5에게 간단하게 구현해달라고 했습니다.
static inline Vector3 normalize(const Vector3& v) {
    float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (len == 0.0f) return { 0,0,0 };
    return { v.x / len, v.y / len, v.z / len };
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    /*
    PiggyBank를 렌더링 할 때 오브젝트 자체가 원점에 위치하지 않아서 GPT-5에게 도움을 청했을 때,
    모델 중심을 계산하고 카메라를 그만큼 이동시키는 방식을 추천하여 적용시켰습니다.
    */
    gluLookAt(0, 0, gDistance, centerX, centerY, centerZ, 0, 1, 0);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    {
        //빛에 대한 초기값 권장 사항을 GPT-5에게 질문하고 적용했습니다.
        const GLfloat Lpos[4] = { 4,4,10,1 }; //Lpos는 제가 임의로 설정했습니다.
        const GLfloat La[4] = { 0.2f,0.2f,0.2f,1 };
        const GLfloat Ld[4] = { 1,1,1,1 };
        const GLfloat Ls[4] = { 1,1,1,1 };
        glLightfv(GL_LIGHT0, GL_POSITION, Lpos);
        glLightfv(GL_LIGHT0, GL_AMBIENT, La);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, Ld);
        glLightfv(GL_LIGHT0, GL_SPECULAR, Ls);
    }

    glPushMatrix();
    /*
    * 물체를 회전시킬 때 물체와 카메라가 원점에 있지 않으면 회전이 제대로 작동하지 않아 GPT-5에게 
    * 질문 하였고, center값을 이용해 이동시키고 회전시키라고 피드백 받은 후 적용하였습니다.
    */
    glTranslatef(centerX, centerY, centerZ);
    glRotatef(gYawDeg, 0, 1, 0);
    glRotatef(gPitchDeg, 1, 0, 0);
    glTranslatef(-centerX, -centerY, -centerZ);

    // --- Squares (GL_POLYGON) ---
    for (size_t i = 0; i < gSquares.size(); ++i) {
        const Square& sq = gSquares[i];
        const bool hasUV = sq.hasUV;

        //Material을 적용시키는 코드를 material.cpp에 따로 구현하라는 GPT-5의 피드백이 있었고
        //이를 적용시켜 따로 구현했습니다.
        applyMaterial_Lit(sq.material, hasUV);

        glBegin(GL_POLYGON);
        //normalize된 벡터를 캐싱하라는 GPT-5의 피드백이 있었고, 이를 추가하는 과정에서도
        //도움받았습니다.
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
