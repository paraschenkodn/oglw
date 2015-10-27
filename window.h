#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>

class Window : public QWidget
{
  Q_OBJECT

public:
  Window();
  void SetTestImage();

private:
  QGraphicsView *view;
  QGraphicsScene *scene;
  QWidget *myGLWidget;
};

#endif
