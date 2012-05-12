#include "camera.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif


Camera::Camera(float fovy, float aspect, float near, float far) : m_fovy(fovy), m_aspect(aspect), m_near(near), m_far(far)
{
    setProjectionMode(false);
    m_hangle = 0.f;
    m_vangle = 0.f;
    m_look = Vector3::fromAngles(m_hangle-M_PI_2, -m_vangle);
    m_zoom = 5.f; 
}

void Camera::perspective(float fovy, float aspect, float near, float far)
{
    m_fovy = fovy;
    m_aspect = aspect;
    m_near = near;
    m_far = far;
    loadPerspectiveMatrix();
}

void Camera::loadPerspectiveMatrix()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    multiplyPerspectiveMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void Camera::loadModelviewMatrix()
{
    glLoadIdentity();
    multiplyModelviewMatrix();
}

void Camera::loadOrthographic()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    multiplyOrthographic();
    glMatrixMode(GL_MODELVIEW);
}

void Camera::multiplyPerspectiveMatrix()
{
    gluPerspective(m_fovy, m_aspect, m_near, m_far);
}

void Camera::multiplyModelviewMatrix()
{
    glTranslatef(0.f, 0.f, -m_zoom);
    glRotatef(m_vangle * 180.f/M_PI, 1.f, 0.f, 0.f);
    glRotatef(m_hangle * 180.f/M_PI, 0.f, 1.f, 0.f);
    glTranslatef(-m_translate.x, -m_translate.y, -m_translate.z);
}

void Camera::multiplyOrthographic()
{
    glOrtho(-m_aspect, m_aspect, -1.f, 1.f, -1.f, 1.f);
}

void Camera::setProjectionMode(bool ortho)
{
    if (ortho) m_projfunc = &Camera::loadOrthographic;
    else m_projfunc = &Camera::loadPerspectiveMatrix;
    (this->*m_projfunc)();
}

void Camera::move(const Vector3 &v)
{
    m_translate += v;
}

void Camera::rotate(float hangle, float vangle)
{
    m_hangle += hangle; 
    m_vangle += vangle;
    m_vangle = fmin(M_PI_2 - 1e-2, fmax(-M_PI_2 + 1e-2, m_vangle)); 
    m_look = Vector3::fromAngles(m_hangle-M_PI_2, -m_vangle);
}

void Camera::zoom(float zoomf)
{
    m_zoom += zoomf;
}
