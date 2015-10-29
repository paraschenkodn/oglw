#include "glwidget.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMouseEvent>

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      clearColour(Qt::black),
      xRot(0),
      yRot(0),
      zRot(0),
      program(0),
      frame_texture(NULL),
      v_aspect(1.0)
{
  hasTexture       = false;
}

GLWidget::~GLWidget()
{
  makeCurrent();

  if(vbo.isCreated()) vbo.destroy();
  if(hasTexture) delete frame_texture;

  delete program;
  doneCurrent();
}

void GLWidget::SetImage(const QImage &img)
{
  makeCurrent();

  if(frame_texture) delete frame_texture;
  frame_texture = new QOpenGLTexture(img);
  hasTexture    = true;
  v_aspect      = (float)(img.height())/img.width();

  makeObject();
  doneCurrent();
  update();
}

QSize GLWidget::minimumSizeHint() const
{
  return(QSize(160, 90));
}

QSize GLWidget::sizeHint() const
{
  return(QSize(800, 450));
}

void GLWidget::rotateBy(int xAngle, int yAngle, int zAngle)
{
  xRot += xAngle;
  yRot += yAngle;
  zRot += zAngle;
  update();
}

void GLWidget::setClearColor(const QColor &color)
{
  clearColour = color;
  update();
}

// Run once when widget is set up
void GLWidget::initializeGL()
{
    /*f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_0>();
    if (!f) {
      qWarning("Could not obtain OpenGL versions object");
      exit(1);
    }
    f->initializeOpenGLFunctions();//*/

  initializeOpenGLFunctions();

  qDebug() << QString((const char*)glGetString(GL_VERSION)) << "\n" << QString((const char*)glGetString(GL_VENDOR))<< "\n" << QString((const char*)glGetString(GL_RENDERER));//<< "\n" << glGetString(GL_EXTENTIONS);

  glEnable(GL_DEPTH_TEST);
  // glEnable(GL_CULL_FACE);

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

  QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
  QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
  program = new QOpenGLShaderProgram;
  const char *vsrc =
      "#version 120\n"
      "attribute highp vec4 vertex;\n"
      "attribute mediump vec4 texCoord;\n"
      "varying mediump vec4 texc;\n"
      "uniform mediump mat4 matrix;\n"
      "void main(void)\n"
      "{\n"
      "    gl_Position = matrix * vertex;\n"
      "    texc = texCoord;\n"
      "}\n";
  vshader->compileSourceCode(vsrc);
  program->addShader(vshader);

  const char *fsrc =
      "#version 120\n"
      "uniform sampler2D texture;\n"
      "varying mediump vec4 texc;\n"
      "void main(void)\n"
      "{\n"
      "    gl_FragColor = texture2D(texture, texc.st);\n"
      "}\n";
  fshader->compileSourceCode(fsrc);
  program->addShader(fshader);

  program->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
  program->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
  program->link();

  program->bind();
  program->setUniformValue("texture", 0);
  //*/

  //TEST ****************************************************
  // Предположим что мы имеем текущий QOpenGLContext и
  // m_shaderProgram экземпляр QOpenGLShaderProgram

  // Создаем VAO для рендеринга первого объекта
  m_vao1 = new QOpenGLVertexArrayObject( this );
  m_vao1->create();
  m_vao1->bind();
  // Установка нескольких VBO и IBO (используем QOpenGLBuffer для хранения данных,
  // указание формата, указание применения и т.д.). Это будет "запомнено"
  // с текущим связанным VAO
  const char *src=
          "#version 120\n"
  "attribute highp vec4 vertexAttr; \n"
  "uniform mediump mat4 viewport;   \n"
  "attribute lowp vec4 colorAttr; \n"
  "varying lowp vec4 color; \n"
  "void main () {\n"
      "gl_Position = viewport * vertexAttr; \n"
      "color = colorAttr; \n"
  "}\n";
  vshader->compileSourceCode(src);
  m_shaderProgram.addShader(vshader);
  src=
          "#version 120\n"
  "varying lowp vec4 color;\n"
  "void main () {\n"
  "gl_FragColor = color;\n"
  "}\n";
  fshader->compileSourceCode(src);
  m_shaderProgram.addShader(fshader);
m_shaderProgram.link();
m_shaderProgram.bind();

  static const float positionData[9] = { -0.5f, -0.5f, 0.0f,
                                         0.5f, -0.5f, 0.0f,
                                         0.0f, 0.5f, 0.0f };
  static const float colorData[9] = { 1.0f, 0.0f, 0.0f,
                                         0.0f, 1.0f, 0.0f,
                                         0.0f, 0.0f, 1.0f };
  vertexCount = 3;
  m_positionBuffer.create();
  m_positionBuffer.setUsagePattern(QOpenGLBuffer::StreamDraw);
  m_positionBuffer.bind();
  m_positionBuffer.allocate(positionData,
                            vertexCount * 3 * sizeof(float));

  m_shaderProgram.enableAttributeArray("vertexAttr");
  m_shaderProgram.setAttributeBuffer  ("vertexAttr", GL_FLOAT, 0, 3);

  m_colorBuffer.create();
  m_colorBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
  m_colorBuffer.bind();
  m_colorBuffer.allocate(colorData,
                         vertexCount * 3 * sizeof(float));

  m_shaderProgram.enableAttributeArray("colorAttr");
  m_shaderProgram.setAttributeBuffer  ("colorAttr", GL_FLOAT, 0, 3);

  // Повторить для буферов нормалей, текстурных координат,
  // касательных, ...

  // Создаем VAO для отображения второго объекта
  // m_vao2 = ....
}

void GLWidget::paintGL()
{
  //glClearColor(clearColour.redF(), clearColour.greenF(), clearColour.blueF(), clearColour.alphaF());
  glClearColor(0.1f,0.1f,0.0f,1.0f); // тёмно-синенький
  // Очищаем буферы
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  QMatrix4x4 m;

  //m.ortho(0.0f, +1.0f, 3.0f/4.0f, 0.0f, 4.0f, 15.0f);
  m.ortho(-2.0f,2.0f,-2.0f,2.0f,8.0f,-8.0f); // glOrtho(left,right,bottom,top,near,far)
  m.translate(0.0f, 0.0f, -5.0f);

  /*m.rotate(xRot / 16.0f, 1.0f, 0.0f, 0.0f);
  m.rotate(yRot / 16.0f, 0.0f, 1.0f, 0.0f);
  m.rotate(zRot / 16.0f, 0.0f, 0.0f, 1.0f);//*/

  program->setUniformValue("matrix", m);
  program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
  program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
  program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
  program->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

  if(hasTexture)
  {
    frame_texture->bind();
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  }
//*/

  /// TEST **************************************************
  // Связываем шейдер для первого набор объектов
  m_shaderProgram.bind();

  m_shaderProgram.setUniformValue("viewport", m);

  // Переключаемся на данные о вершинах первого объекта и рисуем его
  int num_vertices = 3; // три вершины
  m_vao1->bind();
  glDrawElements(GL_TRIANGLES, num_vertices, GL_UNSIGNED_INT, BUFFER_OFFSET(0));   //The starting point of the IBO

  // Переключаемся на данные о вершинах второго объекта и рисуем его
  //m_vao2->bind();
  //m_funcs->glDrawElements(...);

  // Мб поменять шейдер и/или текстуру и т.д.
  // И отрисовать остальные объекты
  //m_skyboxShaderProgram->bind();
  //...
  //m_skyboxVAO->bind();
  //m_funcs->glDrawElements(...);
  //...
}

void GLWidget::resizeGL(int width, int height)
{
  glViewport(0, 0, width, height);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
  lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
  int dx = event->x() - lastPos.x();
  int dy = event->y() - lastPos.y();

  if (event->buttons() & Qt::LeftButton) {
      rotateBy(8 * dy, 8 * dx, 0);
  } else if (event->buttons() & Qt::RightButton) {
      rotateBy(8 * dy, 0, 8 * dx);
  }
  lastPos = event->pos();
}

void GLWidget::mouseReleaseEvent(QMouseEvent * /* event */)
{
  emit clicked();
}

void GLWidget::makeObject()
{
  static const float coords[4][3] = {
    { 1.0f, v_aspect, 0.0f },
    { 0.0f, v_aspect, 0.0f },
    { 0.0f, 0.0f,     0.0f },
    { 1.0f, 0.0f,     0.0f }
  };

  QVector<GLfloat> vertData;
  for(int j = 0; j < 4; ++j)
  {
    // vertex position
    vertData.append(1.0 * coords[j][0]);
    vertData.append(1.0 * coords[j][1]);
    vertData.append(1.0 * coords[j][2]);

    // texture coordinate
    vertData.append(j == 0 || j == 3);
    vertData.append(j == 0 || j == 1);
  }

  vbo.create();
  vbo.bind();
  vbo.allocate(vertData.constData(), vertData.count() * sizeof(GLfloat));
}
