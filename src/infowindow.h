#pragma once

#include <QScrollArea>
#include <QList>
#include <QColor>

class QWidget;
class QVBoxLayout;
class QLabel;
class QStandardItemModel;
class OTreeView;
class QTreeWidget;
class ExpandableWidget;
class QTreeWidgetItem;
class QCheckBox;
class QStandardItem;
class QItemSelection;

class InfoWindow : public QScrollArea {
    Q_OBJECT

public:
    explicit InfoWindow(QWidget * parent = nullptr);

    void clear();
    void setSummary(int total_elems, int total_nodes);
    void setBounds(double xmin, double xmax, double ymin, double ymax, double zmin, double zmax);

signals:
    void blockVisibilityChanged(int block_number, bool visible);
    void blockColorChanged(int block_number, QColor color);
    void blockSelectionChanged(int block_number);
    void sideSetVisibilityChanged(int sideset_id, bool visible);
    void sideSetSelectionChanged(int sideset_id);
    void nodeSetVisibilityChanged(int nodeset_id, bool visible);
    void nodeSetSelectionChanged(int nodeset_id);
    void dimensionsStateChanged(bool visible);

public slots:
    void onBlockAdded(int id, const QString & name);
    void onSideSetAdded(int id, const QString & name);
    void onNodeSetAdded(int id, const QString & name);

protected slots:
    void onBlockChanged(QStandardItem * item);
    void onBlockSelectionChanged(const QItemSelection & selected,
                                 const QItemSelection & deselected);
    void onNameContextMenu(QStandardItem * item, const QPoint & point);
    void onBlockCustomContextMenu(const QPoint &);

    void onSideSetChanged(QStandardItem * item);
    void onSideSetSelectionChanged(const QItemSelection & selected,
                                   const QItemSelection & deselected);

    void onNodeSetChanged(QStandardItem * item);
    void onNodeSetSelectionChanged(const QItemSelection & selected,
                                   const QItemSelection & deselected);

    void onDimensionsStateChanged(int state);

    void onHideBlock();
    void onHideOtherBlocks();
    void onHideAllBlocks();
    void onShowBlock();
    void onShowAllBlocks();
    void onAppearance();

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

    static QList<QColor> colors;
};
