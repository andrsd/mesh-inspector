#include "mainwindow.h"
#include <QApplication>
#include "QVTKOpenGLNativeWidget.h"

int
main(int argc, char * argv[])
{
    QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);
    MainWindow w;
    w.show();
    return app.exec();
}
