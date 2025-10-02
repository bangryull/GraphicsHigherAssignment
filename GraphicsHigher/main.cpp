#include <GL/glew.h>
#include <GL/glut.h>
#include "header.h"
#include <cstdio>

//마우스 입력 받기
extern void OnMouseButton(int button, int state, int x, int y); //클릭 입력
extern void OnMouseDrag(int x, int y); //드래그 입력

static void reshape(int w, int h) { //크기 조절
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = (h == 0) ? 1.f : float(w) / float(h);
    glFrustum(-0.1f * aspect, 0.1f * aspect, -0.1f, 0.1f, 0.1f, 1000.f);
}

extern void display();

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OBJ Viewer");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) { std::puts("GLEW init failed"); return 1; }

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glClearColor(1, 1, 1, 1);

    // 메시 로드
    load_mesh("Piggybank.obj");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(OnMouseButton); //클릭 입력 받기
    glutMotionFunc(OnMouseDrag);  //드래그 입력 받기
    glutMainLoop();
    return 0;
}
