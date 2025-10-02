#include <GL/glew.h>
#include <GL/glut.h>
#include "header.h"
#include <cstdio>

//마우스 입력 받기, display.cpp에 구현됨
extern void OnMouseButton(int button, int state, int x, int y); //클릭 입력
extern void OnMouseDrag(int x, int y); //드래그 입력

//reshape은 전적으로 GPT-5의 도움을 받았습니다.
static void reshape(int w, int h) { //창 크기 조절 시 호출
    glViewport(0, 0, w, h); // 픽셀 좌표계 설정
    glMatrixMode(GL_PROJECTION); // 투영 행렬 스택 선택
    glLoadIdentity(); // 투영 스택 초기화
    float aspect = (h == 0) ? 1.f : float(w) / float(h);
    glFrustum(-0.1f * aspect, 0.1f * aspect, -0.1f, 0.1f, 0.1f, 1000.f);
}

extern void display(); //display.cpp에 구현

//main함수는 적용순서, 예외 처리와 마우스 입력부분에서 GPT-5의 도움을 받았습니다.
int main(int argc, char** argv)
{
    glutInit(&argc, argv); // GLUT 초기화(명령행 파싱)
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // 더블버퍼 + RGBA + 깊이버퍼
    glutInitWindowSize(800, 600);
    glutCreateWindow("OBJ Viewer");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) { std::puts("GLEW init failed"); return 1; }

    glEnable(GL_DEPTH_TEST); // Z-test 켜기(가까운 픽셀이 보이도록)
    glDisable(GL_CULL_FACE); // 백페이스 컬링 끔(앞/뒤 다 그림, 모델 뒤집힘 대비)
    glClearColor(1, 1, 1, 1); // 배경색(흰색)

    // 메시 로드, loadobjfile.cpp에 구현되어 있습니다.
    load_mesh("Piggybank.obj");

    glutDisplayFunc(display); // 프레임 그리기 콜백
    glutReshapeFunc(reshape); // 창 리사이즈 콜백
    glutMouseFunc(OnMouseButton); //클릭 입력 받기
    glutMotionFunc(OnMouseDrag);  //드래그 입력 받기
    glutMainLoop();
    return 0;
}
