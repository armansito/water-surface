#include "glwidget.h"
#include "camera.h"
#include "waterengine.h"

GLWidget::GLWidget(QWidget *parent) : QGLWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));

    m_camera = new Camera(45.f, 1.f, 0.1f, 1000.f);
    m_camera->setZoom(30.f);
    m_engine = NULL;
}

GLWidget::~GLWidget()
{
    delete m_camera;
    delete m_engine;
}

void GLWidget::initializeGL()
{
    m_engine = new WaterEngine();

    m_time.start();
    m_timer.start(1000/60);
}

static float elapsed = 0.f;
void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_camera->loadModelviewMatrix();

    m_engine->render(elapsed);
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    m_camera->setAspect((float)w/(float)h);
}

void GLWidget::tick()
{
    float seconds = m_time.restart() * 0.001f;
    elapsed += seconds;

    updateGL();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_mousep = Vector2(event->x(), event->y());
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    Vector2 pos(event->x(), event->y());
    Vector2 delta = pos - m_mousep;
    delta *= 5e-3f;
    m_mousep = pos;
    m_camera->rotate(delta.x, delta.y);
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    if (event->orientation() == Qt::Vertical) m_camera->zoom(((float)event->delta())/300.f);
}
