#pragma once

#include <QDockWidget>

class MainWindow;
class Model;
class QWidget;
class QVBoxLayout;
class QLabel;
class MeshModel;
class OTreeView;
class QTreeWidget;
class ExpandableWidget;
class QTreeWidgetItem;
class QCheckBox;
class QStandardItem;
class QItemSelection;
class ColorPicker;

class InfoView : public QDockWidget {
    Q_OBJECT

public:
    explicit InfoView(MainWindow * main_wnd);
    ~InfoView() override;

    void clear();
    void init();
    void update();
    void setSummary(int total_elems, int total_nodes);
    void setBounds(double xmin, double xmax, double ymin, double ymax, double zmin, double zmax);

signals:
    void blockVisibilityChanged(int block_number, bool visible);
    void blockColorChanged(int block_number, QColor color);
    void blockOpacityChanged(int block_id, double opacity);
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
    void onItemChanged(QStandardItem * item);
    void onItemSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected);
    void onItemCustomContextMenu(const QPoint & point);
    void onBlockColorPicked(const QColor & qcolor);
    void onBlockOpacityChanged(double opacity);
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
    void setupSummaryWidgets();
    void setupRangeWidgets();
    void setColorPickerColorFromIndex(const QModelIndex & index);

    void onBlockChanged(QStandardItem * item);
    void onSideSetChanged(QStandardItem * item);
    void onNodeSetChanged(QStandardItem * item);

    void onNameContextMenu(QStandardItem * item, const QPoint & point);

    void onBlockSelectionChanged(QStandardItem * item);
    void onSideSetSelectionChanged(QStandardItem * item);
    void onNodeSetSelectionChanged(QStandardItem * item);

    Model * & model;
    QWidget * widget;
    QVBoxLayout * layout;
    MeshModel * mesh_model;
    OTreeView * mesh_view;
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
    ColorPicker * color_picker;

    QStandardItem * block_root;
    QStandardItem * sideset_root;
    QStandardItem * nodeset_root;

protected:
    static const int IDX_NAME = 0;
    static const int IDX_COLOR = 1;
    static const int IDX_ID = 2;

    static QList<QColor> colors;
};
