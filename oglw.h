/*  http://doc.qt.io/qt-5/qtgui-openglwindow-example.html
 *
 */

#ifndef OGLW_H
#define OGLW_H

#include <QWindow>
#include <QOpenGLPaintDevice>
#include <QSurfaceFormat>
#include <QCoreApplication> //*/

//#include <QtGui/QOpenGLFunctions>
#include <QOpenGLFunctions_3_0>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLFunctions_4_3_Core>
#include <QPainter>

#include <QOpenGLShaderProgram>
#include <QScreen>

#include <QOpenGLVertexArrayObject>

class OGLW:public QWindow ///, protected QOpenGLFunctions
{
  Q_OBJECT
public:
  explicit OGLW(QWindow *parent = 0);  // (explicit???)
  ~OGLW();

  void render(QPainter *painter); // прорисовщик вызывающийся по умолчанию
  void render()Q_DECL_OVERRIDE;           // наша функция рисования, которую мы переопределяем в дочерних окнах

  void initialize()Q_DECL_OVERRIDE;       // инициализация OpenGL (настраиваем общие параметры), которую мы переопределим в дочерних окнах

  void setAnimating(bool animating);      // ???

  QOpenGLFunctions_3_0 * m_funcs;    /// объявляем точку для обращения к функциям конкретной версии OpenGL (мы ведь хотим пользоваться последней?)
                                          /// Если бы нам требовалась совместимость мы могли бы просто использовать ... ", protected QOpenGLFunctions"  в объявлении
                                          /// класса и не объявлять данную переменную.
public slots:
  void renderLater();     //
  void renderNow();       //

protected:
  bool event(QEvent *event) Q_DECL_OVERRIDE;                  //

  void exposeEvent(QExposeEvent *event) Q_DECL_OVERRIDE;      //

private:
  bool m_update_pending;          // флаг необходимости обновления сцены (прорисовки буфера)
  bool m_animating;               // флаг запуска анимации
  bool needsInitialize;           // устанавливаем флаг инициализации точек входа функций OpenGL

  QOpenGLContext *m_context;              //
  QOpenGLPaintDevice *m_device;           //

  QOpenGLVertexArrayObject vao;

  /// рисование
  GLuint loadShader(GLenum type, const char *source);

  GLuint m_posAttr;
  GLuint m_colAttr;
  GLuint m_matrixUniform;

  QOpenGLShaderProgram *m_program;
  int m_frame;

};

#endif // OGLW_H
