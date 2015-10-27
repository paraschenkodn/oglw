#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QOpenGLFunctions_3_0>

#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

#include <QOpenGLVertexArrayObject>

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram);
QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
  Q_OBJECT

public:
  explicit GLWidget(QWidget *parent = 0);  // explicit отключает неявные преобразования аргументов при инициализации класса
  ~GLWidget();

  void SetImage(const QImage &img);
  void setClearColor(const QColor &color);

  QSize minimumSizeHint() const Q_DECL_OVERRIDE;
  QSize sizeHint() const Q_DECL_OVERRIDE;
  void rotateBy(int xAngle, int yAngle, int zAngle);

signals:
  void clicked();

protected:
  void initializeGL() Q_DECL_OVERRIDE;
  void paintGL() Q_DECL_OVERRIDE;
  void resizeGL(int width, int height) Q_DECL_OVERRIDE;

  void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
  void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
  void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
  void makeObject();
  void setView(int w, int h);

  QOpenGLFunctions_3_0 *f;

  QColor clearColour;

  int xRot;
  int yRot;
  int zRot;
  QPoint lastPos;

  QOpenGLShaderProgram *program, m_shaderProgram;
  QOpenGLBuffer vbo;

  QOpenGLTexture *frame_texture;
  bool hasTexture;
  float v_aspect;

  ///TEST*******************
  QOpenGLVertexArrayObject * m_vao1;
  QOpenGLBuffer m_positionBuffer;
  QOpenGLBuffer m_colorBuffer;
  int vertexCount;
  #define BUFFER_OFFSET(i) ((char *)NULL + (i))

};

#endif
