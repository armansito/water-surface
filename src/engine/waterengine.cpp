#include "waterengine.h"

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

WaterEngine::Wave::Wave() {}
WaterEngine::Wave::Wave(const WaveParameters &p)
{
    float wl = p.wavelength;
    wl = frand() * (2.f * wl - 0.5f * wl) + 0.5f * wl;

    static float angle = frand() * M_PI_2 - M_PI_4;
    float c = cos(angle);
    float s = sin(angle);
    Vector2 dir(c * p.wave_dir.x - s * p.wave_dir.y,
                s * p.wave_dir.x + c * p.wave_dir.x);

    float st = p.steepness;
    st = frand() * (2.f * st - 0.5f * st) + 0.5f * st;

    params.wavelength = wl;
    params.steepness = st;
    params.kAmpOverLen = p.kAmpOverLen;
    params.wave_dir = dir;
}

WaterEngine::WaterEngine()
{
    // initialize parameters
    m_params.wavelength = 5.f;
    m_params.steepness = 0.8f;
    m_params.kAmpOverLen = 10.f;
    m_params.wave_dir = Vector2(-1.f, 1.f).unit();

    // initialize waves
    initializeWaves();

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

    m_waveprog = new QGLShaderProgram();
    m_waveprog->addShaderFromSourceCode(QGLShader::Vertex,
            "void main(void)"
            "{"
            "   gl_Position = ftransform();"
            "}");

    m_waveprog->addShaderFromSourceCode(QGLShader::Fragment,
            "void main(void)"
            "{"
            "   gl_FragColor = vec4(1.0, 0.0, 1.0, 1.0);"
            "}");
    m_waveprog->link();
}

WaterEngine::~WaterEngine()
{
    glDeleteBuffers(1, &m_vbo);
    delete m_waveprog;
}

void WaterEngine::initializeWaves()
{
    for (int i = 0; i < 4; i++) {
        m_geo_waves[i] = Wave(m_params);
    }
}

void WaterEngine::render(float elapsed_time)
{
    m_waveprog->bind();

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, 0);
    glDrawArrays(GL_QUADS, 0, m_count);
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_waveprog->release();
}


