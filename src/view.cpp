#include "view.h"
#include "mainwindow.h"
#include "model.h"
#include <QMenu>
#include <QPushButton>
#include <QAction>
#include <QActionGroup>
#include "vtkRenderer.h"
#include "vtkOrientationMarkerWidget.h"
#include "vtkCamera.h"
#include "vtkProperty.h"
#include "vtkGenericOpenGLRenderWindow.h"
#include "vtkAxesActor.h"
#include "vtkCubeAxesActor.h"
#include "vtkCaptionActor2D.h"
#include "vtkTextProperty.h"
#include "blockobject.h"
#include "sidesetobject.h"
#include "nodesetobject.h"
#include "selecttool.h"
#include "ointeractorstyle2d.h"
#include "ointeractorstyle3d.h"
#include "colorprofile.h"

QColor View::SIDESET_CLR = QColor(255, 173, 79);
QColor View::SIDESET_EDGE_CLR = QColor(26, 26, 102);
QColor View::NODESET_CLR = QColor(168, 91, 2);

float View::EDGE_WIDTH = 1;
float View::OUTLINE_WIDTH = 1.5;

View::View(MainWindow * main_wnd) :
    main_window(main_wnd),
    model(main_wnd->getModel()),
    view_menu(nullptr),
    view_mode(nullptr),
    shaded_action(nullptr),
    shaded_w_edges_action(nullptr),
    hidden_edges_removed_action(nullptr),
    transluent_action(nullptr),
    render_mode(SHADED_WITH_EDGES),
    visual_repr(nullptr),
    perspective_action(nullptr),
    ori_marker_action(nullptr),
    ori_marker(nullptr),
    render_window(vtkGenericOpenGLRenderWindow::New()),
    renderer(vtkRenderer::New()),
    interactor(nullptr),
    interactor_style_2d(new OInteractorStyle2D(this->main_window)),
    interactor_style_3d(new OInteractorStyle3D(this->main_window)),
    cube_axes_actor(nullptr)
{
    this->setRenderWindow(this->render_window);
    this->render_window->AddRenderer(this->renderer);
}

View::~View()
{
    delete this->view_menu;
    delete this->view_mode;
    this->render_window->Delete();
    this->renderer->Delete();
}

vtkCamera *
View::getActiveCamera()
{
    return this->renderer->GetActiveCamera();
}

void
View::setupWidgets()
{
    setupViewModeWidget();
}

void
View::setupViewModeWidget()
{
    this->view_menu = new QMenu();
    this->shaded_action = this->view_menu->addAction("Shaded");
    this->shaded_action->setCheckable(true);
    this->shaded_action->setShortcut(QKeySequence("Ctrl+1"));
    this->shaded_w_edges_action = this->view_menu->addAction("Shaded with edges");
    this->shaded_w_edges_action->setCheckable(true);
    this->shaded_w_edges_action->setShortcut(QKeySequence("Ctrl+2"));
    this->hidden_edges_removed_action = this->view_menu->addAction("Hidden edges removed");
    this->hidden_edges_removed_action->setCheckable(true);
    this->hidden_edges_removed_action->setShortcut(QKeySequence("Ctrl+3"));
    this->transluent_action = this->view_menu->addAction("Transluent");
    this->transluent_action->setCheckable(true);
    this->transluent_action->setShortcut(QKeySequence("Ctrl+4"));
    this->shaded_w_edges_action->setChecked(true);
    this->render_mode = SHADED_WITH_EDGES;

    this->visual_repr = new QActionGroup(this->view_menu);
    this->visual_repr->addAction(this->shaded_action);
    this->visual_repr->addAction(this->shaded_w_edges_action);
    this->visual_repr->addAction(this->hidden_edges_removed_action);
    this->visual_repr->addAction(this->transluent_action);
    this->visual_repr->setExclusive(true);

    this->view_menu->addSeparator();
    this->perspective_action = this->view_menu->addAction("Perspective");
    this->perspective_action->setCheckable(true);
    this->perspective_action->setChecked(true);

    this->view_menu->addSeparator();
    this->ori_marker_action = this->view_menu->addAction("Orientation marker");
    this->ori_marker_action->setCheckable(true);
    this->ori_marker_action->setChecked(true);

    connect(this->shaded_action, &QAction::triggered, this, &View::onShadedTriggered);
    connect(this->shaded_w_edges_action,
            &QAction::triggered,
            this,
            &View::onShadedWithEdgesTriggered);
    connect(this->hidden_edges_removed_action,
            &QAction::triggered,
            this,
            &View::onHiddenEdgesRemovedTriggered);
    connect(this->transluent_action, &QAction::triggered, this, &View::onTransluentTriggered);
    connect(this->perspective_action, &QAction::toggled, this, &View::onPerspectiveToggled);
    connect(this->ori_marker_action,
            &QAction::toggled,
            this,
            &View::onOrientationMarkerVisibilityChanged);

    this->view_mode = new QPushButton(this);
    this->view_mode->setFixedSize(60, 32);
    this->view_mode->setIcon(QPixmap(":/resources/render-mode.svg"));
    this->view_mode->setMenu(this->view_menu);
    this->view_mode->show();
}

void
View::setMenu(QMenu * menu)
{
    menu->addAction(this->shaded_action);
    menu->addAction(this->shaded_w_edges_action);
    menu->addAction(this->hidden_edges_removed_action);
    menu->addAction(this->transluent_action);
}

void
View::updateMenuBar(bool enabled)
{
    this->visual_repr->setEnabled(enabled);
}

void
View::setupVtk()
{
    this->interactor = this->render_window->GetInteractor();

    // TODO: set background from preferences/templates
    this->renderer->SetGradientBackground(true);
    // set anti-aliasing on
    this->renderer->SetUseFXAA(true);
    this->render_window->SetMultiSamples(1);

    setupOrientationMarker();
    setupCubeAxesActor();
}

void
View::clear()
{
    this->renderer->RemoveAllViewProps();
}

void
View::addBlock(BlockObject * block)
{
    setBlockProperties(block);
    this->renderer->AddViewProp(block->getActor());
    this->renderer->AddViewProp(block->getSilhouetteActor());
}

void
View::addSideSet(SideSetObject * sideset)
{
    setSideSetProperties(sideset);
    this->renderer->AddViewProp(sideset->getActor());
}

void
View::addNodeSet(NodeSetObject * nodeset)
{
    this->setNodeSetProperties(nodeset);
    this->renderer->AddViewProp(nodeset->getActor());
}

void
View::onShadedTriggered(bool checked)
{
    this->render_mode = SHADED;
    auto blocks = this->model->getBlocks();
    for (auto & it : blocks) {
        auto * block = it.second;
        bool selected = this->main_window->getSelectedBlock() == block;
        setBlockProperties(block, selected);
        block->setSilhouetteVisible(false);
    }
    auto side_sets = this->model->getSideSets();
    for (auto & it : side_sets) {
        auto * sideset = it.second;
        setSideSetProperties(sideset);
    }
}

void
View::onShadedWithEdgesTriggered(bool checked)
{
    this->render_mode = SHADED_WITH_EDGES;
    auto blocks = this->model->getBlocks();
    for (auto & it : blocks) {
        auto * block = it.second;
        bool selected = this->main_window->getSelectedBlock() == block;
        setBlockProperties(block, selected);
        block->setSilhouetteVisible(false);
    }
    auto side_sets = this->model->getSideSets();
    for (auto & it : side_sets) {
        auto * sideset = it.second;
        setSideSetProperties(sideset);
    }
}

void
View::onHiddenEdgesRemovedTriggered(bool checked)
{
    this->render_mode = HIDDEN_EDGES_REMOVED;
    auto blocks = this->model->getBlocks();
    for (auto & it : blocks) {
        auto * block = it.second;
        bool selected = this->main_window->getSelectedBlock() == block;
        setBlockProperties(block, selected);
        block->setSilhouetteVisible(block->visible());
    }
    auto side_sets = this->model->getSideSets();
    for (auto & it : side_sets) {
        auto * sideset = it.second;
        setSideSetProperties(sideset);
    }
}

void
View::onTransluentTriggered(bool checked)
{
    this->render_mode = TRANSLUENT;
    auto blocks = this->model->getBlocks();
    for (auto & it : blocks) {
        auto * block = it.second;
        bool selected = this->main_window->getSelectedBlock() == block;
        setBlockProperties(block, selected);
        block->setSilhouetteVisible(block->visible());
    }
    auto side_sets = this->model->getSideSets();
    for (auto & it : side_sets) {
        auto * sideset = it.second;
        setSideSetProperties(sideset);
    }
}

void
View::onPerspectiveToggled(bool checked)
{
    auto renderer = this->renderer;
    if (checked) {
        auto * camera = renderer->GetActiveCamera();
        camera->ParallelProjectionOff();
    }
    else {
        auto * camera = renderer->GetActiveCamera();
        camera->ParallelProjectionOn();
    }
}

void
View::onOrientationMarkerVisibilityChanged(bool visible)
{
    if (visible)
        this->ori_marker->EnabledOn();
    else
        this->ori_marker->EnabledOff();
}

void
View::setSelectedBlockProperties(BlockObject * block, bool highlighted)
{
    auto * property = block->getProperty();
    if (this->render_mode == SHADED) {
        property->SetColor(SIDESET_CLR.redF(), SIDESET_CLR.greenF(), SIDESET_CLR.blueF());
        property->SetOpacity(1.0);
        property->SetEdgeVisibility(false);
    }
    else if (this->render_mode == SHADED_WITH_EDGES) {
        property->SetColor(SIDESET_CLR.redF(), SIDESET_CLR.greenF(), SIDESET_CLR.blueF());
        property->SetOpacity(1.0);
        property->SetEdgeVisibility(true);
        property->SetEdgeColor(SIDESET_EDGE_CLR.redF(),
                               SIDESET_EDGE_CLR.greenF(),
                               SIDESET_EDGE_CLR.blueF());
        property->SetLineWidth(this->main_window->HIDPI(EDGE_WIDTH));
    }
    else if (this->render_mode == HIDDEN_EDGES_REMOVED) {
        property->SetColor(SIDESET_CLR.redF(), SIDESET_CLR.greenF(), SIDESET_CLR.blueF());
        property->SetOpacity(1.0);
        property->SetEdgeVisibility(false);
    }
    else if (this->render_mode == TRANSLUENT) {
        property->SetColor(SIDESET_CLR.redF(), SIDESET_CLR.greenF(), SIDESET_CLR.blueF());
        property->SetOpacity(0.33);
        property->SetEdgeVisibility(false);
    }

    setHighlightedBlockProperties(block, highlighted);
}

void
View::setDeselectedBlockProperties(BlockObject * block, bool highlighted)
{
    auto * property = block->getProperty();
    if (this->render_mode == SHADED) {
        auto & clr = block->getColor();
        property->SetColor(clr.redF(), clr.greenF(), clr.blueF());
        property->SetOpacity(block->getOpacity());
        property->SetEdgeVisibility(false);
    }
    else if (this->render_mode == SHADED_WITH_EDGES) {
        auto & clr = block->getColor();
        property->SetColor(clr.redF(), clr.greenF(), clr.blueF());
        property->SetOpacity(block->getOpacity());
        property->SetEdgeVisibility(true);
        property->SetEdgeColor(SIDESET_EDGE_CLR.redF(),
                               SIDESET_EDGE_CLR.greenF(),
                               SIDESET_EDGE_CLR.blueF());
        property->SetLineWidth(this->main_window->HIDPI(EDGE_WIDTH));
    }
    else if (this->render_mode == HIDDEN_EDGES_REMOVED) {
        property->SetColor(1., 1., 1.);
        property->SetOpacity(1.0);
        property->SetEdgeVisibility(false);
    }
    else if (this->render_mode == TRANSLUENT) {
        auto & clr = block->getColor();
        property->SetColor(clr.redF(), clr.greenF(), clr.blueF());
        property->SetOpacity(0.33);
        property->SetEdgeVisibility(false);
    }

    setHighlightedBlockProperties(block, highlighted);
}

void
View::setHighlightedBlockProperties(BlockObject * block, bool highlighted)
{
    auto * property = block->getSilhouetteProperty();
    if (highlighted) {
        block->setSilhouetteVisible(true);

        if (this->render_mode == SHADED) {
            property->SetColor(SelectTool::HIGHLIGHT_CLR.redF(),
                               SelectTool::HIGHLIGHT_CLR.greenF(),
                               SelectTool::HIGHLIGHT_CLR.blueF());
            property->SetLineWidth(this->main_window->HIDPI(OUTLINE_WIDTH));
        }
        else if (this->render_mode == SHADED_WITH_EDGES) {
            property->SetColor(SelectTool::HIGHLIGHT_CLR.redF(),
                               SelectTool::HIGHLIGHT_CLR.greenF(),
                               SelectTool::HIGHLIGHT_CLR.blueF());
            property->SetLineWidth(this->main_window->HIDPI(OUTLINE_WIDTH));
        }
        else if (this->render_mode == HIDDEN_EDGES_REMOVED) {
            property->SetColor(SelectTool::HIGHLIGHT_CLR.redF(),
                               SelectTool::HIGHLIGHT_CLR.greenF(),
                               SelectTool::HIGHLIGHT_CLR.blueF());
            property->SetLineWidth(this->main_window->HIDPI(OUTLINE_WIDTH));
        }
        else if (this->render_mode == TRANSLUENT) {
            property->SetColor(SelectTool::HIGHLIGHT_CLR.redF(),
                               SelectTool::HIGHLIGHT_CLR.greenF(),
                               SelectTool::HIGHLIGHT_CLR.blueF());
            property->SetLineWidth(this->main_window->HIDPI(OUTLINE_WIDTH));
        }
    }
    else {
        if (this->render_mode == SHADED) {
            block->setSilhouetteVisible(false);
        }
        else if (this->render_mode == SHADED_WITH_EDGES) {
            block->setSilhouetteVisible(false);
        }
        else if (this->render_mode == HIDDEN_EDGES_REMOVED) {
            block->setSilhouetteVisible(true);
            property->SetColor(0, 0, 0);
            property->SetLineWidth(this->main_window->HIDPI(OUTLINE_WIDTH));
        }
        else if (this->render_mode == TRANSLUENT) {
            block->setSilhouetteVisible(true);
            property->SetColor(0, 0, 0);
            property->SetLineWidth(this->main_window->HIDPI(OUTLINE_WIDTH));
        }
    }
}

void
View::setBlockProperties(BlockObject * block, bool selected, bool highlighted)
{
    auto * property = block->getProperty();
    property->SetRepresentationToSurface();
    property->SetAmbient(0.4);
    property->SetDiffuse(0.6);
    if (selected)
        this->setSelectedBlockProperties(block, highlighted);
    else
        this->setDeselectedBlockProperties(block, highlighted);
}

void
View::setSideSetProperties(SideSetObject * sideset)
{
    auto * property = sideset->getProperty();
    property->SetColor(SIDESET_CLR.redF(), SIDESET_CLR.greenF(), SIDESET_CLR.blueF());
    property->SetEdgeVisibility(false);
    property->LightingOff();
    if (this->render_mode == SHADED || this->render_mode == SHADED_WITH_EDGES) {
        property->SetEdgeColor(SIDESET_EDGE_CLR.redF(),
                               SIDESET_EDGE_CLR.greenF(),
                               SIDESET_EDGE_CLR.blueF());
        if (this->render_mode == SHADED_WITH_EDGES) {
            property->SetLineWidth(this->main_window->HIDPI(EDGE_WIDTH));
            property->SetEdgeVisibility(true);
        }
    }
}

void
View::setNodeSetProperties(NodeSetObject * nodeset)
{
    auto * property = nodeset->getProperty();
    property->SetRepresentationToPoints();
    property->SetRenderPointsAsSpheres(true);
    property->SetVertexVisibility(true);
    property->SetEdgeVisibility(false);
    property->SetPointSize(this->main_window->HIDPI(5));
    property->SetColor(NODESET_CLR.redF(), NODESET_CLR.greenF(), NODESET_CLR.blueF());
    property->SetOpacity(1);
    property->SetAmbient(1);
    property->SetDiffuse(0);
}

void
View::setupOrientationMarker()
{
    std::array<QColor, 3> clr({ QColor(188, 39, 26), QColor(65, 147, 41), QColor(0, 0, 200) });

    vtkAxesActor * axes = vtkAxesActor::New();
    axes->SetNormalizedTipLength(0, 0, 0);

    std::array<vtkProperty *, 3> shaft_property({ axes->GetXAxisShaftProperty(),
                                                  axes->GetYAxisShaftProperty(),
                                                  axes->GetZAxisShaftProperty() });
    for (std::size_t i = 0; i < shaft_property.size(); i++) {
        auto prop = shaft_property[i];
        prop->SetLineWidth(this->main_window->HIDPI(1));
        prop->SetColor(clr[i].redF(), clr[i].greenF(), clr[i].blueF());
    }

    std::array<vtkCaptionActor2D *, 3> caption_actors({ axes->GetXAxisCaptionActor2D(),
                                                        axes->GetYAxisCaptionActor2D(),
                                                        axes->GetZAxisCaptionActor2D() });
    for (std::size_t i = 0; i < caption_actors.size(); i++) {
        auto text_prop = caption_actors[i]->GetCaptionTextProperty();
        text_prop->SetColor(clr[i].redF(), clr[i].greenF(), clr[i].blueF());
        text_prop->BoldOn();
        text_prop->ItalicOff();
        text_prop->ShadowOff();
    }

    this->ori_marker = vtkOrientationMarkerWidget::New();
    this->ori_marker->SetDefaultRenderer(this->renderer);
    this->ori_marker->SetOrientationMarker(axes);
    this->ori_marker->SetViewport(0.8, 0, 1.0, 0.2);
    this->ori_marker->SetInteractor(this->interactor);
    this->ori_marker->SetEnabled(true);
    this->ori_marker->SetInteractive(false);
}

void
View::setColorProfile(ColorProfile * profile)
{
    const auto & qclr = profile->getColor("bkgnd");
    double bkgnd[3] = { qclr.redF(), qclr.greenF(), qclr.blueF() };

    this->renderer->SetBackground(bkgnd);
    this->renderer->SetBackground2(bkgnd);
}

void
View::setInteractorStyle(int dim)
{
    if (dim == 3)
        this->interactor->SetInteractorStyle(this->interactor_style_3d);
    else
        this->interactor->SetInteractorStyle(this->interactor_style_2d);
}

void
View::resetCamera()
{
    auto * camera = this->renderer->GetActiveCamera();
    auto * focal_point = camera->GetFocalPoint();
    camera->SetPosition(focal_point[0], focal_point[1], 1);
    camera->SetRoll(0);
    this->renderer->ResetCamera();
}

void
View::setBlockVisibility(int block_id, bool visible)
{
    BlockObject * block = this->model->getBlock(block_id);
    if (block) {
        block->setVisible(visible);
        if (this->render_mode == HIDDEN_EDGES_REMOVED || this->render_mode == TRANSLUENT)
            block->setSilhouetteVisible(visible);
        else
            block->setSilhouetteVisible(false);
    }
}

void
View::setBlockOpacity(int block_id, double opacity)
{
    BlockObject * block = this->model->getBlock(block_id);
    if (block) {
        block->setOpacity(opacity);
        if (this->render_mode == SHADED || this->render_mode == SHADED_WITH_EDGES) {
            auto * property = block->getProperty();
            property->SetOpacity(opacity);
        }
    }
}

void
View::setBlockColor(int block_id, QColor color)
{
    BlockObject * block = this->model->getBlock(block_id);
    if (block) {
        block->setColor(color);
        auto * property = block->getProperty();
        if (this->render_mode == HIDDEN_EDGES_REMOVED)
            property->SetColor(1, 1, 1);
        else
            property->SetColor(color.redF(), color.greenF(), color.blueF());
    }
}

void
View::setSideSetVisibility(int sideset_id, bool visible)
{
    auto * sideset = this->model->getSideSet(sideset_id);
    if (sideset)
        sideset->setVisible(visible);
}

void
View::setNodeSetVisibility(int nodeset_id, bool visible)
{
    auto * nodeset = this->model->getNodeSet(nodeset_id);
    if (nodeset)
        nodeset->setVisible(visible);
}

void
View::render()
{
    this->render_window->Render();
}

void
View::updateLocation()
{
    auto width = this->main_window->getRenderWindowWidth();
    auto tr = this->geometry().topRight();
    this->view_mode->move(width - 10 - this->view_mode->width(), tr.y() + 10);
}

void
View::activateRenderMode()
{
    switch (this->render_mode) {
    case SHADED:
        onShadedTriggered(true);
        break;
    case SHADED_WITH_EDGES:
        onShadedWithEdgesTriggered(true);
        break;
    case HIDDEN_EDGES_REMOVED:
        onHiddenEdgesRemovedTriggered(true);
        break;
    case TRANSLUENT:
        onTransluentTriggered(true);
        break;
    }
}

void
View::setupCubeAxesActor()
{
    this->cube_axes_actor = vtkCubeAxesActor::New();
    this->cube_axes_actor->VisibilityOff();
    this->cube_axes_actor->SetCamera(getActiveCamera());
    this->cube_axes_actor->SetGridLineLocation(vtkCubeAxesActor::VTK_GRID_LINES_ALL);
    this->cube_axes_actor->SetFlyMode(vtkCubeAxesActor::VTK_FLY_OUTER_EDGES);
}

void
View::updateBoundingBox()
{
    auto bbox = this->model->getTotalBoundingBox();
    double bounds[6];
    bbox.GetBounds(bounds);
    this->cube_axes_actor->SetBounds(bounds);
    this->renderer->AddViewProp(this->cube_axes_actor);
}

void
View::setCubeAxisVisibility(bool visible)
{
    if (visible)
        this->cube_axes_actor->VisibilityOn();
    else
        this->cube_axes_actor->VisibilityOff();

}

vtkRenderer *
View::getRenderer() const
{
    return this->renderer;
}

vtkGenericOpenGLRenderWindow *
View::getRenderWindow() const
{
    return this->render_window;
}
