#include "infoview.h"
#include "mainwindow.h"
#include "model.h"
#include <QDockWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QTreeWidgetItem>
#include <QCheckBox>
#include <QBrush>
#include <QMenu>
#include <QLocale>
#include "common/horzline.h"
#include "common/expandablewidget.h"
#include "common/otreeview.h"
#include "common/colorpicker.h"
#include "meshmodel.h"

QList<QColor> InfoView::colors({ QColor(156, 207, 237),
                                 QColor(165, 165, 165),
                                 QColor(60, 97, 180),
                                 QColor(234, 234, 234),
                                 QColor(197, 226, 243),
                                 QColor(127, 127, 127),
                                 QColor(250, 182, 0) });

InfoView::InfoView(MainWindow * main_wnd) :
    QDockWidget(main_wnd),
    model(main_wnd->getModel()),
    widget(nullptr),
    layout(nullptr),
    mesh_model(nullptr),
    mesh_view(nullptr),
    totals(nullptr),
    total_elements(nullptr),
    total_nodes(nullptr),
    totals_expd(nullptr),
    range(nullptr),
    x_range(nullptr),
    y_range(nullptr),
    z_range(nullptr),
    dimensions(nullptr),
    range_expd(nullptr),
    color_picker(nullptr),

    block_root(nullptr),
    sideset_root(nullptr),
    nodeset_root(nullptr)
{
    setTitleBarWidget(new QWidget());

    setupWidgets();
    this->widget = new QWidget();
    this->widget->setLayout(this->layout);
    this->widget->setFixedWidth(300);

    this->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    this->setFloating(false);
    this->setFeatures(QDockWidget::DockWidgetMovable);
    this->setWidget(this->widget);

    show();
}

InfoView::~InfoView() {}

void
InfoView::setupWidgets()
{
    this->layout = new QVBoxLayout();
    this->layout->setContentsMargins(20, 10, 20, 10);
    this->layout->setSpacing(4);

    this->title = new QLabel();
    this->title->setText("Information");
    this->title->setStyleSheet("color: #444;"
                               "font-weight: bold;");
    this->title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    this->title->setContentsMargins(0, 0, 0, 4);
    this->layout->addWidget(this->title);

    this->color_picker = new ColorPicker(this);
    connect(this->color_picker, &ColorPicker::colorChanged, this, &InfoView::onBlockColorPicked);
    connect(this->color_picker,
            &ColorPicker::opacityChanged,
            this,
            &InfoView::onBlockOpacityChanged);

    setupBlocksWidgets();
    setupSummaryWidgets();
    setupRangeWidgets();
}

void
InfoView::setupBlocksWidgets()
{
    this->mesh_model = new MeshModel();
    this->mesh_model->setHorizontalHeaderLabels(QStringList({ "Name", "", "ID" }));
    connect(this->mesh_model, &MeshModel::itemChanged, this, &InfoView::onItemChanged);
    this->mesh_view = new OTreeView();
    this->mesh_view->setRootIsDecorated(true);
    this->mesh_view->setModel(this->mesh_model);
    this->mesh_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->mesh_view->setColumnWidth(0, 170);
    this->mesh_view->setColumnWidth(1, 20);
    this->mesh_view->setColumnWidth(2, 40);
    this->mesh_view->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this->mesh_view,
            &OTreeView::customContextMenuRequested,
            this,
            &InfoView::onItemCustomContextMenu);
    auto * sel_model = this->mesh_view->selectionModel();
    connect(sel_model,
            &QItemSelectionModel::selectionChanged,
            this,
            &InfoView::onItemSelectionChanged);
    this->layout->addWidget(this->mesh_view);
}

void
InfoView::setupSummaryWidgets()
{
    this->totals = new QTreeWidget();
    this->totals->setFixedHeight(60);
    this->totals->setIndentation(0);
    this->totals->setHeaderLabels(QStringList({ "Total", "Count" }));
    this->total_elements = new QTreeWidgetItem(QStringList({ "Elements", "" }));
    this->totals->addTopLevelItem(this->total_elements);
    this->total_nodes = new QTreeWidgetItem(QStringList({ "Nodes", "" }));
    this->totals->addTopLevelItem(this->total_nodes);

    this->totals_expd = new ExpandableWidget("Summary");
    this->totals_expd->setWidget(this->totals);
    this->totals_expd->setExpanded(true);
    this->layout->addWidget(this->totals_expd);
}

void
InfoView::setupRangeWidgets()
{
    this->range = new QTreeWidget();
    this->range->setFixedHeight(80);
    this->range->setIndentation(0);
    this->range->setHeaderLabels(QStringList({ "Dimension", "Range" }));
    this->x_range = new QTreeWidgetItem(QStringList({ "X", "" }));
    this->range->addTopLevelItem(this->x_range);
    this->y_range = new QTreeWidgetItem(QStringList({ "Y", "" }));
    this->range->addTopLevelItem(this->y_range);
    this->z_range = new QTreeWidgetItem(QStringList({ "Z", "" }));
    this->range->addTopLevelItem(this->z_range);

    this->dimensions = new QCheckBox("Show dimensions");
    connect(this->dimensions, &QCheckBox ::stateChanged, this, &InfoView::onDimensionsStateChanged);

    auto * l = new QVBoxLayout();
    l->setSpacing(8);
    l->setContentsMargins(0, 0, 0, 0);
    l->addWidget(this->range);
    l->addWidget(this->dimensions);

    auto w = new QWidget();
    w->setLayout(l);

    this->range_expd = new ExpandableWidget("Dimensions");
    this->range_expd->setWidget(w);
    this->range_expd->setExpanded(true);
    this->layout->addWidget(this->range_expd);
}

void
InfoView::onBlockAdded(int id, const QString & name)
{
    addBlocksRoot();

    QModelIndex index = this->block_root->index();
    int row = this->mesh_model->rowCount(index);

    auto * si_name = new QStandardItem();
    si_name->setText(name);
    si_name->setCheckable(true);
    si_name->setCheckState(Qt::Checked);
    si_name->setData(id);
    this->block_root->setChild(row, IDX_NAME, si_name);

    auto * si_clr = new QStandardItem();
    si_clr->setText("\u25a0");
    int clr_idx = row % (int) InfoView::colors.length();
    auto color = InfoView::colors[clr_idx];
    si_clr->setForeground(QBrush(color));
    si_clr->setData(color);
    this->block_root->setChild(row, IDX_COLOR, si_clr);

    auto * si_id = new QStandardItem();
    si_id->setText(QString::number(id));
    this->block_root->setChild(row, IDX_ID, si_id);

    QString text = QString("Blocks (%1)").arg(this->block_root->rowCount());
    this->block_root->setText(text);
}

void
InfoView::onItemChanged(QStandardItem * item)
{
    auto * parent = item->parent();
    if (parent == this->block_root)
        onBlockChanged(item);
    else if (parent == this->sideset_root)
        onSideSetChanged(item);
    else if (parent == this->nodeset_root)
        onNodeSetChanged(item);
}

void
InfoView::onBlockChanged(QStandardItem * item)
{
    if (item->column() == IDX_NAME) {
        bool visible = item->checkState() == Qt::Checked;
        int block_id = item->data().toInt();
        emit blockVisibilityChanged(block_id, visible);
    }
    else if (item->column() == IDX_COLOR) {
        auto color = item->foreground().color();
        auto index = this->mesh_model->indexFromItem(item);
        auto name_index = index.siblingAtColumn(IDX_NAME);
        int block_id = this->mesh_model->itemFromIndex(name_index)->data().toInt();
        emit blockColorChanged(block_id, color);
    }
}

void
InfoView::setColorPickerColorFromIndex(const QModelIndex & index)
{
    blockSignals(true);
    auto clr_index = index.siblingAtColumn(IDX_COLOR);
    auto * clr_item = this->mesh_model->itemFromIndex(clr_index);
    auto qcolor = clr_item->data().value<QColor>();
    this->color_picker->setData(clr_index);
    this->color_picker->setColor(qcolor);
    blockSignals(false);
}

void
InfoView::onItemSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
    if (selected.indexes().length() > 0) {
        auto index = selected.indexes()[0];
        auto * item = this->mesh_model->itemFromIndex(index);

        auto * parent = item->parent();
        if (parent == this->block_root)
            onBlockSelectionChanged(item);
        else if (parent == this->sideset_root)
            onSideSetSelectionChanged(item);
        else if (parent == this->nodeset_root)
            onNodeSetSelectionChanged(item);
    }
    else {
        onBlockSelectionChanged(nullptr);
        onSideSetSelectionChanged(nullptr);
        onNodeSetSelectionChanged(nullptr);
    }
}

void
InfoView::onBlockSelectionChanged(QStandardItem * item)
{
    if (item) {
        int block_id = item->data().toInt();
        setColorPickerColorFromIndex(item->index());
        emit blockSelectionChanged(block_id);
    }
    else {
        this->color_picker->setData(QVariant());
        emit blockSelectionChanged(-1);
    }
}

void
InfoView::onNameContextMenu(QStandardItem * item, const QPoint & point)
{
    QMenu menu;
    if (item->checkState() == Qt::Checked) {
        menu.addAction("Hide", this, &InfoView::onHideBlock);
        menu.addAction("Hide others", this, &InfoView::onHideOtherBlocks);
        menu.addAction("Hide all", this, &InfoView::onHideAllBlocks);
    }
    else {
        menu.addAction("Show", this, &InfoView::onShowBlock);
        menu.addAction("Show all", this, &InfoView::onShowAllBlocks);
    }
    menu.addSeparator();
    menu.addAction("Appearance...", this, &InfoView::onAppearance);
    menu.exec(point);
}

void
InfoView::onItemCustomContextMenu(const QPoint & point)
{
    auto index = this->mesh_view->indexAt(point);
    if (index.isValid()) {
        auto * item = this->mesh_model->itemFromIndex(index);

        auto * parent = item->parent();
        if (parent == this->block_root)
            onNameContextMenu(item, this->mesh_view->viewport()->mapToGlobal(point));
    }
}

void
InfoView::onDimensionsStateChanged(int state)
{
    emit dimensionsStateChanged(state == Qt::Checked);
}

void
InfoView::onHideBlock()
{
    auto * selection_model = this->mesh_view->selectionModel();
    auto indexes = selection_model->selectedIndexes();
    if (indexes.length() > 0) {
        auto index = indexes[0];
        auto * item = this->mesh_model->itemFromIndex(index);
        item->setCheckState(Qt::Unchecked);
    }
}

void
InfoView::onHideOtherBlocks()
{
    auto * selection_model = this->mesh_view->selectionModel();
    auto indexes = selection_model->selectedIndexes();
    if (indexes.length() > 0) {
        auto index = indexes[0];
        for (int row = 0; row < this->block_root->rowCount(); row++) {
            if (row != index.row()) {
                auto * item = this->block_root->child(row, 0);
                item->setCheckState(Qt::Unchecked);
            }
        }
    }
}

void
InfoView::onHideAllBlocks()
{
    for (int row = 0; row < this->block_root->rowCount(); row++) {
        auto * item = this->block_root->child(row, 0);
        item->setCheckState(Qt::Unchecked);
    }
}

void
InfoView::onShowBlock()
{
    auto * selection_model = this->mesh_view->selectionModel();
    auto indexes = selection_model->selectedIndexes();
    if (indexes.length() > 0) {
        auto index = indexes[0];
        auto * item = this->mesh_model->itemFromIndex(index);
        item->setCheckState(Qt::Checked);
    }
}

void
InfoView::onShowAllBlocks()
{
    for (int row = 0; row < this->block_root->rowCount(); row++) {
        auto * item = this->block_root->child(row, 0);
        item->setCheckState(Qt::Checked);
    }
}

void
InfoView::onAppearance()
{
    auto * selection_model = this->mesh_view->selectionModel();
    auto indexes = selection_model->selectedIndexes();
    if (indexes.length() == 0)
        return;
    auto index = indexes[0];
    setColorPickerColorFromIndex(index);
    this->color_picker->show();
}

void
InfoView::onSideSetAdded(int id, const QString & name)
{
    addSideSetsRoot();

    QModelIndex index = this->sideset_root->index();
    auto row = this->mesh_model->rowCount(index);

    auto * si_name = new QStandardItem();
    si_name->setText(name);
    si_name->setCheckable(true);
    si_name->setData(id);
    this->sideset_root->setChild(row, IDX_NAME, si_name);

    auto * si_id = new QStandardItem();
    si_id->setText(QString::number(id));
    this->sideset_root->setChild(row, IDX_ID, si_id);

    QString text = QString("Side sets (%1)").arg(this->sideset_root->rowCount());
    this->sideset_root->setText(text);
}

void
InfoView::onSideSetChanged(QStandardItem * item)
{
    if (item->column() == IDX_NAME) {
        bool visible = item->checkState() == Qt::Checked;
        auto sideset_id = item->data().toInt();
        emit sideSetVisibilityChanged(sideset_id, visible);
    }
}

void
InfoView::onSideSetSelectionChanged(QStandardItem * item)
{
    if (item) {
        auto sideset_id = item->data().toInt();
        emit sideSetSelectionChanged(sideset_id);
    }
    else {
        emit sideSetSelectionChanged(-1);
    }
}

void
InfoView::onNodeSetAdded(int id, const QString & name)
{
    addNodeSetsRoot();

    QModelIndex index = this->nodeset_root->index();
    auto row = this->mesh_model->rowCount(index);

    auto * si_name = new QStandardItem();
    si_name->setText(name);
    si_name->setCheckable(true);
    si_name->setData(id);
    this->nodeset_root->setChild(row, IDX_NAME, si_name);

    auto * si_id = new QStandardItem();
    si_id->setText(QString::number(id));
    this->nodeset_root->setChild(row, IDX_ID, si_id);

    QString text = QString("Node sets (%1)").arg(this->nodeset_root->rowCount());
    this->nodeset_root->setText(text);
}

void
InfoView::onNodeSetChanged(QStandardItem * item)
{
    if (item->column() == IDX_NAME) {
        bool visible = item->checkState() == Qt::Checked;
        auto nodeset_id = item->data().toInt();
        emit nodeSetVisibilityChanged(nodeset_id, visible);
    }
}

void
InfoView::onNodeSetSelectionChanged(QStandardItem * item)
{
    if (item) {
        auto nodeset_id = item->data().toInt();
        emit nodeSetSelectionChanged(nodeset_id);
    }
    else {
        emit nodeSetSelectionChanged(-1);
    }
}

void
InfoView::setSummary(int total_elems, int total_nodes)
{
    this->total_elements->setText(1, QLocale::system().toString(total_elems));
    this->total_nodes->setText(1, QLocale::system().toString(total_nodes));
}

void
InfoView::setBounds(double xmin, double xmax, double ymin, double ymax, double zmin, double zmax)
{
    char format = 'f';
    int precision = 5;
    auto x_range_str = QString("%1 to %2")
                           .arg(QString::number(xmin, format, precision))
                           .arg(QString::number(xmax, format, precision));
    this->x_range->setText(1, x_range_str);
    auto y_range_str = QString("%1 to %2")
                           .arg(QString::number(ymin, format, precision))
                           .arg(QString::number(ymax, format, precision));
    this->y_range->setText(1, y_range_str);
    auto z_range_str = QString("%1 to %2")
                           .arg(QString::number(zmin, format, precision))
                           .arg(QString::number(zmax, format, precision));
    this->z_range->setText(1, z_range_str);
}

void
InfoView::onBlockColorPicked(const QColor & qcolor)
{
    QVariant data = this->color_picker->data();
    if (data.isValid()) {
        auto index = data.value<QModelIndex>();
        auto * item = this->mesh_model->itemFromIndex(index);
        item->setForeground(QBrush(qcolor));
        item->setData(qcolor);
    }
}

void
InfoView::onBlockOpacityChanged(double opacity)
{
    QVariant data = this->color_picker->data();
    if (data.isValid()) {
        auto clr_index = data.value<QModelIndex>();
        auto * clr_item = this->mesh_model->itemFromIndex(clr_index);
        auto color = clr_item->foreground().color();
        color.setAlphaF(opacity);
        clr_item->setForeground(QBrush(color));
        clr_item->setData(color);

        auto index = clr_index.siblingAtColumn(IDX_NAME);
        auto * item = this->mesh_model->itemFromIndex(index);
        auto block_id = item->data().toInt();
        emit blockOpacityChanged(block_id, opacity);
    }
}

void
InfoView::clear()
{
    this->mesh_model->removeRows(0, this->mesh_model->rowCount());
    this->block_root = nullptr;
    this->sideset_root = nullptr;
    this->nodeset_root = nullptr;

    this->total_elements->setText(1, "");
    this->total_nodes->setText(1, "");

    this->x_range->setText(1, "");
    this->y_range->setText(1, "");
    this->z_range->setText(1, "");
}

void
InfoView::addBlocksRoot()
{
    if (this->block_root == nullptr) {
        auto * si_name = new QStandardItem();
        si_name->setText("Blocks");
        si_name->setCheckable(true);
        si_name->setCheckState(Qt::Checked);
        si_name->setAutoTristate(true);
        this->mesh_model->setItem(0, si_name);
        this->block_root = this->mesh_model->item(0);
        this->mesh_view->expand(this->block_root->index());
    }
}

void
InfoView::addSideSetsRoot()
{
    if (this->sideset_root == nullptr) {
        auto * si_name = new QStandardItem();
        si_name->setText("Side sets");
        si_name->setCheckable(true);
        si_name->setCheckState(Qt::Unchecked);
        si_name->setAutoTristate(true);
        this->mesh_model->setItem(1, si_name);
        this->sideset_root = this->mesh_model->item(1);
    }
}

void
InfoView::addNodeSetsRoot()
{
    if (this->nodeset_root == nullptr) {
        auto * si_name = new QStandardItem();
        si_name->setText("Node sets");
        si_name->setCheckable(true);
        si_name->setCheckState(Qt::Unchecked);
        si_name->setAutoTristate(true);
        this->mesh_model->setItem(2, si_name);
        this->nodeset_root = this->mesh_model->item(2);
    }
}

void
InfoView::update()
{
    auto bbox = this->model->getTotalBoundingBox();
    double bounds[6];
    bbox.GetBounds(bounds);
    setBounds(bounds[0], bounds[1], bounds[2], bounds[3], bounds[4], bounds[5]);

    setSummary(this->model->getTotalNumberOfElements(), this->model->getTotalNumberOfNodes());
}
