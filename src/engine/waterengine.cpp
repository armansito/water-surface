#include "waterengine.h"
#include <iostream>
#include <stdlib.h>

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
#define UNIT 1.f
#define TEXSIZE 256 
#define GW GEOMETRIC_WAVES
#define NMW NORMALMAP_WAVES

WaterEngine::Wave::Wave() {}
WaterEngine::Wave::Wave(const WaveParameters &p)
{
    float wl = p.wavelength;
    wl = frand() * (2.f * wl - 0.7f * wl) + 0.7f * wl;

    static float angle = frandf() * M_PI_4;// - M_PI_2;
    float c = cos(angle);
    float s = sin(angle);
    Vector2 dir(c * p.wave_dir.x,
                s * p.wave_dir.y);
    dir = Vector2::fromAngle(frandf() * M_PI_4); 
    float st = p.steepness;

    params.wavelength = wl;
    params.steepness = st;
    params.speed = sqrt(9.81f * 2.f*M_PI/wl)*wl*p.speed; 
    params.kAmpOverLen = p.kAmpOverLen;
    params.wave_dir = dir.unit();
}

WaterEngine::WaterEngine()
{
    // cornflower blue
    glClearColor(0.39f, 0.58f, 0.93f, 1.f);

    // enable fog
    glFogi(GL_FOG_MODE, GL_EXP2);
    GLfloat fogColor[] = { 0.39f, 0.58f, 0.93f, 1.f };
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_DENSITY, 0.015f);
    glHint(GL_FOG_HINT, GL_DONT_CARE);
    glFogf(GL_FOG_START, 200.f);
    glFogf(GL_FOG_END, 1000.f);
    glEnable(GL_FOG);
    
    // enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // enable back face culling
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    // seed random
    srand(time(0));

    // initialize parameters
    m_params.wavelength = 10.f;
    m_params.steepness = 0.8f;
    m_params.kAmpOverLen = 0.03f;
    m_params.speed = 0.06f;
    m_params.wave_dir = Vector2(1.f, 0.8f).unit();

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

    // setup the framebuffer for normal map generation
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &m_normalmap);
    glBindTexture(GL_TEXTURE_2D, m_normalmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXSIZE, TEXSIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &m_nmfbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_nmfbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_normalmap, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "error: Problem creating framebuffer" << std::endl;
        exit(0); 
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // shader program that generates the normal map
    m_nmprog = new QGLShaderProgram();
    m_nmprog->addShaderFromSourceCode(QGLShader::Fragment,
            "uniform float time;"
            "uniform float waves[300];"
            
            "void calc_normal(in vec2 uv, out vec3 N)"
            "{"
            "   float PI = 3.14159265358979323846264;"
            "   N = vec3(0.0, 0.0, 1.0);"
            "   for (int i = 0; i < 300; i += 6) {"
            "       float A = waves[i] * waves[i+3];"         // Amplitude
            "       float omega = 2.0 * PI / waves[i];"       // Frequency
            "       float phi = waves[i+2] * omega;"          // Phase
            "       float k = waves[i+1];"
            "       float term = omega * dot(vec2(waves[i+4], waves[i+5]), uv) + phi * time;"
            "       float C = cos(term);"
            "       float S = sin(term);"
            "       float val = pow(0.5 * (S + 1.0), k - 1.0) * C;"
            "       val = omega * A * k * val;"
            "       N += vec3(waves[i+4] * val,"
            "                 waves[i+5] * val,"
            "                 0.0);"
            "   }"
            "   N = normalize(N);" 
            "}"

            "void main(void)"
            "{"
            "   vec3 N;"
            "   calc_normal(gl_FragCoord.st/128.0, N);"
            "   N = (N * 0.5) + 0.5;"
            "   gl_FragColor = vec4(N.xyz, 1.0);"
            "}");

    m_nmprog->link();

    // shader program that produces the final render
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
            "   B = vec3(0.0);"
            "   T = vec3(0.0);"
            "   N = vec3(0.0);"
            "   for (int i = 0; i < 24; i += 6) {"
            "       float A = waves[i] * waves[i+3];"         // Amplitude
            "       float omega = 2.0 * PI / waves[i];"       // Frequency
            "       float phi = waves[i+2] * omega;"          // Phase
            "       float Qi = waves[i+1]/(omega * A * 4.0);" // Steepness

            "       float WA = omega * A;"
            "       float term = omega * dot(vec2(waves[i+4], waves[i+5]), vec2(P.x, P.z)) + phi * time;"
            "       float C = cos(term);"
            "       float S = sin(term);"
            "       B += vec3 (Qi * waves[i+4]*waves[i+4] * WA * S,"
            "                  Qi * waves[i+4] * waves[i+5] * WA * S,"
            "                  waves[i+4] * WA * C);"

            "       T += vec3 (Qi * waves[i+4] * waves[i+5] * WA * S,"
            "                  Qi * waves[i+5] * waves[i+5] * WA * S,"
            "                  waves[i+5] * WA * C);"

            "       N += vec3 (waves[i+4] * WA * C,"
            "                  waves[i+5] * WA * C,"
            "                  Qi * WA * S);"
            "   }"
            "   B = normalize(vec3(1.0 - B.x, -B.y, B.z));"
            "   T = normalize(vec3(-T.x, 1.0 - T.y, T.z));"
            "   N = normalize(vec3(-N.x, -N.y, 1.0 - N.z));"
            "}"
            
            "uniform float waves[24];"
            "uniform float time;"
            "uniform vec3 light;"

            "varying vec2 texcoord;"
            "varying vec3 lightv;"
            "varying vec3 viewv;"
            "void main(void)"
            "{"
            "   vec3 P, N, B, T;"
            "   wave_function(waves, time, gl_Vertex.xyz, P, N, B, T);"
            "   lightv = vec3(dot(light, B),"
            "                 dot(light, T),"
            "                 dot(light, N));"
            "   lightv = normalize(lightv);"
            "   vec3 pos = (gl_ModelViewMatrix * vec4(P.xyz, 1.0)).xyz;"
            "   viewv = vec3(dot(pos, B),"
            "                dot(pos, T),"
            "                dot(pos, N));"
            "   viewv = normalize(viewv);"
            "   texcoord = vec2(P.x, P.z) * 0.5 + 0.5;"
            "   gl_Position = gl_ProjectionMatrix * vec4(pos, 1.0);"
            "}");

    m_waveprog->addShaderFromSourceCode(QGLShader::Fragment,
            "uniform sampler2D normalmap;"

            "varying vec2 texcoord;"
            "varying vec3 lightv;"
            "varying vec3 viewv;"
            "void main(void)"
            "{"
            "   vec3 N = texture2D(normalmap, texcoord*0.125).xyz * 2.0 - 1.0;"
            "   N = normalize(N);"
            "   vec3 ambient = vec3(0.39, 0.43, 0.93);"
            "   vec3 diffuse = vec3(0.0, 0.2, 1.0) * clamp(dot(N, lightv), 0.0, 1.0);"
            "   vec3 specular = vec3(1.0) * pow(clamp(dot(reflect(lightv, N), viewv), 0.0, 1.0), 50.0);"
            "   gl_FragColor = vec4(0.2 * ambient + 0.4* diffuse + specular, 1.0);"
            "}");
    m_waveprog->link();
}

WaterEngine::~WaterEngine()
{
    glDeleteBuffers(1, &m_vbo);
    glDeleteFramebuffers(1, &m_nmfbo);
    glDeleteTextures(1, &m_normalmap);
    delete m_waveprog;
    delete m_nmprog;
}

void WaterEngine::initializeWaves()
{
    // initialize geometric waves
    for (int i = 0; i < GW; i++) {
        m_geo_waves[i] = m_params;
    }

    // initialize normal map waves
    for (int i = 0; i < NMW; i++) {
        float wl = m_nm_waves[i].params.wavelength = (frandf() * 0.5f + 0.3f);
        m_nm_waves[i].params.wave_dir = (Vector2::randomDirection()*3.f).floor() * wl;
        m_nm_waves[i].params.steepness = 5.f*(frandf() * 2.f + 1.f);
        m_nm_waves[i].params.speed = 0.05f * sqrt(M_PI/wl);
        m_nm_waves[i].params.kAmpOverLen = 0.02f;
    }
}

void WaterEngine::render(float elapsed_time)
{
    // store current viewport and projection matrix
    int vp[4];
    float proj[16];
    glGetIntegerv(GL_VIEWPORT, vp);
    glGetFloatv(GL_PROJECTION_MATRIX, proj);

    // set projection matrix to identity, set viewport to
    // normal map dimensions
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, TEXSIZE, TEXSIZE);
   
    // render normal map
    m_nmprog->bind();
    m_nmprog->setUniformValue("time", elapsed_time);
    m_nmprog->setUniformValueArray("waves", (const GLfloat *)m_nm_waves, NMW * sizeof(Wave)/sizeof(float), 1);

    glBindFramebuffer(GL_FRAMEBUFFER, m_nmfbo);
    glClear(GL_COLOR_BUFFER_BIT);
    glPushMatrix();
    glLoadIdentity();
    glBegin(GL_QUADS);
    glColor3f(1.f, 0.f, 0.f); glVertex3f(-1.f, -1.f, -1.f);
    glColor3f(1.f, 1.f, 0.f); glVertex3f( 1.f, -1.f, -1.f);
    glColor3f(0.f, 1.f, 0.f); glVertex3f( 1.f,  1.f, -1.f);
    glColor3f(0.f, 0.f, 1.f); glVertex3f(-1.f,  1.f, -1.f);
    glEnd();
    glPopMatrix();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    m_nmprog->release();

    // restore viewport and projection
    glViewport(vp[0], vp[1], vp[2], vp[3]);
    glMatrixMode(GL_PROJECTION);
    glMultMatrixf(proj);
    glMatrixMode(GL_MODELVIEW);

    /* render waves */
    glColor3f(1.f, 1.f, 1.f);
    glBindTexture(GL_TEXTURE_2D, m_normalmap);
    m_waveprog->bind();
    m_waveprog->setUniformValue("time", elapsed_time);
    m_waveprog->setUniformValueArray("waves", (const GLfloat *)m_geo_waves, GW * sizeof(Wave)/sizeof(float), 1);
    m_waveprog->setUniformValue("light", 0.f, 100.f, 0.f);
    m_waveprog->setUniformValue("normalmap", 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, 0);
    glDrawArrays(GL_QUADS, 0, m_count);
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_waveprog->release();
    glBindTexture(GL_TEXTURE_2D, 0);
}


