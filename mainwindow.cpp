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
#include <QFrame>
#include <QPushButton>
#include <QFileInfo>
#include <QtDebug>
#include <QDockWidget>
#include "infowindow.h"
#include "common/reader.h"

// Main window - Load thread

MainWindow::LoadThread::LoadThread(const QString & file_name) : QThread(), reader(nullptr) {}

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
    visual_repr(nullptr),
    color_profile_action_group(nullptr),
    mode_select_action_group(nullptr)
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

    // this->_vtk_interactor.Initialize();
    // this->_vtk_interactor.Start();

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
    this->vtk_widget = new QFrame();
    setCentralWidget(this->vtk_widget);

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
    // this->shaded_action.triggered.connect(self.onShadedTriggered);
    // this->shaded_w_edges_action.triggered.connect(self.onShadedWithEdgesTriggered);
    // this->hidden_edges_removed_action.triggered.connect(self.onHiddenEdgesRemovedTriggered);
    // this->transluent_action.triggered.connect(self.onTransluentTriggered);
    // this->perspective_action.toggled.connect(self.onPerspectiveToggled);
    // this->ori_marker_action.toggled.connect(self.onOrientationMarkerVisibilityChanged);

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
    this->color_profile_id = {
        this->settings->value("color_profile", COLOR_PROFILE_DEFAULT).toInt()
    };
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
    this->select_mode = { this->settings->value("tools/select_mode", MODE_SELECT_NONE).toInt() };
    // TODO: fill in selection modes
    connect(this->mode_select_action_group,
            SIGNAL(triggered(QAction *)),
            this,
            SLOT(onSelectModeTriggered(QAction *)));
}

void
MainWindow::updateMenuBar()
{
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
}

void
MainWindow::clear()
{
}

void
MainWindow::loadFile(const QString & file_name)
{
}

void
MainWindow::setupOrientationMarker()
{
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
MainWindow::checkFileExists(const QString & file_name) const
{
    return false;
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
        // for f in reversed(self.recent_files):
        //     unused_path, file_name = os.path.split(f);
        //     action = self._recent_menu.addAction(file_name,
        //                                          self.onOpenRecentFile);
        //     action.setData(f);
        // this->recent_menu->addSeparator();
    }
    this->clear_recent_file =
        this->recent_menu->addAction("Clear Menu", this, SLOT(onClearRecentFiles()));
}

void
MainWindow::addToRecentFiles(const QString & file_name)
{
}

bool
MainWindow::event(QEvent * event)
{
    if (event->type() == QEvent::WindowActivate)
        this->updateMenuBar();
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
}

void
MainWindow::dropEvent(QDropEvent * event)
{
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
MainWindow::onOrientationMarkerVisibilityChanged()
{
}

void
MainWindow::onOpenFile()
{
}

void
MainWindow::onOpenRecentFile()
{
}

void
MainWindow::onClearRecentFiles()
{
}

void
MainWindow::onNewFile()
{
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
