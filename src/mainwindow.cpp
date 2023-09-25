#include "mainwindow.h"
#include "meshinspectorconfig.h"
#include <QLabel>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QString>
#include <QProgressDialog>
#include <QFileSystemWatcher>
#include <QActionGroup>
#include <QSettings>
#include <QEvent>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QPushButton>
#include <QFileInfo>
#include <QDockWidget>
#include <QApplication>
#include <QFileDialog>
#include <QVector3D>
#include <QShortcut>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include "vtkExtractBlock.h"
#include "vtkCamera.h"
#include "infowindow.h"
#include "aboutdlg.h"
#include "licensedlg.h"
#include "filechangednotificationwidget.h"
#include "selecttool.h"
#include "exporttool.h"
#include "explodetool.h"
#include "meshqualitytool.h"
#include "reader.h"
#include "blockobject.h"
#include "sidesetobject.h"
#include "nodesetobject.h"
#include "exodusiireader.h"
#include "vtkreader.h"
#include "stlreader.h"
#include "colorprofile.h"
#include "selection.h"
#include "view.h"
#include "common/loadfileevent.h"
#include "common/notificationwidget.h"
#include "common/infowidget.h"

static const int MAX_RECENT_FILES = 10;

// Main window - Load thread

MainWindow::LoadThread::LoadThread(const QString & file_name) :
    QThread(),
    file_name(file_name),
    reader(nullptr)
{
    if (file_name.endsWith(".e") || file_name.endsWith(".exo"))
        this->reader = new ExodusIIReader(file_name.toStdString());
    else if (file_name.endsWith(".vtk"))
        this->reader = new VTKReader(file_name.toStdString());
    else if (file_name.endsWith(".stl"))
        this->reader = new STLReader(file_name.toStdString());
    else
        this->reader = nullptr;
}

MainWindow::LoadThread::~LoadThread()
{
    delete this->reader;
}

Reader *
MainWindow::LoadThread::getReader()
{
    return this->reader;
}

bool
MainWindow::LoadThread::hasValidFile()
{
    return this->reader != nullptr;
}

const QString &
MainWindow::LoadThread::getFileName()
{
    return this->file_name;
}

void
MainWindow::LoadThread::run()
{
    if (this->reader != nullptr)
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
    file_changed_notification(nullptr),
    color_profile_idx(0),
    menu_bar(new QMenuBar(nullptr)),
    recent_menu(nullptr),
    export_menu(nullptr),
    view(nullptr),
    info_dock(nullptr),
    info_window(nullptr),
    about_dlg(nullptr),
    license_dlg(nullptr),
    select_tool(new SelectTool(this)),
    export_tool(new ExportTool(this)),
    explode_tool(new ExplodeTool(this)),
    mesh_quality_tool(new MeshQualityTool(this)),
    new_action(nullptr),
    open_action(nullptr),
    close_action(nullptr),
    clear_recent_file(nullptr),
    view_info_wnd_action(nullptr),
    tools_explode_action(nullptr),
    tools_mesh_quality_action(nullptr),
    minimize(nullptr),
    bring_all_to_front(nullptr),
    show_main_window(nullptr),
    about_box_action(nullptr),
    check_update_action(nullptr),
    view_license_action(nullptr),
    color_profile_action_group(nullptr),
    windows_action_group(nullptr),
    namgr(new QNetworkAccessManager())
{
    connect(this->namgr, &QNetworkAccessManager::finished, this, &MainWindow::onHttpReply);

    QSize default_size = QSize(1000, 700);
    QVariant geom = this->settings->value("window/geometry", default_size);
    if (!this->restoreGeometry(geom.toByteArray()))
        this->resize(default_size);
    this->recent_files = this->settings->value("recent_files", QStringList()).toStringList();
    loadColorProfiles();

    setupWidgets();
    setupMenuBar();
    updateWindowTitle();
    updateMenuBar();

    setAcceptDrops(true);

    connectSignals();
    this->view->setupVtk();
    this->mesh_quality_tool->setupVtk();
    setColorProfile();

    clear();
    show();

    connect(&this->update_timer, &QTimer::timeout, this, &MainWindow::onUpdateWindow);
    QTimer::singleShot(1, this, &MainWindow::updateViewModeLocation);
}

MainWindow::~MainWindow()
{
    clear();
    delete this->settings;
    delete this->file_watcher;
    delete this->menu_bar;
    delete this->view;
    delete this->info_window;
    delete this->info_dock;
    delete this->explode_tool;
    delete this->mesh_quality_tool;
    for (auto & it : this->color_profiles)
        delete it;
    delete this->load_thread;
    delete this->notification;
    delete this->file_changed_notification;
    delete this->windows_action_group;
    delete this->color_profile_action_group;
    delete this->about_dlg;
    delete this->license_dlg;
    delete this->namgr;
}

const std::map<int, BlockObject *> &
MainWindow::getBlocks() const
{
    return this->blocks;
}

const std::map<int, SideSetObject *> &
MainWindow::getSideSets() const
{
    return this->side_sets;
}

const std::map<int, NodeSetObject *> &
MainWindow::getNodeSets() const
{
    return this->node_sets;
}

const vtkVector3d &
MainWindow::getCenterOfBounds() const
{
    return this->center_of_bounds;
}

QSettings *
MainWindow::getSettings()
{
    return this->settings;
}

View *
MainWindow::getView()
{
    return this->view;
}

const BlockObject *
MainWindow::getSelectedBlock()
{
    return this->select_tool->getSelectedBlock();
}

void
MainWindow::setupWidgets()
{
    this->view = new View(this);
    setCentralWidget(this->view);

    this->info_window = new InfoWindow();

    this->info_dock = new QDockWidget("Information", this);
    this->info_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    this->info_dock->setFloating(false);
    this->info_dock->setFeatures(QDockWidget::DockWidgetMovable);
    this->info_dock->setWidget(this->info_window);
    addDockWidget(Qt::RightDockWidgetArea, this->info_dock);

    this->view->setupWidgets();
    setupFileChangedNotificationWidget();
    setupNotificationWidget();
    this->select_tool->setupWidgets();

    this->explode_tool->setupWidgets();
    this->mesh_quality_tool->setupWidgets();
}

void
MainWindow::setupViewModeWidget(QMainWindow * wnd)
{
}

void
MainWindow::setupNotificationWidget()
{
    this->notification = new NotificationWidget(this);
    this->notification->setVisible(false);
}

void
MainWindow::setupFileChangedNotificationWidget()
{
    this->file_changed_notification = new FileChangedNotificationWidget(this);
    this->file_changed_notification->setVisible(false);
    connect(this->file_changed_notification,
            &FileChangedNotificationWidget::reloaded,
            this,
            &MainWindow::onReloadFile);
}

void
MainWindow::setupMenuBar()
{
    setMenuBar(this->menu_bar);
    QMenu * file_menu = this->menu_bar->addMenu("File");
    this->new_action =
        file_menu->addAction("New", this, &MainWindow::onNewFile, QKeySequence("Ctrl+N"));
    this->open_action =
        file_menu->addAction("Open", this, &MainWindow::onOpenFile, QKeySequence("Ctrl+O"));
    this->recent_menu = file_menu->addMenu("Open Recent");
    buildRecentFilesMenu();
    file_menu->addSeparator();
    this->export_menu = file_menu->addMenu("Export as...");
    this->export_tool->setupMenu(this->export_menu);
    file_menu->addSeparator();
    this->close_action =
        file_menu->addAction("Close", this, &MainWindow::onClose, QKeySequence("Ctrl+W"));

    // The "About" item is fine here, since we assume Mac and that will
    // place the item into different submenu but this will need to be fixed
    // for linux and windows
    file_menu->addSeparator();
    this->about_box_action = file_menu->addAction("About", this, &MainWindow::onAbout);
    this->about_box_action->setMenuRole(QAction::ApplicationSpecificRole);
    this->check_update_action =
        file_menu->addAction("Check for update...", this, &MainWindow::onCheckForUpdate);
    this->check_update_action->setMenuRole(QAction::ApplicationSpecificRole);
    this->view_license_action =
        file_menu->addAction("View license", this, &MainWindow::onViewLicense);
    this->view_license_action->setMenuRole(QAction::ApplicationSpecificRole);

    file_menu->addAction("Quit", this, &QCoreApplication::quit, QKeySequence("Ctrl+Q"));

    QMenu * view_menu = this->menu_bar->addMenu("View");
    this->view->setMenu(view_menu);
    view_menu->addSeparator();
    this->view_info_wnd_action =
        view_menu->addAction("Info window", this, &MainWindow::onViewInfoWindow);
    this->view_info_wnd_action->setCheckable(true);
    QMenu * color_profile_menu = view_menu->addMenu("Color profile");
    setupColorProfileMenu(color_profile_menu);

    QMenu * tools_menu = this->menu_bar->addMenu("Tools");
    this->select_tool->setupMenu(tools_menu);
    this->tools_explode_action =
        tools_menu->addAction("Explode", this->explode_tool, &ExplodeTool::onExplode);
    this->tools_mesh_quality_action =
        tools_menu->addAction("Mesh quality", this->mesh_quality_tool, &MeshQualityTool::onMeshQuality);

    QMenu * window_menu = this->menu_bar->addMenu("Window");
    this->minimize =
        window_menu->addAction("Minimize", this, &MainWindow::onMinimize, QKeySequence("Ctrl+M"));
    window_menu->addSeparator();
    this->bring_all_to_front =
        window_menu->addAction("Bring All to Front", this, &MainWindow::onBringAllToFront);
    window_menu->addSeparator();
    this->show_main_window =
        window_menu->addAction("Mesh Inspector", this, &MainWindow::onShowMainWindow);
    this->show_main_window->setCheckable(true);

    this->windows_action_group = new QActionGroup(this);
    this->windows_action_group->addAction(this->show_main_window);
}

void
MainWindow::setupColorProfileMenu(QMenu * menu)
{
    this->color_profile_action_group = new QActionGroup(this);
    this->color_profile_idx = this->settings->value("color_profile", 0).toInt();

    for (std::size_t id = 0; id < this->color_profiles.size(); id++) {
        auto * cp = this->color_profiles[id];
        auto * action = menu->addAction(cp->getName());
        action->setCheckable(true);
        action->setData((uint) id);
        this->color_profile_action_group->addAction(action);
        if (id == this->color_profile_idx)
            action->setChecked(true);
    }

    connect(this->color_profile_action_group,
            &QActionGroup::triggered,
            this,
            &MainWindow::onColorProfileTriggered);
}

void
MainWindow::updateMenuBar()
{
    auto * active_window = QApplication::activeWindow();
    this->show_main_window->setChecked(active_window == this);

    this->view_info_wnd_action->setChecked(this->info_window->isVisible());
    bool has_file = hasFile();
    this->export_tool->setMenuEnabled(has_file);
    this->tools_explode_action->setEnabled(has_file);
    this->tools_mesh_quality_action->setEnabled(has_file);
    this->close_action->setEnabled(has_file);

    bool showing_mesh_quality = this->mesh_quality_tool->isVisible();
    this->view->updateMenuBar(!showing_mesh_quality);
}

void
MainWindow::updateWindowTitle()
{
    if (hasFile()) {
        QFileInfo fi(this->file_name);
        QString title = QString("%1 \u2014 %2").arg(MESH_INSPECTOR_APP_NAME).arg(fi.fileName());
        setWindowTitle(title);
    }
    else
        setWindowTitle(MESH_INSPECTOR_APP_NAME);
}

void
MainWindow::connectSignals()
{
    connect(this, &MainWindow::blockAdded, this->info_window, &InfoWindow::onBlockAdded);
    connect(this->info_window,
            &InfoWindow::blockVisibilityChanged,
            this,
            &MainWindow::onBlockVisibilityChanged);
    connect(this->info_window,
            &InfoWindow::blockOpacityChanged,
            this,
            &MainWindow::onBlockOpacityChanged);
    connect(this->info_window,
            &InfoWindow::blockColorChanged,
            this,
            &MainWindow::onBlockColorChanged);
    connect(this->info_window,
            &InfoWindow::blockSelectionChanged,
            this->select_tool,
            &SelectTool::onBlockSelectionChanged);

    connect(this, &MainWindow::sideSetAdded, this->info_window, &InfoWindow::onSideSetAdded);
    connect(this->info_window,
            &InfoWindow::sideSetVisibilityChanged,
            this,
            &MainWindow::onSideSetVisibilityChanged);
    connect(this->info_window,
            &InfoWindow::sideSetSelectionChanged,
            this->select_tool,
            &SelectTool::onSideSetSelectionChanged);

    connect(this, &MainWindow::nodeSetAdded, this->info_window, &InfoWindow::onNodeSetAdded);
    connect(this->info_window,
            &InfoWindow::nodeSetVisibilityChanged,
            this,
            &MainWindow::onNodeSetVisibilityChanged);
    connect(this->info_window,
            &InfoWindow::nodeSetSelectionChanged,
            this->select_tool,
            &SelectTool::onNodeSetSelectionChanged);

    connect(this->info_window,
            &InfoWindow::dimensionsStateChanged,
            this,
            &MainWindow::onCubeAxisVisibilityChanged);

    connect(this->file_watcher, &QFileSystemWatcher::fileChanged, this, &MainWindow::onFileChanged);
}

void
MainWindow::clear()
{
    this->mesh_quality_tool->done();
    this->view->clear();

    for (auto & it : this->blocks)
        delete it.second;
    this->blocks.clear();

    for (auto & it : this->side_sets)
        delete it.second;
    this->side_sets.clear();

    for (auto & it : this->node_sets)
        delete it.second;
    this->node_sets.clear();

    for (auto & eb : this->extract_blocks)
        eb->Delete();
    this->extract_blocks.clear();

    auto watched_files = this->file_watcher->files();
    for (auto & file : watched_files)
        this->file_watcher->removePath(file);

    this->select_tool->clear();
}

void
MainWindow::loadFile(const QString & file_name)
{
    this->select_tool->onDeselect();
    this->clear();
    if (!this->checkFileExists(file_name))
        return;

    QFileInfo fi(file_name);
    this->progress =
        new QProgressDialog(QString("Loading %1...").arg(fi.fileName()), QString(), 0, 0, this);
    this->progress->setWindowModality(Qt::WindowModal);
    this->progress->show();

    delete this->load_thread;
    this->load_thread = new LoadThread(file_name);
    connect(this->load_thread, &LoadThread::finished, this, &MainWindow::onLoadFinished);
    this->load_thread->start(QThread::IdlePriority);
}

bool
MainWindow::hasFile()
{
    return !this->file_name.isEmpty();
}

void
MainWindow::computeTotalBoundingBox()
{
    vtkBoundingBox bbox;
    for (auto & it : this->blocks) {
        auto block = it.second;
        bbox.AddBounds(block->getBounds());
    }

    double bounds[6];
    bbox.GetBounds(bounds);
    this->view->setTotalBoundingBox(bounds);
    this->info_window->setBounds(bounds[0], bounds[1], bounds[2], bounds[3], bounds[4], bounds[5]);

    double center[3];
    bbox.GetCenter(center);
    this->center_of_bounds = vtkVector3d(center[0], center[1], center[2]);
}

int
MainWindow::getRenderWindowWidth() const
{
    int info_width = 0;
    if (this->info_dock->isVisible())
        info_width = this->info_dock->geometry().width();
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
    auto * camera = this->view->getActiveCamera();
    auto * reader = this->load_thread->getReader();

    for (auto & binfo : reader->getBlocks()) {
        BlockObject * block = nullptr;
        if (binfo.multiblock_index != -1) {
            vtkExtractBlock * eb = vtkExtractBlock::New();
            eb->SetInputConnection(reader->getVtkOutputPort());
            eb->AddIndex(binfo.multiblock_index);
            eb->Update();
            this->extract_blocks.push_back(eb);

            block = new BlockObject(eb->GetOutputPort(), camera);
        }
        else
            block = new BlockObject(reader->getVtkOutputPort(), camera);
        this->blocks[binfo.number] = block;
        this->view->addBlock(block);
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
        this->extract_blocks.push_back(eb);

        auto sideset = new SideSetObject(eb->GetOutputPort());
        this->side_sets[finfo.number] = sideset;
        this->view->addSideSet(sideset);
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
        this->extract_blocks.push_back(eb);

        auto * nodeset = new NodeSetObject(eb->GetOutputPort());
        this->node_sets[ninfo.number] = nodeset;
        this->view->addNodeSet(nodeset);
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
MainWindow::showNotification(const QString & text, int ms)
{
    this->notification->setText(text);
    this->notification->adjustSize();
    auto width = geometry().width();
    auto left = (width - this->notification->width()) / 2;
    auto top = height() - this->notification->height() - 10;
    this->notification->setGeometry(left,
                                    top,
                                    this->notification->width(),
                                    this->notification->height());
    this->notification->setGraphicsEffect(nullptr);
    this->notification->show(ms);
}

void
MainWindow::showFileChangedNotification()
{
    this->file_changed_notification->adjustSize();
    auto width = getRenderWindowWidth();
    int left = (width - this->file_changed_notification->width()) / 2;
    int top = 10;
    this->file_changed_notification->setGeometry(left,
                                                 top,
                                                 this->file_changed_notification->width(),
                                                 this->file_changed_notification->height());
    this->file_changed_notification->show();
}

int
MainWindow::blockActorToId(vtkActor * actor)
{
    // TODO: when we start to have 1000s of actors, this should be done via a
    // map from 'actor' to 'block_id'
    for (auto & it : this->blocks) {
        auto * block = it.second;
        if (block->getActor() == actor)
            return it.first;
    }
    return -1;
}

void
MainWindow::setColorProfile()
{
    ColorProfile * profile;
    if (this->color_profile_idx < this->color_profiles.size())
        profile = this->color_profiles[this->color_profile_idx];
    else
        profile = this->color_profiles[0];

    this->view->setColorProfile(profile);
    this->mesh_quality_tool->setColorProfile(profile);
}

void
MainWindow::loadColorProfiles()
{
    // Maybe store these as resources and pull it from there
    std::map<QString, QColor> cp_default_color_map;
    cp_default_color_map["bkgnd"] = QColor(82, 87, 110);
    cp_default_color_map["color_bar_label"] = QColor(255, 255, 255);
    auto * cp_default = new ColorProfile("Default", cp_default_color_map);
    this->color_profiles.push_back(cp_default);

    std::map<QString, QColor> cp_light_color_map;
    cp_light_color_map["bkgnd"] = QColor(255, 255, 255);
    cp_light_color_map["color_bar_label"] = QColor(0, 0, 0);
    auto * cp_light = new ColorProfile("Light", cp_light_color_map);
    this->color_profiles.push_back(cp_light);

    std::map<QString, QColor> cp_dark_color_map;
    cp_dark_color_map["bkgnd"] = QColor(0, 0, 0);
    cp_dark_color_map["color_bar_label"] = QColor(255, 255, 255);
    auto * cp_dark = new ColorProfile("Dark", cp_dark_color_map);
    this->color_profiles.push_back(cp_dark);
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
                this->recent_menu->addAction(fi.fileName(), this, &MainWindow::onOpenRecentFile);
            action->setData(f);
        }
        this->recent_menu->addSeparator();
    }
    this->clear_recent_file =
        this->recent_menu->addAction("Clear Menu", this, &MainWindow::onClearRecentFiles);
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
    this->explode_tool->updateLocation();
    this->mesh_quality_tool->updateLocation();
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
    this->select_tool->saveSettings(this->settings);
    this->settings->setValue("color_profile", (uint) this->color_profile_idx);
    this->settings->setValue("window/geometry", this->saveGeometry());
    this->settings->setValue("recent_files", this->recent_files);
    QMainWindow::closeEvent(event);
}

void
MainWindow::onClose()
{
    clear();
    this->info_window->clear();
    this->file_name = QString();
    hide();
    this->update_timer.stop();
    updateMenuBar();
}

void
MainWindow::hideLoadProgressBar()
{
    this->progress->hide();
    delete this->progress;
    this->progress = nullptr;
}

void
MainWindow::onLoadFinished()
{
    if (this->load_thread->hasValidFile()) {
        loadIntoVtk();
        showNormal();
        this->update_timer.start(250);
    }
    else {
        QFileInfo fi(this->load_thread->getFileName());
        showNotification(QString("Unsupported file '%1'.").arg(fi.fileName()));
    }
    hideLoadProgressBar();
    this->updateMenuBar();
}

void
MainWindow::loadIntoVtk()
{
    Reader * reader = this->load_thread->getReader();

    this->info_window->clear();
    this->info_window->init();
    addBlocks();
    addSideSets();
    addNodeSets();

    computeTotalBoundingBox();

    this->info_window->setSummary(reader->getTotalNumberOfElements(),
                                  reader->getTotalNumberOfNodes());

    this->file_name = QString(reader->getFileName().c_str());
    updateWindowTitle();
    addToRecentFiles(this->file_name);
    buildRecentFilesMenu();
    this->file_watcher->addPath(this->file_name);
    this->file_changed_notification->setFileName(this->file_name);

    this->select_tool->loadIntoVtk(reader->getVtkOutputPort());
    this->mesh_quality_tool->loadIntoVtk();

    this->view->setInteractorStyle(reader->getDimensionality());
    this->view->resetCamera();
}

void
MainWindow::onBlockVisibilityChanged(int block_id, bool visible)
{
    this->view->setBlockVisibility(block_id, visible);
}

void
MainWindow::onBlockOpacityChanged(int block_id, double opacity)
{
    this->view->setBlockOpacity(block_id, opacity);
}

void
MainWindow::onBlockColorChanged(int block_id, QColor color)
{
    this->view->setBlockColor(block_id, color);
}

void
MainWindow::onSideSetVisibilityChanged(int sideset_id, bool visible)
{
    this->view->setSideSetVisibility(sideset_id, visible);
}

void
MainWindow::onNodeSetVisibilityChanged(int nodeset_id, bool visible)
{
    this->view->setNodeSetVisibility(nodeset_id, visible);
}

void
MainWindow::onCubeAxisVisibilityChanged(bool visible)
{
    this->view->setCubeAxisVisibility(visible);
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
    auto action = dynamic_cast<QAction *>(this->sender());
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
    this->select_tool->onDeselect();
    this->clear();
    this->info_window->clear();
    this->file_name = QString();
    this->updateWindowTitle();
    showNormal();
}

void
MainWindow::onUpdateWindow()
{
    this->view->render();
}

void
MainWindow::onFileChanged(const QString & path)
{
    if (!this->file_watcher->files().contains(path))
        this->file_watcher->addPath(path);
    showFileChangedNotification();
}

void
MainWindow::onReloadFile()
{
    loadFile(this->file_name);
}

void
MainWindow::onClicked(const QPoint & pt)
{
    this->select_tool->onClicked(pt);
}

void
MainWindow::onMouseMove(const QPoint & pt)
{
    if (hasFile())
        this->select_tool->onMouseMove(pt);
}

void
MainWindow::onViewInfoWindow()
{
    if (this->info_dock->isVisible())
        this->info_dock->hide();
    else
        this->info_dock->show();
    this->updateMenuBar();
    updateViewModeLocation();
}

void
MainWindow::onColorProfileTriggered(QAction * action)
{
    action->setChecked(true);
    this->color_profile_idx = action->data().toInt();
    setColorProfile();
}

void
MainWindow::updateViewModeLocation()
{
    this->view->updateLocation();
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

void
MainWindow::onViewLicense()
{
    if (this->license_dlg == nullptr)
        this->license_dlg = new LicenseDialog(this);
    this->license_dlg->show();
}

void
MainWindow::onCheckForUpdate()
{
    QNetworkRequest request;
    request.setUrl(QUrl("https://api.github.com/repos/andrsd/mesh-inspector/releases/latest"));
    request.setRawHeader(QByteArray("Accept"), QByteArray("application/vnd.github+json"));
    request.setRawHeader(QByteArray("Authorization"),
                         QByteArray("Bearer "
                                    "github_pat_11AAAU5AA0uy230CCje7ng_"
                                    "8pj7WNyfPIj8TZZRv7ZSF4RmpULmyRtN0Y9glzoh0bePPT6765WCoolMPhf"));
    this->namgr->get(request);
}

void
MainWindow::onHttpReply(QNetworkReply * reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QVersionNumber latest = getVersionFromReply(reply);
        QVersionNumber current(MESH_INSPECTOR_MAJOR_VERSION, MESH_INSPECTOR_MINOR_VERSION);

        if (QVersionNumber::compare(latest, current) == 0)
            showNotification("You are running the latest version.");
        else
            showNotification(QString("A newer version (%1.%2) is available.")
                                 .arg(latest.majorVersion())
                                 .arg(latest.minorVersion()));
    }
    else
        showNotification("Check for a new version failed.");
}

QVersionNumber
MainWindow::getVersionFromReply(QNetworkReply * reply)
{
    QByteArray val = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(val);
    auto tag_name = doc["tag_name"].toString();
    QRegularExpression re("v(\\d+)\\.(\\d+)");
    QRegularExpressionMatch match = re.match(tag_name);
    if (match.hasMatch())
        return QVersionNumber(match.captured(1).toInt(), match.captured(2).toInt());
    else
        return QVersionNumber(-1);
}

void
MainWindow::activateRenderMode()
{
    this->view->activateRenderMode();
}
