#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QTime>
#include <QTimer>
#include <QMouseEvent>
#include <QWheelEvent>
#include "vector.h"

class Camera;
class WaterEngine;

class GLWidget : public QGLWidget
{
Q_OBJECT
public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();

private:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

    QTime m_time;
    QTimer m_timer;
    Vector2 m_mousep;
    Camera *m_camera;
    WaterEngine *m_engine;

private slots:
    void tick();
};

#endif // GLWIDGET_H

