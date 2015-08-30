#include "Zed3D.hpp"

Zed3D::Zed3D(float x, float y, float z) {
    px = x;
    py = y;
    pz = z;
}

Zed3D::~Zed3D() {
}

void Zed3D::draw() {
    glPushMatrix();
    glRotatef(180.0f, 0.0f, 1.0f, 0.0f);

	glScalef(0.1f, 0.1f, 0.1f);

    int vertex;
    float x, y, z;
    glBegin(GL_TRIANGLES);
    glColor3f(ALLUMINIUM_COLOR.r, ALLUMINIUM_COLOR.g, ALLUMINIUM_COLOR.b);
    for (int i = 0; i < NB_ALLUMINIUM_TRIANGLES * 3; i += 3) {
        drawVertex(alluminiumTriangles[i] - 1);
        drawVertex(alluminiumTriangles[i + 1] - 1);
        drawVertex(alluminiumTriangles[i + 2] - 1);
    }
    glColor3f(DARK_COLOR.r, DARK_COLOR.g, DARK_COLOR.b);
    for (int i = 0; i < NB_DARK_TRIANGLES * 3; i += 3) {
        drawVertex(darkTriangles[i] - 1);
        drawVertex(darkTriangles[i + 1] - 1);
        drawVertex(darkTriangles[i + 2] - 1);
    }
    glEnd();
    glPopMatrix();
}

void Zed3D::drawVertex(int index) {
    glVertex3f(vertices[index * 3], vertices[index * 3 + 1], vertices[index * 3 + 2]);
}
