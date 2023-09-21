#pragma once

#include <QDialog>

class QVBoxLayout;
class QLabel;

class AboutDialog : public QDialog {
public:
    explicit AboutDialog(QWidget * parent = nullptr);
    ~AboutDialog() override;

protected:
    QVBoxLayout * layout;
    QLabel * icon;
    QLabel * title;
    QLabel * text;
};
