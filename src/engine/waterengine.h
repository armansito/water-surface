#ifndef WATERENGINE_H
#define WATERENGINE_H

#include <qgl.h>
#include <QGLShaderProgram>

#include "vector.h"

struct WaveParameters
{
    float wavelength;
    float steepness;
    float speed;
    float kAmpOverLen;
    Vector2 wave_dir; 
};

class WaterEngine
{
public:
    WaterEngine();
    ~WaterEngine();

    inline const WaveParameters &parameters() const { return m_params; }
    inline void setParameters(const WaveParameters &params) { m_params = params; initializeWaves(); }

    void render(float elapsed_time);

private:
    struct Wave
    {
        Wave();
        Wave(const WaveParameters &params);
        WaveParameters params;
    };

    void initializeWaves();

    Wave m_geo_waves[4]; // geometric waves

    WaveParameters m_params;
    unsigned int m_count;
    GLuint m_vbo;
    QGLShaderProgram *m_waveprog;
};

#endif // WATERENGINE_H
