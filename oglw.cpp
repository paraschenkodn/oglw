/*  http://doc.qt.io/qt-5/qtgui-openglwindow-example.html
 *
 */

#include "oglw.h"
#include <QDebug>

/// шейдеры для рисования (определяем статически, можно конечно же потом грузить из файла)
static const char *vertexShaderSource =
        "#version 330 core\n"
        "uniform mat4 projectionMatrix;\n"
        "in vec3 position;\n"
        "in vec3 color;\n"
        "out vec3 fragmentColor;\n"
        "void main(void) {\n"
                // перевод вершинных координат в однородные
                "gl_Position   = projectionMatrix * vec4(position, 1.0);\n"
                // передаем цвет вершины в фрагментный шейдер
                "fragmentColor = color; }\n";//*/

static const char *fragmentShaderSource =
        "#version 330 core\n"
        "in vec3 fragmentColor;\n"
        "out vec4 color;\n"
        "void main(void) {\n"
                // зададим цвет пикселя
                "color = vec4(fragmentColor, 1.0); }\n";//*/

OGLW::OGLW(QWindow *parent) // создаём окно вызывая конструктор с инициализацией переменных.
  : QWindow(parent)
  , m_update_pending(false)       //
  , m_animating(false)            //
  , m_context(0)                  //
  , m_device(0)                   //
  , needsInitialize(false)        // состояние по умолчанию, когда поребуется инициализация точек входа функций OpenGL мы флаг взведём
{
  setSurfaceType(QWindow::OpenGLSurface); /// Необходимо указать Qt что мы для рисования в окне будем использовать OpenGL,
                                          /// а не использовать растровый контент отображаемый QPainter используя QBackingStore.

  // Специфицируем формат и создаем платформо-зависимый сюрфейс
  QSurfaceFormat format;
  format.setDepthBufferSize(24);
  format.setMajorVersion(3);
  format.setMinorVersion(0);
  format.setSamples(4);
  format.setProfile(QSurfaceFormat::NoProfile);
  setFormat(format);
  create();

  // Создаем OpenGL контекст
  m_context = new QOpenGLContext;
  m_context->setFormat(format);
  m_context->create();
  needsInitialize = true;

  // Сделаем контекст текущим для этого окна
  m_context->makeCurrent(this);
  // Получить объект функции и назначить все точки входа
  // m_funcs объявлен как: QOpenGLFunctions_4_3_Core * m_funcs
  m_funcs = (QOpenGLFunctions_3_0*)m_context->versionFunctions();

  if (!m_funcs) {
    qWarning("Could not obtain OpenGL versions object");
    exit(1);
  }

  m_funcs->initializeOpenGLFunctions();

  /*uint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);//*/

  qDebug() << QString((const char*)glGetString(GL_VERSION)) << "\n" << QString((const char*)glGetString(GL_VENDOR))<< "\n" << QString((const char*)glGetString(GL_RENDERER));//<< "\n" << glGetString(GL_EXTENTIONS);

}

OGLW::~OGLW()
{
    delete m_device;
}

/// Если окно должно быть перерисовано, вызываем функцию рисования renderNow()
void OGLW::exposeEvent(QExposeEvent *event)
{
    Q_UNUSED(event);

    if (isExposed())        // проверяем статус окна и вызываем прорисовку при необходимости
        renderNow();
}
/// функции обработки сигналов через которые будет происходить прорисовка экрана следующие: renderNow() и renderLater()
/// renderNow()будет вызываться всякий раз когда поступит событие обновления эрана и заданное нами событие следующей прорисовки
/// которое устанавливается в renderLater() для задания эффекта анимации

void OGLW::renderNow()  // основная функция рисования с проверками инициализации OpenGL
{
    if (!isExposed())                               // Если окно не отображается или его часть, то нам тут делать нечего.
        return;

    if (needsInitialize) {                  //
        initialize();                   // вызываем функцию инициализации
        needsInitialize=false;          // убираем флаг инициализации
    }

    render();                           // добрались до прорисовки сцены (там мы будем рисовать в буфере?)

    m_context->makeCurrent(this);
    m_context->swapBuffers(this);       // выкидываем на экран из буфера то, что нарисовали

    if (m_animating)
        renderLater();
}

// Если функция не переопределена, то вызовется рисовальщик по умолчанию render(QPainter *) с соответствующей инициализацией
/*void OGLW::render()
{
    if (!m_device)
        m_device = new QOpenGLPaintDevice;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    m_device->setSize(size());

    QPainter painter(m_device);
    render(&painter);
}
//*/

// любая инициализация OpenGL , как и функция прорисовки может быть перекрыта в дочернем окне, поэтому эти виртуальные функции пустые,
// функция инициализации вызывается один раз перед вызовом прорисовки сцены,
// но при этом должен быть обеспечен правильный контекст, и проинициализировано устройство
// что обеспечивает по умолчанию функция render(), инициализирует устройство вывода и передаёт его указатель в render(QPainter *painter)

/// прорисовку анимации организовываем через очередь сообщений организованой посредством вызовов renderLater() и event(QEvent *)
void OGLW::renderLater()
{
    if (!m_update_pending) {
        m_update_pending = true;
        QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
    }
}

bool OGLW::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::UpdateRequest:
        m_update_pending = false;
        renderNow();
        return true;
    default:
        return QWindow::event(event);
    }
}

/// установка флага анимации задействует очередь сообщений окна посредством renderLater()
void OGLW::setAnimating(bool animating)
{
    m_animating = animating;

    if (animating)
        renderLater();
}

GLuint OGLW::loadShader(GLenum type, const char *source)
{
    GLuint shader = m_funcs->glCreateShader(type);
    m_funcs->glShaderSource(shader, 1, &source, 0);
    m_funcs->glCompileShader(shader);
    return shader;
}

void OGLW::initialize()
{
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();
    m_posAttr = m_program->attributeLocation("posAttr");
    m_colAttr = m_program->attributeLocation("colAttr");
    m_matrixUniform = m_program->uniformLocation("matrix");

    vao.create();
    vao.bind();
}

void OGLW::render()
{
    const qreal retinaScale = devicePixelRatio();
    m_funcs->glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    m_funcs->glClear(GL_COLOR_BUFFER_BIT);

    m_program->bind();

    QMatrix4x4 matrix;
    matrix.perspective(60.0f, 4.0f/3.0f, 0.1f, 100.0f);
    matrix.translate(0, 0, -2);
    matrix.rotate(100.0f * m_frame / screen()->refreshRate(), 0, 1, 0);

    m_program->setUniformValue(m_matrixUniform, matrix);

    GLfloat vertices[] = {
        0.0f, 0.707f,
        -0.5f, -0.5f,
        0.5f, -0.5f
    };

    GLfloat colors[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };

    m_funcs->glVertexAttribPointer(m_posAttr, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    m_funcs->glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, colors);

    m_funcs->glEnableVertexAttribArray(0);
    m_funcs->glEnableVertexAttribArray(1);

    m_funcs->glDrawArrays(GL_TRIANGLES, 0, 3);

    m_funcs->glDisableVertexAttribArray(1);
    m_funcs->glDisableVertexAttribArray(0);

    m_program->release();

    ++m_frame;
}
