#pragma once

#include <QDialog>

class QVBoxLayout;
class QLabel;

class AboutDialog : public QDialog {
public:
    AboutDialog(QWidget * parent = nullptr);
    virtual ~AboutDialog();

protected:
    QVBoxLayout * layout;
    QLabel * icon;
    QLabel * title;
    QLabel * text;
};
