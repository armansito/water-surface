#ifndef CAMERA_H
#define CAMERA_H

#include "vector.h"

class Camera
{
public:
    
    Camera(float fovy, float aspect, float near, float far);

    void perspective(float fovy, float aspect, float near, float far);

    void loadPerspectiveMatrix();
    void loadModelviewMatrix();
    void loadOrthographic();

    void multiplyPerspectiveMatrix();
    void multiplyModelviewMatrix();
    void multiplyOrthographic();

    void setProjectionMode(bool ortho);

    inline float zoomValue() const { return m_zoom; }
    inline const Vector3 &look() const { return m_look; }
    inline const Vector3 &center() const { return m_translate; }
    inline float horizontalAngle() const { return m_hangle; }
    inline float verticalAngle() const { return m_vangle; }
    inline float fovy() const { return m_fovy; }
    inline float aspectRatio() const { return m_aspect; }
    inline float near() const { return m_near; }
    inline float far() const { return m_far; }

    inline void setCenter(const Vector3 &center) { m_translate = center; }
    inline void setZoom(float z) { m_zoom = fmin(15.f, fmax(0.5f, z)); }
    inline void setAngles(float hangle, float vangle) { m_hangle = hangle; m_vangle = vangle; }
    inline void setFovy(float fovy) { m_fovy = fovy; (this->*m_projfunc)(); }
    inline void setAspect(float aspect) { m_aspect = aspect; (this->*m_projfunc)(); }
    inline void setNear(float near) { m_near = near; (this->*m_projfunc)(); }
    inline void setFar(float far) { m_far = far; (this->*m_projfunc)(); }

    void move(const Vector3 &v);
    void rotate(float hangle, float vangle);
    void zoom(float zoomf);

private:

    float m_hangle, m_vangle, m_zoom;
    Vector3 m_translate, m_look;

    float m_fovy, m_aspect, m_near, m_far;

    void (Camera::*m_projfunc)(void);
};

#endif // CAMERA_H
