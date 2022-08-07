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
#include "infowindow.h"
#include "aboutdlg.h"
#include "reader.h"
#include "exodusiireader.h"
#include "common/loadfileevent.h"

static const int MAX_RECENT_FILES = 10;

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
    windows_action_group(nullptr)
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
}

void
MainWindow::addSideSets()
{
}

void
MainWindow::addNodeSets()
{
}

MainWindow::ERenderMode
MainWindow::getRenderMode()
{
    return this->render_mode;
}

void
MainWindow::setSelectedBlockProperties()
{
}

void
MainWindow::setDeselectedBlockProperties()
{
}

void
MainWindow::setBlockProperties()
{
}

void
MainWindow::setSideSetProperties()
{
}

void
MainWindow::setNodeSetProperties()
{
}

void
MainWindow::setSelectionProperties()
{
}

void
MainWindow::showNotification(const QString & text, int ms)
{
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
MainWindow::deselectBlocks()
{
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
    else if (event->type() == LoadFileEvent::type) {
        auto * e = dynamic_cast<LoadFileEvent *>(event);
        loadFile(e->fileName());
        return true;
    }

    return QMainWindow::event(event);
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

    // self._com = common.centerOfBounds(bnds)
    // self._cube_axes_actor.SetBounds(*bnds)
    // self._vtk_renderer.AddViewProp(self._cube_axes_actor)

    // params = {
    //     'blocks': reader.getBlocks(),
    //     'sidesets': reader.getSideSets(),
    //     'nodesets': reader.getNodeSets(),
    //     'total_elems': reader.getTotalNumberOfElements(),
    //     'total_nodes': reader.getTotalNumberOfNodes()
    // }
    // self.fileLoaded.emit(params)
    // self.boundsChanged.emit(bnds)

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

    // camera = self._vtk_renderer.GetActiveCamera()
    // focal_point = camera.GetFocalPoint()
    // camera.SetPosition(focal_point[0], focal_point[1], 1)
    // camera.SetRoll(0)
    // self._vtk_renderer.ResetCamera()
}

void
MainWindow::onBlockVisibilityChanged()
{
}

void
MainWindow::onBlockColorChanged()
{
}

void
MainWindow::onSidesetVisibilityChanged()
{
}

void
MainWindow::onNodesetVisibilityChanged()
{
}

void
MainWindow::onCubeAxisVisibilityChanged()
{
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
    // emit fileLoaded(None);
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
MainWindow::onBlockSelectionChanged()
{
}

void
MainWindow::onSidesetSelectionChanged()
{
}

void
MainWindow::onNodesetSelectionChanged()
{
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
