#pragma once

#include <QWidget>

class QGraphicsOpacityEffect;
class QHBoxLayout;
class QLabel;
class QComboBox;
class ClickableLabel;

enum MeshQualityMetric {
    MESH_METRIC_JACOBIAN = 1,
    MESH_METRIC_AREA = 2,
    MESH_METRIC_ASPECT_RATIO = 3,
    MESH_METRIC_CONDITION = 4,
    MESH_METRIC_VOLUME = 5
};

class MeshQualityWidget : public QWidget {
    Q_OBJECT

public:
    explicit MeshQualityWidget(QWidget * parent = nullptr);

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
