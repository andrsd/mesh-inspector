#include "selecttool.h"
#include "mainwindow.h"
#include <QShortcut>
#include <QLocale>
#include <QMenu>
#include <QAction>
#include <QActionGroup>
#include <QSettings>
#include "common/infowidget.h"
#include "blockobject.h"
#include "sidesetobject.h"
#include "nodesetobject.h"
#include "selection.h"
#include "vtkPropPicker.h"
#include "vtkCellPicker.h"
#include "vtkPointPicker.h"
#include "vtkActor.h"
#include "vtkUnstructuredGrid.h"
#include "vtkRenderer.h"
#include "vtkProperty.h"
#include "vtkAlgorithmOutput.h"

QColor SelectTool::SELECTION_CLR = QColor(255, 173, 79);
QColor SelectTool::SELECTION_EDGE_CLR = QColor(179, 95, 0);
QColor SelectTool::HIGHLIGHT_CLR = QColor(255, 211, 79);

namespace {

QString
cellTypeToName(int cell_type)
{
    // clang-format off
    switch (cell_type) {
        case 3: return "Edge2";
        case 5: return "Tri3";
        case 9: return "Quad4";
        case 10: return "Tetra4";
        case 12: return "Hex8";
        case 13: return "Prism6";
        case 14: return "Pyramid5";
        case 21: return "Edge3";
        case 22: return "Tri6";
        case 23: return "Quad9";
        case 24: return "Tetra10";
        case 25: return "Hex27";
        case 26: return "Prism18";
        case 27: return "Pyramid14";
        default: return QString::number(cell_type);
    }
    // clang-format on
}

} // namespace

SelectTool::SelectTool(MainWindow * main_wnd) :
    main_window(main_wnd),
    selected_mesh_ent_info(nullptr),
    deselect_sc(nullptr),
    mode_select_action_group(nullptr),
    select_mode(MODE_SELECT_NONE),
    selection(nullptr),
    selected_block(nullptr),
    highlight(nullptr),
    highlighted_block(nullptr)
{
}

SelectTool::~SelectTool()
{
    delete this->selected_mesh_ent_info;
    delete this->deselect_sc;
    delete this->mode_select_action_group;
}

const BlockObject *
SelectTool::getSelectedBlock() const
{
    return this->selected_block;
}

void
SelectTool::setupWidgets()
{
    this->selected_mesh_ent_info = new InfoWidget(this->main_window);
    this->selected_mesh_ent_info->setVisible(false);

    this->deselect_sc = new QShortcut(QKeySequence(Qt::Key_Space), this->main_window);
    connect(this->deselect_sc, &QShortcut::activated, this, &SelectTool::onDeselect);
}

void
SelectTool::setupMenu(QMenu * menu)
{
    auto settings = this->main_window->getSettings();
    QMenu * select_menu = menu->addMenu("Select mode");
    this->mode_select_action_group = new QActionGroup(this);
    this->select_mode =
        static_cast<EModeSelect>(settings->value("tools/select_mode", MODE_SELECT_NONE).toInt());

    QList<QString> names(
        { QString("None"), QString("Blocks"), QString("Cells"), QString("Points") });
    QList<EModeSelect> modes(
        { MODE_SELECT_NONE, MODE_SELECT_BLOCKS, MODE_SELECT_CELLS, MODE_SELECT_POINTS });
    for (int i = 0; i < names.size(); i++) {
        auto name = names[i];
        auto mode = modes[i];
        auto * action = select_menu->addAction(name);
        action->setCheckable(true);
        action->setData(mode);
        this->mode_select_action_group->addAction(action);
        if (mode == this->select_mode)
            action->setChecked(true);
    }

    connect(this->mode_select_action_group,
            &QActionGroup::triggered,
            this,
            &SelectTool::onSelectModeTriggered);
}

void
SelectTool::loadIntoVtk(vtkAlgorithmOutput * output_port)
{
    delete this->selection;
    this->selection = new Selection(output_port);
    setSelectionProperties();
    auto renderer = this->main_window->getRenderer();
    renderer->AddActor(this->selection->getActor());

    delete this->highlight;
    this->highlight = new Selection(output_port);
    setHighlightProperties();
    renderer->AddActor(this->highlight->getActor());
}

void
SelectTool::onDeselect()
{
    deselectBlocks();
    hideSelectedMeshEntity();
    if (this->selection)
        this->selection->clear();
}

void
SelectTool::showSelectedMeshEntity(const QString & info)
{
    this->selected_mesh_ent_info->setText(info);
    this->selected_mesh_ent_info->adjustSize();
    auto tl = this->main_window->getView()->geometry().topLeft();
    this->selected_mesh_ent_info->move(tl.x() + 10, tl.y() + 10);
    this->selected_mesh_ent_info->show();
}

void
SelectTool::hideSelectedMeshEntity()
{
    this->selected_mesh_ent_info->hide();
}

void
SelectTool::deselectBlocks()
{
    if (this->selected_block != nullptr) {
        this->main_window->setBlockProperties(this->selected_block, false);
        this->selected_block = nullptr;
    }
}

void
SelectTool::onBlockSelectionChanged(int block_id)
{
    deselectBlocks();
    auto blocks = this->main_window->getBlocks();
    const auto & it = blocks.find(block_id);
    if (it != blocks.end()) {
        BlockObject * block = it->second;
        auto info = QString("Block: %1\n"
                            "Cells: %2\n"
                            "Points: %3")
                        .arg(block_id)
                        .arg(QLocale::system().toString(block->getNumCells()))
                        .arg(QLocale::system().toString(block->getNumPoints()));
        showSelectedMeshEntity(info);
    }
    else
        hideSelectedMeshEntity();
}

void
SelectTool::onSideSetSelectionChanged(int sideset_id)
{
    auto side_sets = this->main_window->getSideSets();
    const auto & it = side_sets.find(sideset_id);
    if (it != side_sets.end()) {
        auto * sideset = it->second;
        auto info = QString("Side set: %1\n"
                            "Cells: %2\n"
                            "Points: %3")
                        .arg(sideset_id)
                        .arg(QLocale::system().toString(sideset->getNumCells()))
                        .arg(QLocale::system().toString(sideset->getNumPoints()));
        showSelectedMeshEntity(info);
    }
    else
        hideSelectedMeshEntity();
}

void
SelectTool::onNodeSetSelectionChanged(int nodeset_id)
{
    auto node_sets = this->main_window->getNodeSets();
    const auto & it = node_sets.find(nodeset_id);
    if (it != node_sets.end()) {
        auto * nodeset = it->second;
        auto info = QString("Node set: %1\n"
                            "Points: %2")
                        .arg(nodeset_id)
                        .arg(QLocale::system().toString(nodeset->getNumPoints()));
        showSelectedMeshEntity(info);
    }
    else
        hideSelectedMeshEntity();
}

void
SelectTool::onSelectModeTriggered(QAction * action)
{
    action->setChecked(true);
    this->select_mode = static_cast<EModeSelect>(action->data().toInt());
    if (this->select_mode == MODE_SELECT_NONE) {
        deselectBlocks();
        onDeselect();
        if (this->highlight)
            this->highlight->clear();
    }
}

void
SelectTool::clear()
{
    if (this->selection) {
        delete this->selection;
        this->selection = nullptr;
    }
    if (this->highlight) {
        delete this->highlight;
        this->highlight = nullptr;
    }
}

void
SelectTool::selectBlock(const QPoint & pt)
{
    auto * picker = vtkPropPicker::New();
    if (picker->PickProp(pt.x(), pt.y(), this->main_window->getRenderer())) {
        auto * actor = dynamic_cast<vtkActor *>(picker->GetViewProp());
        if (actor) {
            auto blk_id = this->main_window->blockActorToId(actor);
            auto * block = this->main_window->getBlock(blk_id);
            onBlockSelectionChanged(blk_id);
            this->selected_block = block;
            auto highlighted = this->selected_block == this->highlighted_block;
            this->main_window->setBlockProperties(block, true, highlighted);
        }
    }
    picker->Delete();
}

void
SelectTool::selectCell(const QPoint & pt)
{
    auto * picker = vtkCellPicker::New();
    if (picker->Pick(pt.x(), pt.y(), 0, this->main_window->getRenderer())) {
        auto cell_id = picker->GetCellId();
        this->selection->selectCell(cell_id);
        setSelectionProperties();

        auto * unstr_grid = this->selection->getSelected();
        auto * cell = unstr_grid->GetCell(0);
        auto cell_type = cell->GetCellType();
        QString nfo =
            QString("Element ID: %1\nType: %2").arg(cell_id).arg(cellTypeToName(cell_type));
        showSelectedMeshEntity(nfo);
    }
    picker->Delete();
}

void
SelectTool::selectPoint(const QPoint & pt)
{
    auto * picker = vtkPointPicker::New();
    if (picker->Pick(pt.x(), pt.y(), 0, this->main_window->getRenderer())) {
        auto point_id = picker->GetPointId();
        this->selection->selectPoint(point_id);
        setSelectionProperties();

        auto * unstr_grid = this->selection->getSelected();
        auto * points = unstr_grid->GetPoints();
        if (points) {
            double * coords = points->GetPoint(0);
            char format = 'f';
            int precision = 5;
            QString nfo = QString("Node ID: %1\nX: %2\nY: %3\nZ: %4")
                              .arg(point_id)
                              .arg(QString::number(coords[0], format, precision))
                              .arg(QString::number(coords[1], format, precision))
                              .arg(QString::number(coords[2], format, precision));
            showSelectedMeshEntity(nfo);
        }
        else {
            QString nfo = QString("Node ID: %1").arg(point_id);
            showSelectedMeshEntity(nfo);
        }
    }
    picker->Delete();
}

void
SelectTool::setSelectionProperties()
{
    auto * actor = this->selection->getActor();
    auto * property = actor->GetProperty();
    if (this->select_mode == MODE_SELECT_POINTS) {
        property->SetRepresentationToPoints();
        property->SetRenderPointsAsSpheres(true);
        property->SetVertexVisibility(true);
        property->SetEdgeVisibility(false);
        property->SetPointSize(this->main_window->HIDPI(7.5));
        property->SetColor(SELECTION_CLR.redF(), SELECTION_CLR.greenF(), SELECTION_CLR.blueF());
        property->SetOpacity(1);
        property->SetAmbient(1);
        property->SetDiffuse(0);
    }
    else if (this->select_mode == MODE_SELECT_CELLS) {
        property->SetRepresentationToSurface();
        property->SetRenderPointsAsSpheres(false);
        property->SetVertexVisibility(false);
        property->EdgeVisibilityOn();
        property->SetColor(SELECTION_CLR.redF(), SELECTION_CLR.greenF(), SELECTION_CLR.blueF());
        property->SetLineWidth(this->main_window->HIDPI(3.5));
        property->SetEdgeColor(SELECTION_EDGE_CLR.redF(),
                               SELECTION_EDGE_CLR.greenF(),
                               SELECTION_EDGE_CLR.blueF());
        property->SetOpacity(0.5);
        property->SetAmbient(1);
        property->SetDiffuse(0);
    }
}

void
SelectTool::highlightBlock(const QPoint & pt)
{
    if (this->highlighted_block) {
        auto selected = this->highlighted_block == this->selected_block;
        this->main_window->setBlockProperties(this->highlighted_block, selected, false);
    }

    auto * picker = vtkPropPicker::New();
    if (picker->PickProp(pt.x(), pt.y(), this->main_window->getRenderer())) {
        auto * actor = dynamic_cast<vtkActor *>(picker->GetViewProp());
        if (actor) {
            auto blk_id = this->main_window->blockActorToId(actor);
            auto * block = this->main_window->getBlock(blk_id);
            if (block) {
                this->highlighted_block = block;
                auto selected = this->highlighted_block == this->selected_block;
                this->main_window->setBlockProperties(block, selected, true);
            }
        }
    }
    else if (this->highlighted_block) {
        this->highlighted_block = nullptr;
    }
    picker->Delete();
}

void
SelectTool::highlightCell(const QPoint & pt)
{
    auto * picker = vtkCellPicker::New();
    if (picker->Pick(pt.x(), pt.y(), 0, this->main_window->getRenderer())) {
        auto cell_id = picker->GetCellId();
        this->highlight->selectCell(cell_id);
        setHighlightProperties();
    }
    else
        this->highlight->clear();
    picker->Delete();
}

void
SelectTool::highlightPoint(const QPoint & pt)
{
    auto * picker = vtkPointPicker::New();
    if (picker->Pick(pt.x(), pt.y(), 0, this->main_window->getRenderer())) {
        auto point_id = picker->GetPointId();
        this->highlight->selectPoint(point_id);
        setHighlightProperties();
    }
    else
        this->highlight->clear();
    picker->Delete();
}

void
SelectTool::setHighlightProperties()
{
    auto * actor = this->highlight->getActor();
    auto * property = actor->GetProperty();
    if (this->select_mode == MODE_SELECT_POINTS) {
        property->SetRepresentationToPoints();
        property->SetRenderPointsAsSpheres(true);
        property->SetVertexVisibility(true);
        property->SetEdgeVisibility(false);
        property->SetPointSize(this->main_window->HIDPI(7.5));
        property->SetColor(HIGHLIGHT_CLR.redF(), HIGHLIGHT_CLR.greenF(), HIGHLIGHT_CLR.blueF());
        property->SetOpacity(1);
        property->SetAmbient(1);
        property->SetDiffuse(0);
    }
    else if (this->select_mode == MODE_SELECT_CELLS) {
        property->SetRepresentationToSurface();
        property->SetRenderPointsAsSpheres(false);
        property->SetVertexVisibility(false);
        property->EdgeVisibilityOn();
        property->SetColor(HIGHLIGHT_CLR.redF(), HIGHLIGHT_CLR.greenF(), HIGHLIGHT_CLR.blueF());
        property->SetLineWidth(this->main_window->HIDPI(3.5));
        property->SetEdgeColor(HIGHLIGHT_CLR.redF(), HIGHLIGHT_CLR.greenF(), HIGHLIGHT_CLR.blueF());
        property->SetOpacity(0.33);
        property->SetAmbient(1);
        property->SetDiffuse(0);
    }
}

void
SelectTool::saveSettings(QSettings * settings)
{
    settings->setValue("tools/select_mode", this->select_mode);
}

void
SelectTool::onClicked(const QPoint & pt)
{
    onDeselect();
    if (this->select_mode == MODE_SELECT_BLOCKS)
        selectBlock(pt);
    else if (this->select_mode == MODE_SELECT_CELLS)
        selectCell(pt);
    else if (this->select_mode == MODE_SELECT_POINTS)
        selectPoint(pt);
}

void
SelectTool::onMouseMove(const QPoint & pt)
{
    if (this->select_mode == MODE_SELECT_BLOCKS)
        highlightBlock(pt);
    else if (this->select_mode == MODE_SELECT_CELLS)
        highlightCell(pt);
    else if (this->select_mode == MODE_SELECT_POINTS)
        highlightPoint(pt);
}
