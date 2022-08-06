#pragma once

#include <QScrollArea>

class QWidget;
class QVBoxLayout;

class InfoWindow : public QScrollArea {
public:
    explicit InfoWindow(QWidget * parent = nullptr);

protected:
    void setupWidgets();

    QWidget * widget;
    QVBoxLayout * layout;
};
