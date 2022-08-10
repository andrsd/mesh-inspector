#include "colorpicker.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QLineEdit>
#include <QAbstractButton>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QRadioButton>

QList<QColor> ColorPicker::colors_tl =
    QList<QColor>({ QColor(0, 0, 0), QColor(19, 0, 255), QColor(254, 0, 0) });

QList<QColor> ColorPicker::colors_tr = QList<QColor>({ QColor(156, 207, 237),
                                                       QColor(165, 165, 165),
                                                       QColor(60, 97, 180),
                                                       QColor(234, 234, 234),
                                                       QColor(197, 226, 243),
                                                       QColor(247, 135, 3),
                                                       QColor(127, 127, 127),
                                                       QColor(250, 182, 0) });

ColorPicker::ColorPicker(QWidget * parent) :
    QDialog(parent),
    id(0),
    qcolor(QColor("#eee")),
    layout(nullptr),
    grid_layout(nullptr),
    color_group(nullptr),
    opacity_layout(nullptr),
    color_sample(nullptr),
    opacity_slider(nullptr),
    opacity(nullptr),
    rgb_layout(nullptr),
    lbl_name(nullptr),
    name(nullptr),
    lbl_red(nullptr),
    red(nullptr),
    lbl_green(nullptr),
    green(nullptr),
    lbl_blue(nullptr),
    blue(nullptr)
{
    setUpWidegts();
}

ColorPicker::~ColorPicker() {}

void
ColorPicker::setUpWidegts()
{
    this->layout = new QVBoxLayout();
    this->layout->setSizeConstraint(QLayout::SetFixedSize);
    this->layout->setContentsMargins(8, 8, 8, 10);
    this->layout->setSpacing(4);

    this->grid_layout = new QGridLayout();
    this->grid_layout->setContentsMargins(0, 0, 0, 0);
    this->grid_layout->setVerticalSpacing(8);

    this->color_group = new QButtonGroup();
    this->color_group->setExclusive(true);
    connect(this->color_group,
            SIGNAL(buttonClicked(QAbstractButton *)),
            this,
            SLOT(onColorPicked(QAbstractButton *)));

    // do these first so they get IDs starting from 0
    this->grid_layout->setColumnMinimumWidth(3, 5);
    this->grid_layout->setRowMinimumHeight(1, 5);

    fillLayoutWithColors(0, 4, ColorPicker::colors_tr);
    fillLayoutWithColors(0, 0, ColorPicker::colors_tl);
    fillLayoutWithGreys(2, 0);
    fillLayoutWithColors2(2, 4);

    this->layout->addLayout(this->grid_layout, 1);

    this->opacity_layout = new QHBoxLayout();
    this->opacity_layout->setContentsMargins(0, 0, 0, 0);

    this->color_sample = new QLabel();
    this->color_sample->setFixedSize(50, 20);
    updateColorSample();
    this->opacity_layout->addWidget(this->color_sample);

    this->opacity_slider = new QSlider(Qt::Horizontal);
    this->opacity_slider->setRange(0, 100);
    this->opacity_layout->addWidget(this->opacity_slider);

    this->opacity = new QLineEdit("0.99");
    this->opacity->setFixedWidth(40);
    this->opacity->setValidator(new QDoubleValidator(0., 1., 2));
    this->opacity_layout->addWidget(this->opacity);

    this->layout->addLayout(this->opacity_layout);

    this->rgb_layout = new QHBoxLayout();
    this->rgb_layout->setSpacing(5);

    this->lbl_name = new QLabel("#");
    this->rgb_layout->addWidget(this->lbl_name);

    this->name = new QLineEdit("FFFFFF");
    this->name->setFixedWidth(55);
    this->rgb_layout->addWidget(this->name);

    this->lbl_red = new QLabel("R");
    this->rgb_layout->addWidget(this->lbl_red);

    this->red = new QLineEdit("255");
    this->red->setFixedWidth(30);
    this->red->setValidator(new QIntValidator(0, 255));
    this->rgb_layout->addWidget(this->red);

    this->lbl_green = new QLabel("G");
    this->rgb_layout->addWidget(this->lbl_green);

    this->green = new QLineEdit("255");
    this->green->setFixedWidth(30);
    this->green->setValidator(new QIntValidator(0, 255));
    this->rgb_layout->addWidget(this->green);

    this->lbl_blue = new QLabel("B");
    this->rgb_layout->addWidget(this->lbl_blue);

    this->blue = new QLineEdit("255");
    this->blue->setFixedWidth(30);
    this->blue->setValidator(new QIntValidator(0, 255));
    this->rgb_layout->addWidget(this->blue);

    this->layout->addLayout(this->rgb_layout);

    setLayout(this->layout);
    setWindowTitle("Colors");
    setWindowFlag(Qt::Tool);
    setWindowFlag(Qt::CustomizeWindowHint);
    setWindowFlag(Qt::WindowMinMaxButtonsHint, false);

    connect(this->opacity_slider,
            SIGNAL(valueChanged(int)),
            this,
            SLOT(onOpacitySliderChanged(int)));
    connect(this->opacity,
            SIGNAL(textChanged(const QString &)),
            this,
            SLOT(onOpacityChanged(const QString &)));
    connect(this->red,
            SIGNAL(textChanged(const QString &)),
            this,
            SLOT(onRedChanged(const QString &)));
    connect(this->green,
            SIGNAL(textChanged(const QString &)),
            this,
            SLOT(onGreenChanged(const QString &)));
    connect(this->blue,
            SIGNAL(textChanged(const QString &)),
            this,
            SLOT(onBlueChanged(const QString &)));
}

void
ColorPicker::fillLayoutWithColors(int row, int col, const QList<QColor> & colors)
{
    for (const auto & qclr : colors) {
        QString color_str = qclr.name();

        auto * button = new QRadioButton("");
        button->setFixedSize(17, 17);
        auto qss = QString("QRadioButton::indicator {"
                           "    width: 15px;"
                           "    height: 15px;"
                           "    background-color: %1;"
                           "    border: 1px solid #eee;"
                           "}"
                           "QRadioButton::indicator::checked {"
                           "    border: 1px solid #000;"
                           "}")
                       .arg(color_str);
        button->setStyleSheet(qss);

        this->color_group->addButton(button, this->id);
        this->color_map[this->id] = qclr;
        this->id++;

        this->grid_layout->addWidget(button, row, col);
        col++;
    }
}

void
ColorPicker::fillLayoutWithGreys(int st_row, int st_col)
{
    QList<double> values({ 0.9, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3 });
    int row = 0;
    for (const auto & v : values) {
        auto greys = QList<QColor>({ QColor::fromHsv(0, 0, v * 255),
                                     QColor::fromHsv(44, 0.1 * 255, v * 255),
                                     QColor::fromHsv(207, 0.1 * 255, v * 255) });

        int col = 0;
        for (const auto & qclr : greys) {
            QString color_str = qclr.name();
            auto * button = new QRadioButton("");
            button->setFixedSize(17, 17);
            auto qss = QString("QRadioButton::indicator {"
                               "    width: 15px;"
                               "    height: 15px;"
                               "    background-color: %1;"
                               "    border: 1px solid #eee;"
                               "}"
                               "QRadioButton::indicator::checked {"
                               "    border: 1px solid #000;"
                               "}")
                           .arg(color_str);
            button->setStyleSheet(qss);

            this->color_group->addButton(button, this->id);
            this->color_map[this->id] = qclr;
            this->id++;

            this->grid_layout->addWidget(button, st_row + row, st_col + col);
            col++;
        }
        row++;
    }
}

void
ColorPicker::fillLayoutWithColors2(int st_row, int st_col)
{
    QList<int> hues = QList<int>({ 14, 32, 44, 100, 175, 214, 261, 291 });

    int col = 0;
    for (const auto & h : hues) {
        QList<QColor> qcolors;
        QList<double> lightnesses = QList<double>({ 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2 });
        QList<double> saturations = QList<double>({ 0.5, 0.6, 0.7, 0.73, 0.75, 0.84, 1.0 });

        for (int i = 0; i < lightnesses.size(); i++) {
            auto qclr = QColor::fromHsl(h, saturations[i] * 255, lightnesses[i] * 255);
            qcolors.append(qclr);
        }

        int row = 0;
        for (const auto & qclr : qcolors) {
            auto color_str = qclr.name();

            auto * button = new QRadioButton("");
            button->setFixedSize(17, 17);
            button->setStyleSheet(QString("QRadioButton::indicator {"
                                          "    width: 15px;"
                                          "    height: 15px;"
                                          "    background-color: %1;"
                                          "    border: 1px solid #eee;"
                                          "}"
                                          "QRadioButton::indicator::checked {"
                                          "    border: 1px solid #000;"
                                          "}")
                                      .arg(color_str));

            this->color_group->addButton(button, this->id);
            this->color_map[this->id] = qclr;
            this->id++;

            this->grid_layout->addWidget(button, st_row + row, st_col + col);
            row++;
        }
        col++;
    }
}

void
ColorPicker::setColor(const QColor & qcolor)
{
    this->qcolor = qcolor;
    updateColorWidgets();
}

QColor
ColorPicker::color() const
{
    return this->qcolor;
}

void
ColorPicker::setData(const QVariant & data)
{
    this->qdata = data;
}

QVariant
ColorPicker::data() const
{
    return this->qdata;
}

void
ColorPicker::updateColorSample()
{
    this->color_sample->setStyleSheet(QString("border: 1px solid #000;"
                                              "background: %1;")
                                          .arg(this->qcolor.name()));
}

void
ColorPicker::updateColorWidgets()
{
    this->name->setText(this->qcolor.name().mid(1));
    this->red->setText(QString::number(this->qcolor.red()));
    this->green->setText(QString::number(this->qcolor.green()));
    this->blue->setText(QString::number(this->qcolor.blue()));
    this->opacity->setText(QString::number(this->qcolor.alphaF()));
    updateColorSample();
}

void
ColorPicker::onColorPicked(QAbstractButton * button)
{
    int id = this->color_group->id(button);
    this->qcolor = this->color_map[id];
    updateColorWidgets();
    emit colorChanged(this->qcolor);
}

void
ColorPicker::onOpacitySliderChanged(int value)
{
    blockSignals(true);
    this->opacity->setText(QString::number(value / 100., 'f', 2));
    blockSignals(false);
}

void
ColorPicker::onOpacityChanged(const QString & text)
{
    blockSignals(true);
    if (text.length() > 0) {
        auto val = text.toDouble() * 100;
        this->opacity_slider->setValue(val);
    }
    else
        this->opacity_slider->setValue(0.);
    blockSignals(false);
}

void
ColorPicker::onRedChanged(const QString & text)
{
    if (text.length() > 0) {
        auto val = text.toInt();
        this->qcolor.setRed(val);
        blockSignals(true);
        updateColorWidgets();
        blockSignals(false);
    }
    else
        this->qcolor.setRed(0);
}

void
ColorPicker::onGreenChanged(const QString & text)
{
    if (text.length() > 0) {
        auto val = text.toInt();
        this->qcolor.setGreen(val);
        blockSignals(true);
        updateColorWidgets();
        blockSignals(false);
    }
    else
        this->qcolor.setGreen(0);
}

void
ColorPicker::onBlueChanged(const QString & text)
{
    if (text.length() > 0) {
        auto val = text.toInt();
        this->qcolor.setBlue(val);
        blockSignals(true);
        updateColorWidgets();
        blockSignals(false);
    }
    else
        this->qcolor.setBlue(0);
}
