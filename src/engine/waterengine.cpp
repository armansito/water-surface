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
    params.speed = p.speed;
    params.kAmpOverLen = p.kAmpOverLen;
    params.wave_dir = dir;
}

WaterEngine::WaterEngine()
{
    // initialize parameters
    m_params.wavelength = 30.f;
    m_params.steepness = 0.8f;
    m_params.kAmpOverLen = 0.2f;
    m_params.speed = 1.f;
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
            "struct Wave"
            "{"
            "   float wavelength;"
            "   float steepness;"
            "   float speed;"
            "   float kAmpOverLen;"
            "   vec2 direction;"
            "};"

            "void wave_function(in Wave waves[4], in float time, in vec3 pos,"
            "                   out vec3 P, out vec3 N, out vec3 B, out vec3 T)"
            "{"
            "   float PI = 3.14159265358979323846264;"
            "   P = pos;"
            "   for (int i = 0; i < 4; i++) {"
            "       float A = 16.0 * 0.04;"//waves[i].wavelength * waves[i].kAmpOverLen;"  // Amplitude
            "       float omega = 2.0 * PI / 16.0;"//waves[i].wavelength;"          // Frequency
            "       float phi = 4.0 * omega;"//waves[i].speed * omega;"                    // Phase
            "       float Qi = 0.5/(omega * A * 4.0);"//waves[i].steepness/(omega * A * 4.0);"

            "       float term = omega * dot(normalize(vec2(1.0, 0.5)), vec2(pos.x, pos.z)) + phi * time;"
            "       float C = cos(term);"
            "       float S = sin(term);"
            "       P += vec3(Qi * A * 1.0 * C,"
            "                 A * S,"
            "                 Qi * A * 0.5 * C);"
            "   }"
            "}"

            "uniform Wave waves[4];"
            "uniform float time;"

            "void main(void)"
            "{"
            "   vec3 P, N, B, T;"
            "   wave_function(waves, time, gl_Vertex.xyz, P, N, B, T);"
            "   gl_Position = gl_ModelViewProjectionMatrix * vec4(P.xyz, 1.0);"
            "}");

    m_waveprog->addShaderFromSourceCode(QGLShader::Fragment,
            "void main(void)"
            "{"
            "   gl_FragColor = vec4(1.0);"
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
    m_waveprog->setUniformValue("time", elapsed_time);
    m_waveprog->setUniformValueArray("waves", (const GLfloat *)m_geo_waves, 4, sizeof(Wave)/sizeof(float));

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, 0);
    glDrawArrays(GL_QUADS, 0, m_count);
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_waveprog->release();
}


