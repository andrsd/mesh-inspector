#pragma once

#include <QWidget>

class QGraphicsOpacityEffect;
class QHBoxLayout;
class QLabel;
class QComboBox;
class ClickableLabel;

class MeshQualityWidget : public QWidget {
    Q_OBJECT

public:
    explicit MeshQualityWidget(QWidget * parent = nullptr);
    virtual ~MeshQualityWidget();

    void setPosition(const QRect & geometry);
    int getMetricId();
    void done();

signals:
    void closed();
    void metricChanged(int metric_id);

protected slots:
    void onClose();
    void onMetricChanged(int index);

protected:
    QGraphicsOpacityEffect * opacity;
    QHBoxLayout * layout;
    QLabel * metric_label;
    QComboBox * metric;
    ClickableLabel * close;
};
