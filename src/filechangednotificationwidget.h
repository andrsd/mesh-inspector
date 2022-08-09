#pragma once

#include <QWidget>

class QHBoxLayout;
class QLabel;
class ClickableLabel;

class FileChangedNotificationWidget : public QWidget {
    Q_OBJECT

public:
    explicit FileChangedNotificationWidget(QWidget * parent = nullptr);
    virtual ~FileChangedNotificationWidget();

    void setFileName(const QString & file_name);

signals:
    void reloaded();
    void dismissed();

protected slots:
    void onReload();
    void onDismiss();

protected:
    void setUpWidgets();

    QHBoxLayout * layout;
    QLabel * text;
    ClickableLabel * reload;
    ClickableLabel * dismiss;
};
