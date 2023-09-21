#pragma once

#include <QWidget>

class QHBoxLayout;
class QLabel;
class ClickableLabel;
class QShortcut;

class FileChangedNotificationWidget : public QWidget {
    Q_OBJECT

public:
    explicit FileChangedNotificationWidget(QWidget * parent = nullptr);
    ~FileChangedNotificationWidget() override;

    void setFileName(const QString & file_name);

signals:
    void reloaded();
    void dismissed();

protected slots:
    void onReload();
    void onDismiss();

protected:
    void setUpWidgets();
    void showEvent(QShowEvent * event) override;
    void hideEvent(QHideEvent * event) override;

    QHBoxLayout * layout;
    QLabel * text;
    ClickableLabel * reload;
    ClickableLabel * dismiss;
    QShortcut * dismiss_shortcut;
    QShortcut * reload_shortcut;
};
