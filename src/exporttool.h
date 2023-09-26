#pragma once

#include <QObject>

class MainWindow;
class QMenu;
class QAction;

class ExportTool : public QObject {
public:
    explicit ExportTool(MainWindow * main_window);

    void setupMenu(QMenu * menu);
    void setMenuEnabled(bool enabled);

public slots:
    void onExportAsPng();
    void onExportAsJpg();

protected:
    QString getFileName(const QString & window_title,
                        const QString & name_filter,
                        const QString & default_suffix);

    MainWindow * main_window;
    QAction * export_as_png;
    QAction * export_as_jpg;
};
