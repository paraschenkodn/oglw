#include "oglw.h"
#include <QApplication>
#include <QSurfaceFormat>
#include "window.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  //OGLW w;
  //w.show();

  //
  Window window;
  window.show();

  window.resize(640,480);
  window.SetTestImage();
  //*/

  return a.exec();
}
