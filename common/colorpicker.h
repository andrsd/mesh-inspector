#pragma once

#include <QDialog>
#include <QColor>
#include <QVariant>
#include <QMap>
#include <QList>

class QVBoxLayout;
class QGridLayout;
class QButtonGroup;
class QHBoxLayout;
class QLabel;
class QSlider;
class QLineEdit;
class QAbstractButton;

class ColorPicker : public QDialog {
    Q_OBJECT;

public:
    ColorPicker(QWidget * parent = nullptr);
    virtual ~ColorPicker();

    void setColor(const QColor & qcolor);
    QColor color() const;
    void setData(const QVariant & data);
    QVariant data() const;

signals:
    void colorChanged(const QColor & color);

protected slots:
    void onColorPicked(QAbstractButton * button);
    void onOpacitySliderChanged(int value);
    void onOpacityChanged(const QString & text);
    void onRedChanged(const QString & text);
    void onGreenChanged(const QString & text);
    void onBlueChanged(const QString & text);

protected:
    void setUpWidegts();
    void fillLayoutWithColors(int row, int col, const QList<QColor> & colors);
    void fillLayoutWithGreys(int st_row, int st_col);
    void fillLayoutWithColors2(int st_row, int st_col);
    void updateColorSample();
    void updateColorWidgets();

    int id;
    QColor qcolor;
    QVariant qdata;
    QMap<int, QColor> color_map;

    QVBoxLayout * layout;
    QGridLayout * grid_layout;
    QButtonGroup * color_group;
    QHBoxLayout * opacity_layout;
    QLabel * color_sample;
    QSlider * opacity_slider;
    QLineEdit * opacity;
    QHBoxLayout * rgb_layout;
    QLabel * lbl_name;
    QLineEdit * name;
    QLabel * lbl_red;
    QLineEdit * red;
    QLabel * lbl_green;
    QLineEdit * green;
    QLabel * lbl_blue;
    QLineEdit * blue;

protected:
    static QList<QColor> colors_tl;
    static QList<QColor> colors_tr;
};
