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
    explicit ColorPicker(QWidget * parent = nullptr);
    ~ColorPicker() = default;

    void setColor(const QColor & color);
    QColor color() const;
    void setData(const QVariant & data);
    QVariant data() const;

signals:
    void colorChanged(const QColor & color);
    void opacityChanged(double opacity);

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
    void updateOpacityWidgets();

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
