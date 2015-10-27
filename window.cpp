#include <QtWidgets>

#include "window.h"
#include "glwidget.h"

Window::Window()
{
  QVBoxLayout *mainLayout = new QVBoxLayout;

  view  = new QGraphicsView(this);
  scene = new QGraphicsScene(view);

  view->setRenderHints(QPainter::Antialiasing);
  scene->setBackgroundBrush(Qt::blue);

  myGLWidget = new GLWidget(this);
  QOpenGLWidget *oglw = qobject_cast<QOpenGLWidget *>(myGLWidget);

  QSurfaceFormat format;
  format.setSamples(4);
  format.setDepthBufferSize(24);
  //format.setMajorVersion(3);
  //format.setMinorVersion(0);
  //format.setVersion(3,0);
  //format.setProfile(QSurfaceFormat::NoProfile);
  oglw->setFormat(format);

  view->setViewport(myGLWidget);
  view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

  view->setScene(scene);

  mainLayout->addWidget(view);
  setLayout(mainLayout);

  setWindowTitle(tr("GraphicsView + OpenGL Test Unit"));
}

void Window::SetTestImage()
{
  QImage input_image(QString(":/my-test-image.jpg"));
  if (input_image.isNull())
     {
      qWarning() << "Картинка не загрузилась, это фатально.";
      QApplication::closeAllWindows();
      QApplication::exit(1);
  }
  GLWidget *glw = qobject_cast<GLWidget *>(myGLWidget);
  glw->SetImage(input_image);
}
