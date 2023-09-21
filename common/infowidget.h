#pragma once

#include <QWidget>

class QString;
class QGraphicsOpacityEffect;
class QVBoxLayout;
class QLabel;

class InfoWidget : public QWidget {
public:
    explicit InfoWidget(QWidget * parent = nullptr);
    ~InfoWidget() override;

    void setText(const QString & text);
    void clear();

protected:
    QGraphicsOpacityEffect * opacity;
    QVBoxLayout * layout;
    QLabel * text;
};
