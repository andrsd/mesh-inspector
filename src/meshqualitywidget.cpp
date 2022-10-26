#include "meshqualitywidget.h"
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QtDebug>
#include "common/clickablelabel.h"

MeshQualityWidget::MeshQualityWidget(QWidget * parent) : QWidget(parent)
{
    setAttribute(Qt::WA_StyledBackground, true);
    setObjectName("mesh-quality-wdgt");
    setStyleSheet("#mesh-quality-wdgt {"
                  "  border-radius: 3px;"
                  "  background-color: #222;"
                  "  color: #fff;"
                  "  font-size: 14px;"
                  "}"
                  "QLabel {"
                  "  background-color: #222;"
                  "  color: #fff;"
                  "}");
    this->opacity = new QGraphicsOpacityEffect(this);
    this->opacity->setOpacity(0.8);
    setGraphicsEffect(this->opacity);

    this->layout = new QHBoxLayout();
    this->layout->setContentsMargins(15, 8, 15, 8);

    this->metric_label = new QLabel();
    this->metric_label->setText("Metric");
    this->layout->addWidget(this->metric_label);

    this->metric = new QComboBox();
    this->metric->setEditable(false);
    this->metric->addItem("det(J)");
    this->layout->addWidget(this->metric);

    this->close = new ClickableLabel();
    this->close->setText("Close");
    this->close->setStyleSheet("font-weight: bold;");
    this->layout->addWidget(this->close);

    this->setLayout(this->layout);

    connect(this->close, SIGNAL(clicked()), this, SLOT(onClose()));
    connect(this->metric, SIGNAL(currentIndexChanged(int)), this, SLOT(onMetricChanged(int)));

    this->metric->setCurrentIndex(0);
}

MeshQualityWidget::~MeshQualityWidget() {}

void
MeshQualityWidget::setPosition(const QRect & geometry)
{
    adjustSize();
    int left = (geometry.width() - width()) / 2;
    int top = geometry.height() - height() - 10;
    setGeometry(left, top, width(), height());
}

void
MeshQualityWidget::onClose()
{
    hide();
    emit closed();
}

void
MeshQualityWidget::onMetricChanged(int index)
{
    // TODO: eventually convert index to ID when we have multiple metrics
    emit metricChanged(index);
}

int
MeshQualityWidget::getMetricId()
{
    return 0;
}

void
MeshQualityWidget::done()
{
    if (isVisible())
        onClose();
}
