#include "mainwindow.h"
#include <QLabel>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QString>
#include <QProgressDialog>
#include <QFileSystemWatcher>
#include <QMenu>
#include <QActionGroup>
#include <QSettings>
#include <QEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QPushButton>
#include <QFileInfo>
#include <QtDebug>
#include <QDockWidget>
#include <QApplication>
#include <QFileDialog>
#include <QProgressDialog>
#include <QVector3D>
#include "QVTKOpenGLNativeWidget.h"
#include "vtkGenericOpenGLRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkAxesActor.h"
#include "vtkOrientationMarkerWidget.h"
#include "vtkExtractBlock.h"
#include "vtkProperty.h"
#include "vtkCamera.h"
#include "vtkCubeAxesActor.h"
#include "infowindow.h"
#include "aboutdlg.h"
#include "reader.h"
#include "blockobject.h"
#include "sidesetobject.h"
#include "nodesetobject.h"
#include "exodusiireader.h"
#include "common/loadfileevent.h"
#include "common/notificationwidget.h"

static const int MAX_RECENT_FILES = 10;

QColor MainWindow::SIDESET_CLR = QColor(255, 173, 79);
QColor MainWindow::SIDESET_EDGE_CLR = QColor(26, 26, 102);
QColor MainWindow::NODESET_CLR = QColor(168, 91, 2);
QColor MainWindow::SELECTION_CLR = QColor(255, 173, 79);
QColor MainWindow::SELECTION_EDGE_CLR = QColor(179, 95, 0);

int MainWindow::SIDESET_EDGE_WIDTH = 5;

// Main window - Load thread

MainWindow::LoadThread::LoadThread(const QString & file_name) : QThread(), reader(nullptr)
{
    if (file_name.endsWith(".e") || file_name.endsWith(".exo"))
        this->reader = new ExodusIIReader(file_name.toStdString());
    else
        this->reader = nullptr;
}

Reader *
MainWindow::LoadThread::getReader()
{
    return this->reader;
}

void
MainWindow::LoadThread::run()
{
    assert(this->reader != nullptr);
    this->reader->load();
}

// Main window

MainWindow::MainWindow(QWidget * parent) :
    QMainWindow(parent),
    settings(new QSettings("David Andrs", "MeshInspector")),
    load_thread(nullptr),
    progress(nullptr),
    file_name(),
    file_watcher(new QFileSystemWatcher()),
    notification(nullptr),
    render_mode(SHADED_WITH_EDGES),
    select_mode(MODE_SELECT_NONE),
    color_profile_id(COLOR_PROFILE_DEFAULT),
    menu_bar(new QMenuBar(nullptr)),
    recent_menu(nullptr),
    export_menu(nullptr),
    view_menu(nullptr),
    view_mode(nullptr),
    vtk_widget(nullptr),
    vtk_render_window(nullptr),
    vtk_renderer(nullptr),
    vtk_interactor(nullptr),
    ori_marker(nullptr),
    cube_axes_actor(nullptr),
    info_dock(nullptr),
    info_window(nullptr),
    about_dlg(nullptr),
    new_action(nullptr),
    open_action(nullptr),
    close_action(nullptr),
    clear_recent_file(nullptr),
    shaded_action(nullptr),
    shaded_w_edges_action(nullptr),
    hidden_edges_removed_action(nullptr),
    transluent_action(nullptr),
    view_info_wnd_action(nullptr),
    tools_explode_action(nullptr),
    perspective_action(nullptr),
    ori_marker_action(nullptr),
    minimize(nullptr),
    bring_all_to_front(nullptr),
    show_main_window(nullptr),
    visual_repr(nullptr),
    color_profile_action_group(nullptr),
    mode_select_action_group(nullptr),
    windows_action_group(nullptr),
    selected_block(nullptr)
{
    QSize default_size = QSize(1000, 700);
    QVariant geom = this->settings->value("window/geometry", default_size);
    if (!this->restoreGeometry(geom.toByteArray()))
        this->resize(default_size);
    this->recent_files = this->settings->value("recent_files", QStringList()).toStringList();

    setupWidgets();
    setupMenuBar();
    updateWindowTitle();
    updateMenuBar();
    loadColorProfiles();

    setAcceptDrops(true);

    connectSignals();
    setupVtk();
    setColorProfile();

    setupOrientationMarker();
    setupCubeAxesActor();

    clear();
    show();

    connect(&this->update_timer, SIGNAL(timeout()), this, SLOT(onUpdateWindow()));
    this->update_timer.start(250);
    QTimer::singleShot(1, this, SLOT(updateViewModeLocation()));
}

MainWindow::~MainWindow()
{
    delete this->settings;
    delete this->file_watcher;
    delete this->menu_bar;
    delete this->vtk_widget;
    delete this->info_window;
    delete this->info_dock;
}

void
MainWindow::setupWidgets()
{
    this->vtk_widget = new QVTKOpenGLNativeWidget();
    setCentralWidget(this->vtk_widget);

    this->vtk_render_window = vtkGenericOpenGLRenderWindow::New();
    this->vtk_widget->setRenderWindow(this->vtk_render_window);

    this->vtk_renderer = vtkRenderer::New();
    this->vtk_render_window->AddRenderer(this->vtk_renderer);

    this->info_window = new InfoWindow(this);

    this->info_dock = new QDockWidget("Information", this);
    this->info_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    this->info_dock->setFloating(false);
    this->info_dock->setFeatures(QDockWidget::DockWidgetMovable);
    this->info_dock->setWidget(this->info_window);
    addDockWidget(Qt::RightDockWidgetArea, this->info_dock);

    setupViewModeWidget(this);
    setupFileChangedNotificationWidget();
    setupNotificationWidget();
    // this->selected_mesh_ent_info = SelectedMeshEntityInfoWidget(self)
    // this->selected_mesh_ent_info.setVisible(False)

    // this->deselect_sc = QtWidgets.QShortcut(
    //     QtGui.QKeySequence(QtCore.Qt.Key_Space), self)
    // this->deselect_sc.activated.connect(self.onDeselect)

    setupExplodeWidgets();
}

void
MainWindow::setupViewModeWidget(QMainWindow * wnd)
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

    connect(this->shaded_action, SIGNAL(triggered(bool)), this, SLOT(onShadedTriggered(bool)));
    connect(this->shaded_w_edges_action,
            SIGNAL(triggered(bool)),
            this,
            SLOT(onShadedWithEdgesTriggered(bool)));
    connect(this->hidden_edges_removed_action,
            SIGNAL(triggered(bool)),
            this,
            SLOT(onHiddenEdgesRemovedTriggered(bool)));
    connect(this->transluent_action,
            SIGNAL(triggered(bool)),
            this,
            SLOT(onTransluentTriggered(bool)));
    connect(this->perspective_action,
            SIGNAL(toggled(bool)),
            this,
            SLOT(onPerspectiveToggled(bool)));
    connect(this->ori_marker_action,
            SIGNAL(toggled(bool)),
            this,
            SLOT(onOrientationMarkerVisibilityChanged(bool)));

    this->view_mode = new QPushButton(wnd);
    this->view_mode->setFixedSize(60, 32);
    // this->view_mode->setIcon(Assets().icons['render-mode']);
    this->view_mode->setMenu(this->view_menu);
    this->view_mode->show();
}

void
MainWindow::setupNotificationWidget()
{
    this->notification = new NotificationWidget();
    this->notification->setVisible(false);
}

void
MainWindow::setupFileChangedNotificationWidget()
{
}

void
MainWindow::setupExplodeWidgets()
{
}

void
MainWindow::setupMenuBar()
{
    setMenuBar(this->menu_bar);
    QMenu * file_menu = this->menu_bar->addMenu("File");
    this->new_action = file_menu->addAction("New", this, SLOT(onNewFile()), QKeySequence("Ctrl+N"));
    this->open_action =
        file_menu->addAction("Open", this, SLOT(onOpenFile()), QKeySequence("Ctrl+O"));
    this->recent_menu = file_menu->addMenu("Open Recent");
    buildRecentFilesMenu();
    file_menu->addSeparator();
    this->export_menu = file_menu->addMenu("Export as...");
    setupExportMenu(export_menu);
    file_menu->addSeparator();
    this->close_action =
        file_menu->addAction("Close", this, SLOT(onClose()), QKeySequence("Ctrl+W"));

    // The "About" item is fine here, since we assume Mac and that will
    // place the item into different submenu but this will need to be fixed
    // for linux and windows
    file_menu->addSeparator();
    this->about_box_action = file_menu->addAction("About", this, SLOT(onAbout()));

    QMenu * view_menu = this->menu_bar->addMenu("View");
    view_menu->addAction(this->shaded_action);
    view_menu->addAction(this->shaded_w_edges_action);
    view_menu->addAction(this->hidden_edges_removed_action);
    view_menu->addAction(this->transluent_action);
    view_menu->addSeparator();
    this->view_info_wnd_action =
        view_menu->addAction("Info window", this, SLOT(onViewInfoWindow()));
    this->view_info_wnd_action->setCheckable(true);
    QMenu * color_profile_menu = view_menu->addMenu("Color profile");
    setupColorProfileMenu(color_profile_menu);

    QMenu * tools_menu = this->menu_bar->addMenu("Tools");
    setupSelectModeMenu(tools_menu);
    this->tools_explode_action = tools_menu->addAction("Explode", this, SLOT(onToolsExplode()));

    QMenu * window_menu = this->menu_bar->addMenu("Window");
    this->minimize =
        window_menu->addAction("Minimize", this, SLOT(onMinimize()), QKeySequence("Ctrl+M"));
    window_menu->addSeparator();
    this->bring_all_to_front =
        window_menu->addAction("Bring All to Front", this, SLOT(onBringAllToFront()));
    window_menu->addSeparator();
    this->show_main_window =
        window_menu->addAction("Mesh Inspector", this, SLOT(onShowMainWindow()));
    this->show_main_window->setCheckable(true);

    this->windows_action_group = new QActionGroup(this);
    this->windows_action_group->addAction(this->show_main_window);
}

void
MainWindow::setupExportMenu(QMenu * menu)
{
    menu->addAction("PNG...", this, SLOT(onExportAsPng()));
    menu->addAction("JPG...", this, SLOT(onExportAsJpg()));
}

void
MainWindow::setupColorProfileMenu(QMenu * menu)
{
    this->color_profile_action_group = new QActionGroup(this);
    this->color_profile_id = static_cast<EColorProfile>(
        this->settings->value("color_profile", COLOR_PROFILE_DEFAULT).toInt());
    // TODO: fill in color profile
    connect(this->color_profile_action_group,
            SIGNAL(triggered(QAction *)),
            this,
            SLOT(onColorProfileTriggered(QAction *)));
}

void
MainWindow::setupSelectModeMenu(QMenu * menu)
{
    QMenu * select_menu = menu->addMenu("Select mode");
    this->mode_select_action_group = new QActionGroup(this);
    this->select_mode = static_cast<EModeSelect>(
        this->settings->value("tools/select_mode", MODE_SELECT_NONE).toInt());
    // TODO: fill in selection modes
    connect(this->mode_select_action_group,
            SIGNAL(triggered(QAction *)),
            this,
            SLOT(onSelectModeTriggered(QAction *)));
}

void
MainWindow::updateMenuBar()
{
    auto * active_window = QApplication::activeWindow();
    this->show_main_window->setChecked(active_window == this);

    this->view_info_wnd_action->setChecked(this->info_window->isVisible());
    this->tools_explode_action->setEnabled(!this->file_name.isEmpty());
}

void
MainWindow::updateWindowTitle()
{
    if (this->file_name.isEmpty())
        setWindowTitle("Mesh Inspector");
    else {
        QFileInfo fi(this->file_name);
        QString title = QString("Mesh Inspector \u2014 %1").arg(fi.fileName());
        setWindowTitle(title);
    }
}

void
MainWindow::connectSignals()
{
    connect(this,
            SIGNAL(blockAdded(int, const QString &)),
            this->info_window,
            SLOT(onBlockAdded(int, const QString &)));
    connect(this->info_window,
            SIGNAL(blockVisibilityChanged(int, bool)),
            this,
            SLOT(onBlockVisibilityChanged(int, bool)));
    connect(this->info_window,
            SIGNAL(blockColorChanged(int, QColor)),
            this,
            SLOT(onBlockColorChanged(int, QColor)));
    connect(this->info_window,
            SIGNAL(blockSelectionChanged(int)),
            this,
            SLOT(onBlockSelectionChanged(int)));

    connect(this,
            SIGNAL(sideSetAdded(int, const QString &)),
            this->info_window,
            SLOT(onSideSetAdded(int, const QString &)));
    connect(this->info_window,
            SIGNAL(sideSetVisibilityChanged(int, bool)),
            this,
            SLOT(onSideSetVisibilityChanged(int, bool)));
    connect(this->info_window,
            SIGNAL(sideSetSelectionChanged(int)),
            this,
            SLOT(onSideSetSelectionChanged(int)));

    connect(this,
            SIGNAL(nodeSetAdded(int, const QString &)),
            this->info_window,
            SLOT(onNodeSetAdded(int, const QString &)));
    connect(this->info_window,
            SIGNAL(nodeSetVisibilityChanged(int, bool)),
            this,
            SLOT(onNodeSetVisibilityChanged(int, bool)));
    connect(this->info_window,
            SIGNAL(nodeSetSelectionChanged(int)),
            this,
            SLOT(onNodeSetSelectionChanged(int)));

    connect(this->info_window,
            SIGNAL(dimensionsStateChanged(bool)),
            this,
            SLOT(onCubeAxisVisibilityChanged(bool)));
}

void
MainWindow::setupVtk()
{
    this->vtk_interactor = this->vtk_render_window->GetInteractor();

    // TODO: set interactor style

    // TODO: set background from preferences/templates
    this->vtk_renderer->SetGradientBackground(true);
    // set anti-aliasing on
    this->vtk_renderer->SetUseFXAA(true);
    this->vtk_render_window->SetMultiSamples(1);
}

void
MainWindow::clear()
{
    for (auto & it : this->blocks)
        delete it.second;
    this->blocks.clear();

    for (auto & it : this->side_sets)
        delete it.second;
    this->side_sets.clear();

    for (auto & it : this->node_sets)
        delete it.second;
    this->node_sets.clear();

    this->vtk_renderer->RemoveAllViewProps();
}

void
MainWindow::loadFile(const QString & file_name)
{
    this->clear();
    if (!this->checkFileExists(file_name))
        return;

    QFileInfo fi(file_name);
    this->progress =
        new QProgressDialog(QString("Loading %1...").arg(fi.fileName()), QString(), 0, 0, this);
    // this->progress->setWindowModality(Qt::WindowModal);
    // this->progress->setMinimumDuration(0);
    // this->progress->show();

    this->load_thread = new LoadThread(file_name);
    connect(this->load_thread, SIGNAL(finished()), this, SLOT(onLoadFinished()));
    this->load_thread->start(QThread::IdlePriority);
}

void
MainWindow::setupOrientationMarker()
{
    vtkAxesActor * axes = vtkAxesActor::New();
    this->ori_marker = vtkOrientationMarkerWidget::New();
    this->ori_marker->SetDefaultRenderer(this->vtk_renderer);
    this->ori_marker->SetOrientationMarker(axes);
    this->ori_marker->SetViewport(0.8, 0, 1.0, 0.2);
    this->ori_marker->SetInteractor(this->vtk_interactor);
    this->ori_marker->SetEnabled(true);
    this->ori_marker->SetInteractive(false);
}

void
MainWindow::setupCubeAxesActor()
{
    this->cube_axes_actor = vtkCubeAxesActor::New();
    this->cube_axes_actor->VisibilityOff();
    this->cube_axes_actor->SetCamera(this->vtk_renderer->GetActiveCamera());
    this->cube_axes_actor->SetGridLineLocation(vtkCubeAxesActor::VTK_GRID_LINES_ALL);
    this->cube_axes_actor->SetFlyMode(vtkCubeAxesActor::VTK_FLY_OUTER_EDGES);
}

int
MainWindow::getRenderWindowWidth() const
{
    int info_width = this->info_dock->geometry().width();
    return this->geometry().width() - info_width;
}

bool
MainWindow::checkFileExists(const QString & file_name)
{
    QFileInfo fi(file_name);
    if (fi.exists())
        return true;
    else {
        auto base_file = fi.fileName();
        showNotification(QString("Unable to open '%1': File does not exist.").arg(base_file));
        return false;
    }
}

void
MainWindow::addBlocks()
{
    auto * camera = this->vtk_renderer->GetActiveCamera();
    auto * reader = this->load_thread->getReader();

    for (auto & binfo : reader->getBlocks()) {
        vtkExtractBlock * eb = vtkExtractBlock::New();
        eb->SetInputConnection(reader->getVtkOutputPort());
        eb->AddIndex(binfo.multiblock_index);
        eb->Update();

        auto * block = new BlockObject(eb, camera);
        setBlockProperties(block);
        this->blocks[binfo.number] = block;

        this->vtk_renderer->AddViewProp(block->getActor());
        this->vtk_renderer->AddViewProp(block->getSilhouetteActor());

        emit blockAdded(binfo.number, QString::fromStdString(binfo.name));
    }
}

void
MainWindow::addSideSets()
{
    auto * reader = this->load_thread->getReader();

    for (auto & finfo : reader->getSideSets()) {
        auto * eb = vtkExtractBlock::New();
        eb->SetInputConnection(reader->getVtkOutputPort());
        eb->AddIndex(finfo.multiblock_index);
        eb->Update();

        auto sideset = new SideSetObject(eb);
        this->side_sets[finfo.number] = sideset;
        setSideSetProperties(sideset);
        this->vtk_renderer->AddViewProp(sideset->getActor());

        emit sideSetAdded(finfo.number, QString::fromStdString(finfo.name));
    }
}

void
MainWindow::addNodeSets()
{
    auto * reader = this->load_thread->getReader();

    for (auto & ninfo : reader->getNodeSets()) {
        auto * eb = vtkExtractBlock::New();
        eb->SetInputConnection(reader->getVtkOutputPort());
        eb->AddIndex(ninfo.multiblock_index);
        eb->Update();

        auto * nodeset = new NodeSetObject(eb);
        this->node_sets[ninfo.number] = nodeset;
        this->setNodeSetProperties(nodeset);
        this->vtk_renderer->AddViewProp(nodeset->getActor());

        emit nodeSetAdded(ninfo.number, QString::fromStdString(ninfo.name));
    }
}

BlockObject *
MainWindow::getBlock(int block_id)
{
    const auto & it = this->blocks.find(block_id);
    if (it != this->blocks.end())
        return it->second;
    else
        return nullptr;
}

SideSetObject *
MainWindow::getSideSet(int sideset_id)
{
    const auto & it = this->side_sets.find(sideset_id);
    if (it != this->side_sets.end())
        return it->second;
    else
        return nullptr;
}

NodeSetObject *
MainWindow::getNodeSet(int nodeset_id)
{
    const auto & it = this->node_sets.find(nodeset_id);
    if (it != this->node_sets.end())
        return it->second;
    else
        return nullptr;
}

void
MainWindow::setSelectedBlockProperties(BlockObject * block)
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
        property->SetLineWidth(2);
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
}

void
MainWindow::setDeselectedBlockProperties(BlockObject * block)
{
    auto * property = block->getProperty();
    if (this->render_mode == SHADED) {
        auto & clr = block->getColor();
        property->SetColor(clr.redF(), clr.greenF(), clr.blueF());
        property->SetOpacity(1.0);
        property->SetEdgeVisibility(false);
    }
    else if (this->render_mode == SHADED_WITH_EDGES) {
        auto & clr = block->getColor();
        property->SetColor(clr.redF(), clr.greenF(), clr.blueF());
        property->SetOpacity(1.0);
        property->SetEdgeVisibility(true);
        property->SetEdgeColor(SIDESET_EDGE_CLR.redF(),
                               SIDESET_EDGE_CLR.greenF(),
                               SIDESET_EDGE_CLR.blueF());
        property->SetLineWidth(2);
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
}

void
MainWindow::setBlockProperties(BlockObject * block, bool selected)
{
    auto * property = block->getProperty();
    property->SetRepresentationToSurface();
    property->SetAmbient(0.4);
    property->SetDiffuse(0.6);
    if (selected)
        this->setSelectedBlockProperties(block);
    else
        this->setDeselectedBlockProperties(block);
}

void
MainWindow::setSideSetProperties(SideSetObject * sideset)
{
    auto * property = sideset->getProperty();
    property->SetColor(SIDESET_CLR.redF(), SIDESET_CLR.greenF(), SIDESET_CLR.blueF());
    property->SetEdgeVisibility(false);
    property->LightingOff();
    if (this->render_mode == SHADED || this->render_mode == SHADED_WITH_EDGES) {
        property->SetEdgeColor(SIDESET_EDGE_CLR.redF(),
                               SIDESET_EDGE_CLR.greenF(),
                               SIDESET_EDGE_CLR.blueF());
        property->SetLineWidth(SIDESET_EDGE_WIDTH);
    }
}

void
MainWindow::setNodeSetProperties(NodeSetObject * nodeset)
{
    auto * property = nodeset->getProperty();
    property->SetRepresentationToPoints();
    property->SetRenderPointsAsSpheres(true);
    property->SetVertexVisibility(true);
    property->SetEdgeVisibility(false);
    property->SetPointSize(10);
    property->SetColor(NODESET_CLR.redF(), NODESET_CLR.greenF(), NODESET_CLR.blueF());
    property->SetOpacity(1);
    property->SetAmbient(1);
    property->SetDiffuse(0);
}

void
MainWindow::setSelectionProperties()
{
}

void
MainWindow::showNotification(const QString & text, int ms)
{
    this->notification->setText(text);
    this->notification->adjustSize();
    auto width = geometry().width();
    auto left = (width - this->notification->width()) / 2;
    // top = 10
    auto top = height() - this->notification->height() - 10;
    this->notification->setGeometry(left,
                                    top,
                                    this->notification->width(),
                                    this->notification->height());
    this->notification->setGraphicsEffect(nullptr);
    // this->notification->show(ms);
}

void
MainWindow::showFileChangedNotification()
{
}

void
MainWindow::showSelectedMeshEntity()
{
}

void
MainWindow::hideSelectedMeshEntity()
{
    // this->selected_mesh_ent_info->hide();
}

void
MainWindow::deselectBlocks()
{
    if (this->selected_block != nullptr) {
        setBlockProperties(this->selected_block, false);
        this->selected_block = nullptr;
    }
}

// void
// MainWindow::blockActorToId(actor)
// {
// }

void
MainWindow::selectBlock(const QPoint & pt)
{
}

void
MainWindow::selectCell(const QPoint & pt)
{
}

void
MainWindow::selectPoint(const QPoint & pt)
{
}

void
MainWindow::setColorProfile()
{
    double bkgnd[3] = { 82 / 256., 87 / 256., 110 / 256. };
    this->vtk_renderer->SetBackground(bkgnd);
    this->vtk_renderer->SetBackground2(bkgnd);
}

void
MainWindow::loadColorProfiles()
{
}

QString
MainWindow::getFileName()
{
    return QString();
}

void
MainWindow::buildRecentFilesMenu()
{
    assert(this->recent_menu != nullptr);
    this->recent_menu->clear();
    if (this->recent_files.length() > 0) {
        for (auto it = this->recent_files.rbegin(); it != this->recent_files.rend(); ++it) {
            QString f = *it;
            QFileInfo fi(f);
            auto * action =
                this->recent_menu->addAction(fi.fileName(), this, SLOT(onOpenRecentFile()));
            action->setData(f);
        }
        this->recent_menu->addSeparator();
    }
    this->clear_recent_file =
        this->recent_menu->addAction("Clear Menu", this, SLOT(onClearRecentFiles()));
}

void
MainWindow::addToRecentFiles(const QString & file_name)
{
    QStringList rf_list;
    for (auto & f : this->recent_files) {
        if (f.compare(file_name) != 0)
            rf_list.append(f);
    }
    rf_list.append(file_name);
    if (rf_list.length() > MAX_RECENT_FILES)
        rf_list.removeFirst();
    this->recent_files = rf_list;
}

bool
MainWindow::event(QEvent * event)
{
    if (event->type() == QEvent::WindowActivate)
        this->updateMenuBar();

    return QMainWindow::event(event);
}

void
MainWindow::customEvent(QEvent * event)
{
    if (event->type() == LoadFileEvent::type) {
        auto * e = dynamic_cast<LoadFileEvent *>(event);
        loadFile(e->fileName());
    }
}

void
MainWindow::resizeEvent(QResizeEvent * event)
{
    QMainWindow::resizeEvent(event);
    updateViewModeLocation();
}

void
MainWindow::dragEnterEvent(QDragEnterEvent * event)
{
    if (event->mimeData() && event->mimeData()->hasUrls())
        event->accept();
    else
        event->ignore();
}

void
MainWindow::dropEvent(QDropEvent * event)
{
    if (event->mimeData() && event->mimeData()->hasUrls()) {
        event->setDropAction(Qt::CopyAction);
        event->accept();

        QStringList file_names;
        for (auto & url : event->mimeData()->urls())
            file_names.append(url.toLocalFile());
        if (file_names.length() > 0)
            loadFile(file_names[0]);
    }
    else
        event->ignore();
}

void
MainWindow::closeEvent(QCloseEvent * event)
{
    this->settings->setValue("tools/select_mode", this->select_mode);
    this->settings->setValue("color_profile", this->color_profile_id);
    this->settings->setValue("window/geometry", this->saveGeometry());
    this->settings->setValue("recent_files", this->recent_files);
    QMainWindow::closeEvent(event);
}

void
MainWindow::onClose()
{
}

void
MainWindow::onLoadFinished()
{
    Reader * reader = this->load_thread->getReader();

    this->info_window->clear();
    addBlocks();
    addSideSets();
    addNodeSets();

    // auto gmin = QVector3D(float('inf'), float('inf'), float('inf'));
    // auto gmax = QVector3D(float('-inf'), float('-inf'), float('-inf'));
    // for block in self._blocks.values():
    //     bmin, bmax = block.bounds
    //     gmin = common.point_min(bmin, gmin)
    //     gmax = common.point_max(bmax, gmax)
    // bnds = [gmin.x(), gmax.x(), gmin.y(), gmax.y(), gmin.z(), gmax.z()]
    // self.boundsChanged.emit(bnds)

    // self._com = common.centerOfBounds(bnds)
    // self._cube_axes_actor.SetBounds(*bnds)
    this->vtk_renderer->AddViewProp(this->cube_axes_actor);

    this->info_window->setSummary(reader->getTotalNumberOfElements(),
                                  reader->getTotalNumberOfNodes());

    this->file_name = QString(reader->getFileName().c_str());
    updateWindowTitle();
    addToRecentFiles(this->file_name);
    buildRecentFilesMenu();
    this->file_watcher->addPath(this->file_name);
    // this->file_changed_notification->setFileName(this->file_name);

    // self._selection = Selection(self._geometry.GetOutput())
    // self._setSelectionProperties(self._selection)
    // self._vtk_renderer.AddActor(self._selection.getActor())

    // this->progress->hide();
    delete this->progress;
    this->progress = nullptr;

    this->updateMenuBar();

    // if reader.getDimensionality() == 3:
    //     style = OtterInteractorStyle3D(self)
    // else:
    //     style = OtterInteractorStyle2D(self)
    // self._vtk_interactor.SetInteractorStyle(style)

    auto * camera = this->vtk_renderer->GetActiveCamera();
    auto * focal_point = camera->GetFocalPoint();
    camera->SetPosition(focal_point[0], focal_point[1], 1);
    camera->SetRoll(0);
    this->vtk_renderer->ResetCamera();
}

void
MainWindow::onBlockVisibilityChanged(int block_id, bool visible)
{
    BlockObject * block = getBlock(block_id);
    if (block) {
        block->setVisible(visible);
        if (this->render_mode == HIDDEN_EDGES_REMOVED || this->render_mode == TRANSLUENT)
            block->setSilhouetteVisible(visible);
        else
            block->setSilhouetteVisible(false);
    }
}

void
MainWindow::onBlockColorChanged(int block_id, QColor color)
{
    BlockObject * block = getBlock(block_id);
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
MainWindow::onSideSetVisibilityChanged(int sideset_id, bool visible)
{
    auto * sideset = getSideSet(sideset_id);
    if (sideset)
        sideset->setVisible(visible);
}

void
MainWindow::onNodeSetVisibilityChanged(int nodeset_id, bool visible)
{
    auto * nodeset = getNodeSet(nodeset_id);
    if (nodeset)
        nodeset->setVisible(visible);
}

void
MainWindow::onCubeAxisVisibilityChanged(bool visible)
{
    if (visible)
        this->cube_axes_actor->VisibilityOn();
    else
        this->cube_axes_actor->VisibilityOff();
}

void
MainWindow::onOrientationMarkerVisibilityChanged(bool visible)
{
    if (visible)
        this->ori_marker->EnabledOn();
    else
        this->ori_marker->EnabledOff();
}

void
MainWindow::onOpenFile()
{
    QString file_name = QFileDialog::getOpenFileName(this,
                                                     "Open File",
                                                     "",
                                                     "ExodusII files (*.e *.exo);;"
                                                     "VTK Unstructured Grid files (*.vtk)");
    if (!file_name.isNull())
        this->loadFile(file_name);
}

void
MainWindow::onOpenRecentFile()
{
    QAction * action = dynamic_cast<QAction *>(this->sender());
    if (action != nullptr) {
        auto file_name = action->data();
        loadFile(file_name.toString());
    }
}

void
MainWindow::onClearRecentFiles()
{
    this->recent_files = QStringList();
    buildRecentFilesMenu();
}

void
MainWindow::onNewFile()
{
    this->clear();
    this->info_window->clear();
    // emit boundsChanged([]);
    this->file_name = QString();
    this->updateWindowTitle();
}

void
MainWindow::onShadedTriggered(bool checked)
{
}

void
MainWindow::onShadedWithEdgesTriggered(bool checked)
{
}

void
MainWindow::onHiddenEdgesRemovedTriggered(bool checked)
{
}

void
MainWindow::onTransluentTriggered(bool checked)
{
}

void
MainWindow::onPerspectiveToggled(bool checked)
{
}

void
MainWindow::onUpdateWindow()
{
    this->vtk_render_window->Render();
}

void
MainWindow::onFileChanged()
{
}

void
MainWindow::onReloadFile()
{
}

void
MainWindow::onBlockSelectionChanged(int block_id)
{
    deselectBlocks();
    const auto & it = this->blocks.find(block_id);
    if (it != this->blocks.end()) {
        BlockObject * block = it->second;
        // TODO
        // this->selected_mesh_ent_info.setBlockInfo(block_id, block->info());
        showSelectedMeshEntity();
        this->selected_block = block;
        setBlockProperties(block, true);
    }
    else
        hideSelectedMeshEntity();
}

void
MainWindow::onSideSetSelectionChanged(int sideset_id)
{
    const auto & it = this->side_sets.find(sideset_id);
    if (it != this->side_sets.end()) {
        auto * sideset = it->second;
        // this->selected_mesh_ent_info->setSidesetInfo(sideset_id, sideset->info);
        showSelectedMeshEntity();
    }
    else
        hideSelectedMeshEntity();
}

void
MainWindow::onNodeSetSelectionChanged(int nodeset_id)
{
    const auto & it = this->node_sets.find(nodeset_id);
    if (it != this->node_sets.end()) {
        auto * nodeset = it->second;
        // this->selected_mesh_ent_info->setNodesetInfo(sideset_id, sideset->info);
        showSelectedMeshEntity();
    }
    else
        hideSelectedMeshEntity();
}

void
MainWindow::onClicked(const QPoint & pt)
{
}

void
MainWindow::onViewInfoWindow()
{
}

void
MainWindow::onSelectModeTriggered(QAction * action)
{
}

void
MainWindow::onDeselect()
{
}

void
MainWindow::onColorProfileTriggered(QAction *)
{
}

void
MainWindow::onExportAsPng()
{
}

void
MainWindow::onExportAsJpg()
{
}

void
MainWindow::onToolsExplode()
{
}

void
MainWindow::onExplodeValueChanged()
{
}

void
MainWindow::updateViewModeLocation()
{
    auto width = this->getRenderWindowWidth();
    this->view_mode->move(width - 5 - this->view_mode->width(), 10);
}

void
MainWindow::onMinimize()
{
    showMinimized();
}

void
MainWindow::onBringAllToFront()
{
    showNormal();
}

void
MainWindow::onShowMainWindow()
{
    showNormal();
    activateWindow();
    raise();
    updateMenuBar();
}

void
MainWindow::onAbout()
{
    if (this->about_dlg == nullptr)
        this->about_dlg = new AboutDialog(this);
    this->about_dlg->show();
}
