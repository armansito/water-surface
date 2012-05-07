#include "waterengine.h"
#include "vector.h"

#ifndef __APPLE__
extern "C"
{
    void glBindBuffer (GLenum, GLuint);
    void glDeleteBuffers (GLsizei, const GLuint *);
    void glGenBuffers (GLsizei, GLuint *);
    GLboolean glIsBuffer (GLuint);
    void glBufferData (GLenum, GLsizeiptr, const GLvoid *, GLenum);
}
#endif

#define DIM 50

WaterEngine::WaterEngine()
{
    // initialize parameters
    m_params.kWavelength = 5.f;
    m_params.kSteepness = 0.8f;
    m_params.kAmpOverLen = 10.f;

    // build vbo with base mesh
    unsigned int size = (DIM)*(DIM)*4;
    Vector3 *vertices = new Vector3[size];
    Vector3 *v = &vertices[0];
    float x, z;
    z = -DIM/2.f;
    for (int i = 0; i < DIM; i++) {
        x = -DIM/2.f;
        for (int j = 0; j < DIM; j++) {
            v->x = x;
            v->y = 0.f;
            v->z = z;
            v++;
            v->x = x;
            v->y = 0.f;
            v->z = z + 1.f;
            v++;
            v->x = x + 1.f;
            v->y = 0.f;
            v->z = z + 1.f;
            v++;
            v->x = x + 1.f;
            v->y = 0.f;
            v->z = z;
            v++;
            x++;
        }
        z++;
    }

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, size * sizeof(Vector3), (GLvoid *)vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    m_count = size;
    delete[] vertices;
}

WaterEngine::~WaterEngine()
{
    glDeleteBuffers(1, &m_vbo);
}

void WaterEngine::render(float elapsed_time)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, 0);
    glDrawArrays(GL_QUADS, 0, m_count);
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


