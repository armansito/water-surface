#ifndef WATERENGINE_H
#define WATERENGINE_H

#include <qgl.h>
#include <QGLShaderProgram>

struct WaveParameters
{
    float kWavelength;
    float kSteepness;
    float kAmpOverLen;
};

class WaterEngine
{
public:
    WaterEngine();
    ~WaterEngine();

    inline WaveParameters &parameters() { return m_params; }

    void render(float elapsed_time);

private:
    WaveParameters m_params;
    unsigned int m_count;
    GLuint m_vbo;
};

#endif // WATERENGINE_H
