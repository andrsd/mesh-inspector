#include "infowindow.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QStandardItemModel>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QCheckBox>
#include "common/horzline.h"
#include "common/expandablewidget.h"
#include "common/otreeview.h"

InfoWindow::InfoWindow(QWidget * parent) :
    widget(nullptr),
    layout(nullptr),
    lbl_info(nullptr),
    block_model(nullptr),
    blocks(nullptr),
    totals(nullptr),
    total_elements(nullptr),
    totals_expd(nullptr),
    range(nullptr),
    x_range(nullptr),
    y_range(nullptr),
    z_range(nullptr),
    dimensions(nullptr),
    range_expd(nullptr)
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
    this->layout->setSpacing(8);

    // this->color_picker = new ColorPicker(self);
    // this->color_picker.colorChanged.connect(self.onBlockColorPicked);

    setupBlocksWidgets();
    this->layout->addWidget(new HorzLine());
    // this->setupSidesetsWidgets();
    // this->layout.addWidget(HLine());
    // this->setupNodesetsWidgets();
    // this->layout.addWidget(HLine());
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
    // this->block_model->itemChanged.connect(self.onBlockChanged)
    this->blocks = new OTreeView();
    this->blocks->setFixedHeight(250);
    this->blocks->setRootIsDecorated(false);
    this->blocks->setModel(this->block_model);
    this->blocks->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->blocks->setColumnWidth(0, 170);
    this->blocks->setColumnWidth(1, 20);
    this->blocks->setColumnWidth(2, 40);
    this->blocks->setContextMenuPolicy(Qt::CustomContextMenu);
    // this->blocks.customContextMenuRequested.connect(self.onBlockCustomContextMenu)
    auto * sel_model = this->blocks->selectionModel();
    // sel_model.selectionChanged.connect(self.onBlockSelectionChanged)
    this->layout->addWidget(this->blocks);
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
    // this->dimensions->stateChanged.connect(self.onDimensionsStateChanged)

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
