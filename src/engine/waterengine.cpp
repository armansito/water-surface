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

#define DIM 100
#define UNIT 2.f

WaterEngine::Wave::Wave() {}
WaterEngine::Wave::Wave(const WaveParameters &p)
{
    float wl = p.wavelength;
    wl = frand() * (2.f * wl - 0.7f * wl) + 0.7f * wl;

    static float angle = frand() * M_PI - M_PI_2;
    float c = cos(angle);
    float s = sin(angle);
    Vector2 dir(c * p.wave_dir.x - s * p.wave_dir.y,
                s * p.wave_dir.x + c * p.wave_dir.x);

    float st = p.steepness;

    params.wavelength = wl;
    params.steepness = st;
    params.speed = p.speed;
    params.kAmpOverLen = p.kAmpOverLen;
    params.wave_dir = dir;
}

WaterEngine::WaterEngine()
{
    // initialize parameters
    m_params.wavelength = 16.f;
    m_params.steepness = 1.f;
    m_params.kAmpOverLen = 0.05f;
    m_params.speed = 4.f;
    m_params.wave_dir = Vector2(1.f, 0.5f).unit();

    // initialize waves
    initializeWaves();

    // build vbo with base mesh
    unsigned int size = (DIM)*(DIM)*4;
    Vector3 *vertices = new Vector3[size];
    Vector3 *v = &vertices[0];
    float x, z;
    z = -DIM/2.f;
    for (int i = 0; i < DIM/UNIT; i++) {
        x = -DIM/2.f;
        for (int j = 0; j < DIM/UNIT; j++) {
            v->x = x;
            v->y = 0.f;
            v->z = z;
            v++;
            v->x = x;
            v->y = 0.f;
            v->z = z + UNIT;
            v++;
            v->x = x + UNIT;
            v->y = 0.f;
            v->z = z + UNIT;
            v++;
            v->x = x + UNIT;
            v->y = 0.f;
            v->z = z;
            v++;
            x += UNIT;
        }
        z += UNIT;
    }

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, size * sizeof(Vector3), (GLvoid *)vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    m_count = size;
    delete[] vertices;

    m_waveprog = new QGLShaderProgram();
    m_waveprog->addShaderFromSourceCode(QGLShader::Vertex,

            "void wave_function(in float waves[24], in float time, in vec3 pos,"
            "                   out vec3 P, out vec3 N, out vec3 B, out vec3 T)"
            "{"
            "   float PI = 3.14159265358979323846264;"
            "   P = pos;"
            "   for (int i = 0; i < 24; i += 6) {"
            "       float A = waves[i] * waves[i+3];"         // Amplitude
            "       float omega = 2.0 * PI / waves[i];"       // Frequency
            "       float phi = waves[i+2] * omega;"          // Phase
            "       float Qi = waves[i+1]/(omega * A * 4.0);" // Steepness

            "       float term = omega * dot(vec2(waves[i+4], waves[i+5]), vec2(pos.x, pos.z)) + phi * time;"
            "       float C = cos(term);"
            "       float S = sin(term);"
            "       P += vec3(Qi * A * waves[i+4] * C,"
            "                 A * S,"
            "                 Qi * A * waves[i+5] * C);"
            "   }"
            "}"
            
            "uniform float waves[24];"
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
        m_geo_waves[i] = m_params;
    }
}

void WaterEngine::render(float elapsed_time)
{
    m_waveprog->bind();
    m_waveprog->setUniformValue("time", elapsed_time);
    m_waveprog->setUniformValueArray("waves", (const GLfloat *)m_geo_waves, 4 * sizeof(Wave)/sizeof(float), 1);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, 0);
    glDrawArrays(GL_QUADS, 0, m_count);
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_waveprog->release();
}


