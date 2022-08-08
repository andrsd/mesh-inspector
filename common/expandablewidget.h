#pragma once

#include <QWidget>
#include <QString>

class QPushButton;
class QLabel;
class QGridLayout;

class ExpandableWidget : public QWidget {
    Q_OBJECT

public:
    ExpandableWidget(const QString & text, QWidget * parent = nullptr);
    virtual ~ExpandableWidget();

    void setLabel(const QString & text);
    void setWidget(QWidget * widget);

protected slots:
    void onExpandToggled(bool checked);

protected:
    void setExpandButtonText(bool checked);
    void setNumberOfItems(int num);

    QString caption;
    QWidget * widget;
    QPushButton * expand_button;
    QLabel * label;
    QGridLayout * layout;
};
