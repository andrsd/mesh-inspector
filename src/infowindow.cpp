#include "infowindow.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QStandardItemModel>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QCheckBox>
#include <QBrush>
#include <QMenu>
#include <QLocale>
#include "common/horzline.h"
#include "common/expandablewidget.h"
#include "common/otreeview.h"
#include "common/colorpicker.h"

QList<QColor> InfoWindow::colors({ QColor(156, 207, 237),
                                   QColor(165, 165, 165),
                                   QColor(60, 97, 180),
                                   QColor(234, 234, 234),
                                   QColor(197, 226, 243),
                                   QColor(127, 127, 127),
                                   QColor(250, 182, 0) });

InfoWindow::InfoWindow(QWidget * parent) :
    QScrollArea(parent),
    widget(nullptr),
    layout(nullptr),
    lbl_info(nullptr),
    block_model(nullptr),
    blocks(nullptr),
    nodeset_model(nullptr),
    nodesets(nullptr),
    sideset_model(nullptr),
    sidesets(nullptr),
    totals(nullptr),
    total_elements(nullptr),
    totals_expd(nullptr),
    range(nullptr),
    x_range(nullptr),
    y_range(nullptr),
    z_range(nullptr),
    dimensions(nullptr),
    range_expd(nullptr),
    color_picker(nullptr)
{
    setupWidgets();

    this->widget = new QWidget();
    this->widget->setLayout(this->layout);
    this->setWidget(this->widget);
    this->setWindowTitle("Info");
    this->setFixedWidth(300);
    this->setWidgetResizable(true);
    this->setWindowFlag(Qt::Tool);

    show();
}

void
InfoWindow::setupWidgets()
{
    this->layout = new QVBoxLayout();
    this->layout->setContentsMargins(20, 10, 20, 10);
    this->layout->setSpacing(4);

    this->color_picker = new ColorPicker(this);
    connect(this->color_picker,
            SIGNAL(colorChanged(const QColor &)),
            this,
            SLOT(onBlockColorPicked(const QColor &)));
    connect(this->color_picker,
            SIGNAL(opacityChanged(double)),
            this,
            SLOT(onBlockOpacityChanged(double)));

    setupBlocksWidgets();
    this->layout->addWidget(new HorzLine());
    setupSidesetsWidgets();
    this->layout->addWidget(new HorzLine());
    setupNodesetsWidgets();
    this->layout->addWidget(new HorzLine());
    setupSummaryWidgets();
    this->layout->addWidget(new HorzLine());
    setupRangeWidgets();
    this->layout->addWidget(new HorzLine());

    this->layout->addStretch();
}

void
InfoWindow::setupBlocksWidgets()
{
    this->lbl_info = new QLabel("Blocks");
    this->lbl_info->setStyleSheet("color: #444;"
                                  "font-weight: bold;");
    this->layout->addWidget(this->lbl_info);

    this->block_model = new QStandardItemModel();
    this->block_model->setHorizontalHeaderLabels(QStringList({ "Name", "", "ID" }));
    connect(this->block_model,
            SIGNAL(itemChanged(QStandardItem *)),
            this,
            SLOT(onBlockChanged(QStandardItem *)));
    this->blocks = new OTreeView();
    this->blocks->setFixedHeight(250);
    this->blocks->setRootIsDecorated(false);
    this->blocks->setModel(this->block_model);
    this->blocks->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->blocks->setColumnWidth(0, 170);
    this->blocks->setColumnWidth(1, 20);
    this->blocks->setColumnWidth(2, 40);
    this->blocks->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this->blocks,
            SIGNAL(customContextMenuRequested(const QPoint &)),
            this,
            SLOT(onBlockCustomContextMenu(const QPoint &)));
    auto * sel_model = this->blocks->selectionModel();
    connect(sel_model,
            SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this,
            SLOT(onBlockSelectionChanged(const QItemSelection &, const QItemSelection &)));
    this->layout->addWidget(this->blocks);
}

void
InfoWindow::setupSidesetsWidgets()
{
    this->sideset_model = new QStandardItemModel();
    this->sideset_model->setHorizontalHeaderLabels(QStringList({ "Name", "", "ID" }));
    connect(this->sideset_model,
            SIGNAL(itemChanged(QStandardItem *)),
            this,
            SLOT(onSideSetChanged(QStandardItem *)));

    this->sidesets = new OTreeView();
    this->sidesets->setFixedHeight(150);
    this->sidesets->setRootIsDecorated(false);
    this->sidesets->setModel(this->sideset_model);
    this->sidesets->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->sidesets->setColumnWidth(0, 190);
    this->sidesets->setColumnWidth(2, 40);
    this->sidesets->hideColumn(IDX_COLOR);
    auto * sel_model = this->sidesets->selectionModel();
    connect(sel_model,
            SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this,
            SLOT(onSideSetSelectionChanged(const QItemSelection &, const QItemSelection &)));

    this->sidesets_expd = new ExpandableWidget("Side sets");
    this->sidesets_expd->setWidget(this->sidesets);
    this->layout->addWidget(this->sidesets_expd);
}

void
InfoWindow::setupNodesetsWidgets()
{
    this->nodeset_model = new QStandardItemModel();
    this->nodeset_model->setHorizontalHeaderLabels(QStringList({ "Name", "", "ID" }));
    connect(this->nodeset_model,
            SIGNAL(itemChanged(QStandardItem *)),
            this,
            SLOT(onNodeSetChanged(QStandardItem *)));

    this->nodesets = new OTreeView();
    this->nodesets->setFixedHeight(150);
    this->nodesets->setRootIsDecorated(false);
    this->nodesets->setModel(this->nodeset_model);
    this->nodesets->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->nodesets->setColumnWidth(0, 190);
    this->nodesets->setColumnWidth(2, 40);
    this->nodesets->hideColumn(IDX_COLOR);
    auto * sel_model = this->nodesets->selectionModel();
    connect(sel_model,
            SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this,
            SLOT(onNodeSetSelectionChanged(const QItemSelection &, const QItemSelection &)));

    this->nodesets_expd = new ExpandableWidget("Node sets");
    this->nodesets_expd->setWidget(this->nodesets);
    this->layout->addWidget(this->nodesets_expd);
}

void
InfoWindow::setupSummaryWidgets()
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
    this->layout->addWidget(this->totals_expd);
}

void
InfoWindow::setupRangeWidgets()
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
    connect(this->dimensions, SIGNAL(stateChanged(int)), this, SLOT(onDimensionsStateChanged(int)));

    auto * l = new QVBoxLayout();
    l->setSpacing(8);
    l->setContentsMargins(0, 0, 0, 0);
    l->addWidget(this->range);
    l->addWidget(this->dimensions);

    auto w = new QWidget();
    w->setLayout(l);

    this->range_expd = new ExpandableWidget("Dimensions");
    this->range_expd->setWidget(w);
    this->layout->addWidget(this->range_expd);
}

void
InfoWindow::clear()
{
    this->block_model->removeRows(0, this->block_model->rowCount());
    this->sideset_model->removeRows(0, this->sideset_model->rowCount());
    this->sidesets_expd->setNumberOfItems(0);
    this->nodeset_model->removeRows(0, this->nodeset_model->rowCount());
    this->nodesets_expd->setNumberOfItems(0);

    this->total_elements->setText(1, "");
    this->total_nodes->setText(1, "");
}

void
InfoWindow::onBlockAdded(int id, const QString & name)
{
    int row = this->block_model->rowCount();

    auto * si_name = new QStandardItem();
    si_name->setText(name);
    si_name->setCheckable(true);
    si_name->setCheckState(Qt::Checked);
    si_name->setData(id);
    this->block_model->setItem(row, IDX_NAME, si_name);

    auto * si_clr = new QStandardItem();
    si_clr->setText("\u25a0");
    int clr_idx = row % InfoWindow::colors.length();
    auto color = InfoWindow::colors[clr_idx];
    si_clr->setForeground(QBrush(color));
    si_clr->setData(color);
    this->block_model->setItem(row, IDX_COLOR, si_clr);

    auto * si_id = new QStandardItem();
    si_id->setText(QString::number(id));
    this->block_model->setItem(row, IDX_ID, si_id);
}

void
InfoWindow::onBlockChanged(QStandardItem * item)
{
    if (item->column() == IDX_NAME) {
        bool visible = item->checkState() == Qt::Checked;
        int block_id = item->data().toInt();
        emit blockVisibilityChanged(block_id, visible);
    }
    else if (item->column() == IDX_COLOR) {
        auto color = item->foreground().color();
        auto index = this->block_model->indexFromItem(item);
        auto name_index = index.siblingAtColumn(IDX_NAME);
        int block_id = this->block_model->itemFromIndex(name_index)->data().toInt();
        emit blockColorChanged(block_id, color);
    }
}

void
InfoWindow::setColorPickerColorFromIndex(const QModelIndex & index)
{
    blockSignals(true);
    auto clr_index = index.siblingAtColumn(IDX_COLOR);
    auto * clr_item = this->block_model->itemFromIndex(clr_index);
    auto qcolor = clr_item->data().value<QColor>();
    this->color_picker->setData(clr_index);
    this->color_picker->setColor(qcolor);
    blockSignals(false);
}

void
InfoWindow::onBlockSelectionChanged(const QItemSelection & selected,
                                    const QItemSelection & deselected)
{
    if (selected.indexes().length() > 0) {
        this->sidesets->clearSelection();
        this->nodesets->clearSelection();
        auto index = selected.indexes()[0];
        auto * item = this->block_model->itemFromIndex(index);
        int block_id = item->data().toInt();
        setColorPickerColorFromIndex(index);
        emit blockSelectionChanged(block_id);
    }
    else {
        this->color_picker->setData(QVariant());
        emit blockSelectionChanged(-1);
    }
}

void
InfoWindow::onNameContextMenu(QStandardItem * item, const QPoint & point)
{
    QMenu menu;
    if (item->checkState() == Qt::Checked) {
        menu.addAction("Hide", this, SLOT(onHideBlock()));
        menu.addAction("Hide others", this, SLOT(onHideOtherBlocks()));
        menu.addAction("Hide all", this, SLOT(onHideAllBlocks()));
    }
    else {
        menu.addAction("Show", this, SLOT(onShowBlock()));
        menu.addAction("Show all", this, SLOT(onShowAllBlocks()));
    }
    menu.addSeparator();
    menu.addAction("Appearance...", this, SLOT(onAppearance()));
    menu.exec(point);
}

void
InfoWindow::onBlockCustomContextMenu(const QPoint & point)
{
    auto index = this->blocks->indexAt(point);
    if (index.isValid()) {
        auto * item = this->block_model->itemFromIndex(index);
        onNameContextMenu(item, this->blocks->viewport()->mapToGlobal(point));
    }
}

void
InfoWindow::onDimensionsStateChanged(int state)
{
    emit dimensionsStateChanged(state == Qt::Checked);
}

void
InfoWindow::onHideBlock()
{
    auto * selection_model = this->blocks->selectionModel();
    auto indexes = selection_model->selectedIndexes();
    if (indexes.length() > 0) {
        auto index = indexes[0];
        auto * item = this->block_model->itemFromIndex(index);
        item->setCheckState(Qt::Unchecked);
    }
}

void
InfoWindow::onHideOtherBlocks()
{
    auto * selection_model = this->blocks->selectionModel();
    auto indexes = selection_model->selectedIndexes();
    if (indexes.length() > 0) {
        auto index = indexes[0];
        for (int row = 0; row < this->block_model->rowCount(); row++) {
            if (row != index.row()) {
                auto * item = this->block_model->item(row, 0);
                item->setCheckState(Qt::Unchecked);
            }
        }
    }
}

void
InfoWindow::onHideAllBlocks()
{
    for (int row = 0; row < this->block_model->rowCount(); row++) {
        auto * item = this->block_model->item(row, 0);
        item->setCheckState(Qt::Unchecked);
    }
}

void
InfoWindow::onShowBlock()
{
    auto * selection_model = this->blocks->selectionModel();
    auto indexes = selection_model->selectedIndexes();
    if (indexes.length() > 0) {
        auto index = indexes[0];
        auto * item = this->block_model->itemFromIndex(index);
        item->setCheckState(Qt::Checked);
    }
}

void
InfoWindow::onShowAllBlocks()
{
    for (int row = 0; row < this->block_model->rowCount(); row++) {
        auto * item = this->block_model->item(row, 0);
        item->setCheckState(Qt::Checked);
    }
}

void
InfoWindow::onAppearance()
{
    auto * selection_model = this->blocks->selectionModel();
    auto indexes = selection_model->selectedIndexes();
    if (indexes.length() == 0)
        return;
    auto index = indexes[0];
    setColorPickerColorFromIndex(index);
    this->color_picker->show();
}

void
InfoWindow::onSideSetAdded(int id, const QString & name)
{
    auto row = this->sideset_model->rowCount();

    auto * si_name = new QStandardItem();
    si_name->setText(name);
    si_name->setCheckable(true);
    si_name->setData(id);
    this->sideset_model->setItem(row, IDX_NAME, si_name);

    auto * si_id = new QStandardItem();
    si_id->setText(QString::number(id));
    this->sideset_model->setItem(row, IDX_ID, si_id);

    this->sidesets_expd->setNumberOfItems(this->sideset_model->rowCount());
}

void
InfoWindow::onSideSetChanged(QStandardItem * item)
{
    if (item->column() == IDX_NAME) {
        bool visible = item->checkState() == Qt::Checked;
        auto sideset_id = item->data().toInt();
        emit sideSetVisibilityChanged(sideset_id, visible);
    }
}

void
InfoWindow::onSideSetSelectionChanged(const QItemSelection & selected,
                                      const QItemSelection & deselected)
{
    if (selected.indexes().length() > 0) {
        this->blocks->clearSelection();
        this->nodesets->clearSelection();
        auto index = selected.indexes()[0];
        auto * item = this->sideset_model->itemFromIndex(index);
        auto sideset_id = item->data().toInt();
        emit sideSetSelectionChanged(sideset_id);
    }
    else
        emit sideSetSelectionChanged(-1);
}

void
InfoWindow::onNodeSetAdded(int id, const QString & name)
{
    auto row = this->nodeset_model->rowCount();

    auto * si_name = new QStandardItem();
    si_name->setText(name);
    si_name->setCheckable(true);
    si_name->setData(id);
    this->nodeset_model->setItem(row, IDX_NAME, si_name);

    auto * si_id = new QStandardItem();
    si_id->setText(QString::number(id));
    this->nodeset_model->setItem(row, IDX_ID, si_id);

    this->nodesets_expd->setNumberOfItems(this->nodeset_model->rowCount());
}

void
InfoWindow::onNodeSetChanged(QStandardItem * item)
{
    if (item->column() == IDX_NAME) {
        bool visible = item->checkState() == Qt::Checked;
        auto nodeset_id = item->data().toInt();
        emit nodeSetVisibilityChanged(nodeset_id, visible);
    }
}

void
InfoWindow::onNodeSetSelectionChanged(const QItemSelection & selected,
                                      const QItemSelection & deselected)
{
    if (selected.indexes().length() > 0) {
        this->blocks->clearSelection();
        this->sidesets->clearSelection();
        auto index = selected.indexes()[0];
        auto * item = this->nodeset_model->itemFromIndex(index);
        auto sideset_id = item->data().toInt();
        emit nodeSetSelectionChanged(sideset_id);
    }
    else
        emit nodeSetSelectionChanged(-1);
}

void
InfoWindow::setSummary(int total_elems, int total_nodes)
{
    this->total_elements->setText(1, QLocale::system().toString(total_elems));
    this->total_nodes->setText(1, QLocale::system().toString(total_nodes));
}

void
InfoWindow::setBounds(double xmin, double xmax, double ymin, double ymax, double zmin, double zmax)
{
    char format = 'f';
    int precision = 5;
    auto x_range = QString("%1 to %2")
                       .arg(QString::number(xmin, format, precision))
                       .arg(QString::number(xmax, format, precision));
    this->x_range->setText(1, x_range);
    auto y_range = QString("%1 to %2")
                       .arg(QString::number(ymin, format, precision))
                       .arg(QString::number(ymax, format, precision));
    this->y_range->setText(1, y_range);
    auto z_range = QString("%1 to %2")
                       .arg(QString::number(zmin, format, precision))
                       .arg(QString::number(zmax, format, precision));
    this->z_range->setText(1, z_range);
}

void
InfoWindow::onBlockColorPicked(const QColor & qcolor)
{
    QVariant data = this->color_picker->data();
    if (data.isValid()) {
        auto index = data.value<QModelIndex>();
        auto * item = this->block_model->itemFromIndex(index);
        item->setForeground(QBrush(qcolor));
        item->setData(qcolor);
    }
}

void
InfoWindow::onBlockOpacityChanged(double opacity)
{
    QVariant data = this->color_picker->data();
    if (data.isValid()) {
        auto clr_index = data.value<QModelIndex>();
        auto * clr_item = this->block_model->itemFromIndex(clr_index);
        auto color = clr_item->foreground().color();
        color.setAlphaF(opacity);
        clr_item->setForeground(QBrush(color));
        clr_item->setData(color);

        auto index = clr_index.siblingAtColumn(IDX_NAME);
        auto * item = this->block_model->itemFromIndex(index);
        auto block_id = item->data().toInt();
        emit blockOpacityChanged(block_id, opacity);
    }
}
