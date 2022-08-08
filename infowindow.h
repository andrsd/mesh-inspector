#pragma once

#include <QScrollArea>

class QWidget;
class QVBoxLayout;
class QLabel;
class QStandardItemModel;
class OTreeView;
class QTreeWidget;
class ExpandableWidget;
class QTreeWidgetItem;
class QCheckBox;

class InfoWindow : public QScrollArea {
    Q_OBJECT

public:
    explicit InfoWindow(QWidget * parent = nullptr);

protected:
    void setupWidgets();
    void setupBlocksWidgets();
    void setupSidesetsWidgets();
    void setupNodesetsWidgets();
    void setupSummaryWidgets();
    void setupRangeWidgets();

    QWidget * widget;
    QVBoxLayout * layout;
    QLabel * lbl_info;
    QStandardItemModel * block_model;
    OTreeView * blocks;
    QStandardItemModel * nodeset_model;
    OTreeView * nodesets;
    ExpandableWidget * nodesets_expd;
    QStandardItemModel * sideset_model;
    OTreeView * sidesets;
    ExpandableWidget * sidesets_expd;
    QTreeWidget * totals;
    QTreeWidgetItem * total_elements;
    QTreeWidgetItem * total_nodes;
    ExpandableWidget * totals_expd;
    QTreeWidget * range;
    QTreeWidgetItem * x_range;
    QTreeWidgetItem * y_range;
    QTreeWidgetItem * z_range;
    QCheckBox * dimensions;
    ExpandableWidget * range_expd;

protected:
    static const int IDX_NAME = 0;
    static const int IDX_COLOR = 1;
    static const int IDX_ID = 2;
};
