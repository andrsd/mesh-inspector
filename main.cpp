// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "meshinspectorconfig.h"
#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include "QVTKOpenGLNativeWidget.h"
#include "common/loadfileevent.h"

int
main(int argc, char * argv[])
{
    QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

    QApplication app(argc, argv);
    QCoreApplication::setApplicationName(MESH_INSPECTOR_APP_NAME);
    QCoreApplication::setApplicationVersion(MESH_INSPECTOR_VERSION);

    QCommandLineParser parser;
    parser.setApplicationDescription("View mesh files");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file",
                                 QCoreApplication::translate("main", "Mesh file to load"),
                                 "[file]");
    parser.process(app);

    const QStringList args = parser.positionalArguments();

#ifdef __APPLE__
    app.setQuitOnLastWindowClosed(false);
#endif
    app.setWindowIcon(QIcon(":/resources/app-icon.png"));
    MainWindow w;
    w.show();

    if (args.length() > 0) {
        auto * event = new LoadFileEvent(args[0]);
        app.postEvent(&w, event);
    }

    return app.exec();
}
