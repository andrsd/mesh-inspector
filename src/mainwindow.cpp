// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

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
#include <QApplication>
#include <QFileDialog>
#include <QVector3D>
#include <QShortcut>
#include <QStandardPaths>
#include "vtkExtractBlock.h"
#include "vtkCamera.h"
#include "aboutdlg.h"
#include "licensedlg.h"
#include "filechangednotificationwidget.h"
#include "selecttool.h"
#include "exporttool.h"
#include "explodetool.h"
#include "meshqualitytool.h"
#include "checkforupdatetool.h"
#include "blockobject.h"
#include "sidesetobject.h"
#include "nodesetobject.h"
#include "colorprofile.h"
#include "selection.h"
#include "model.h"
#include "view.h"
#include "infoview.h"
#include "common/loadfileevent.h"
#include "common/notificationwidget.h"
#include "common/infowidget.h"

static const int MAX_RECENT_FILES = 10;

MainWindow::MainWindow(QWidget * parent) :
    QMainWindow(parent),
    settings(new QSettings("David Andrs", "MeshInspector")),
    notification(nullptr),
    file_changed_notification(nullptr),
    color_profile_idx(0),
    menu_bar(new QMenuBar(nullptr)),
    recent_menu(nullptr),
    export_menu(nullptr),
    view(nullptr),
    info_view(nullptr),
    about_dlg(nullptr),
    license_dlg(nullptr),
    select_tool(new SelectTool(this)),
    export_tool(new ExportTool(this)),
    explode_tool(new ExplodeTool(this)),
    mesh_quality_tool(new MeshQualityTool(this)),
    update_tool(new CheckForUpdateTool(this)),
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
    view_license_action(nullptr),
    color_profile_action_group(nullptr),
    windows_action_group(nullptr),
    model(new Model(this)),
    progress(nullptr)
{
    auto doc_dir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    auto cwd = this->settings->value("cwd", doc_dir).toString();
    QDir::setCurrent(cwd);

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
    delete this->menu_bar;
    delete this->view;
    delete this->info_view;
    delete this->explode_tool;
    delete this->mesh_quality_tool;
    delete this->update_tool;
    for (auto & it : this->color_profiles)
        delete it;
    delete this->notification;
    delete this->file_changed_notification;
    delete this->windows_action_group;
    delete this->color_profile_action_group;
    delete this->about_dlg;
    delete this->license_dlg;
}

QSettings *
MainWindow::getSettings()
{
    return this->settings;
}

View *&
MainWindow::getView()
{
    return this->view;
}

InfoView *&
MainWindow::getInfoView()
{
    return this->info_view;
}

Model *&
MainWindow::getModel()
{
    return this->model;
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

    this->info_view = new InfoView(this);
    addDockWidget(Qt::RightDockWidgetArea, this->info_view);

    this->view->setupWidgets();
    setupFileChangedNotificationWidget();
    setupNotificationWidget();
    this->select_tool->setupWidgets();
    this->explode_tool->setupWidgets();
    this->mesh_quality_tool->setupWidgets();
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
        file_menu->addAction("New", QKeySequence("Ctrl+N"), this, &MainWindow::onNewFile);
    this->open_action =
        file_menu->addAction("Open", QKeySequence("Ctrl+O"), this, &MainWindow::onOpenFile);
    this->recent_menu = file_menu->addMenu("Open Recent");
    buildRecentFilesMenu();
    file_menu->addSeparator();
    this->export_menu = file_menu->addMenu("Export as...");
    this->export_tool->setupMenu(this->export_menu);
    file_menu->addSeparator();
    this->close_action =
        file_menu->addAction("Close", QKeySequence("Ctrl+W"), this, &MainWindow::onClose);

    // The "About" item is fine here, since we assume Mac and that will
    // place the item into different submenu but this will need to be fixed
    // for linux and windows
    file_menu->addSeparator();
    this->about_box_action = file_menu->addAction("About", this, &MainWindow::onAbout);
    this->about_box_action->setMenuRole(QAction::ApplicationSpecificRole);
    this->update_tool->setupMenu(file_menu);
    this->view_license_action =
        file_menu->addAction("View license", this, &MainWindow::onViewLicense);
    this->view_license_action->setMenuRole(QAction::ApplicationSpecificRole);

    file_menu->addAction("Quit", QKeySequence("Ctrl+Q"), this, &QCoreApplication::quit);

    QMenu * view_menu = this->menu_bar->addMenu("View");
    this->view->setMenu(view_menu);
    view_menu->addSeparator();
    this->view_info_wnd_action =
        view_menu->addAction("Info window", this, &MainWindow::onViewInfoWindow);
    this->view_info_wnd_action->setCheckable(true);
    QMenu * color_profile_menu = view_menu->addMenu("Color profile");
    setupColorProfileMenu(color_profile_menu);
    view_menu->addSeparator();
    view_menu->addAction("Reset camera",
                         QKeySequence("Ctrl+Shift+R"),
                         this->view,
                         &View::resetCamera);

    QMenu * tools_menu = this->menu_bar->addMenu("Tools");
    this->select_tool->setupMenu(tools_menu);
    this->tools_explode_action =
        tools_menu->addAction("Explode", this->explode_tool, &ExplodeTool::onExplode);
    this->tools_mesh_quality_action = tools_menu->addAction("Mesh quality",
                                                            this->mesh_quality_tool,
                                                            &MeshQualityTool::onMeshQuality);

    QMenu * window_menu = this->menu_bar->addMenu("Window");
    this->minimize =
        window_menu->addAction("Minimize", QKeySequence("Ctrl+M"), this, &MainWindow::onMinimize);
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

    this->view_info_wnd_action->setChecked(this->info_view->isVisible());
    bool has_file = this->model->hasFile();
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
    if (this->model->hasFile()) {
        auto fi = this->model->getFileInfo();
        QString title = QString("%1 \u2014 %2").arg(MESH_INSPECTOR_APP_NAME).arg(fi.fileName());
        setWindowTitle(title);
    }
    else
        setWindowTitle(MESH_INSPECTOR_APP_NAME);
}

void
MainWindow::connectSignals()
{
    connect(this->model, &Model::loadFinished, this, &MainWindow::onLoadFinished);
    connect(this->model, &Model::fileChanged, this, &MainWindow::onFileChanged);
    connect(this->model, &Model::blockAdded, this->info_view, &InfoView::onBlockAdded);
    connect(this->info_view,
            &InfoView::blockVisibilityChanged,
            this,
            &MainWindow::onBlockVisibilityChanged);
    connect(this->info_view,
            &InfoView::blockOpacityChanged,
            this,
            &MainWindow::onBlockOpacityChanged);
    connect(this->info_view, &InfoView::blockColorChanged, this, &MainWindow::onBlockColorChanged);
    connect(this->info_view,
            &InfoView::blockSelectionChanged,
            this->select_tool,
            &SelectTool::onBlockSelectionChanged);

    connect(this->model, &Model::sideSetAdded, this->info_view, &InfoView::onSideSetAdded);
    connect(this->info_view,
            &InfoView::sideSetVisibilityChanged,
            this,
            &MainWindow::onSideSetVisibilityChanged);
    connect(this->info_view,
            &InfoView::sideSetSelectionChanged,
            this->select_tool,
            &SelectTool::onSideSetSelectionChanged);

    connect(this->model, &Model::nodeSetAdded, this->info_view, &InfoView::onNodeSetAdded);
    connect(this->info_view,
            &InfoView::nodeSetVisibilityChanged,
            this,
            &MainWindow::onNodeSetVisibilityChanged);
    connect(this->info_view,
            &InfoView::nodeSetSelectionChanged,
            this->select_tool,
            &SelectTool::onNodeSetSelectionChanged);

    connect(this->info_view,
            &InfoView::dimensionsStateChanged,
            this,
            &MainWindow::onCubeAxisVisibilityChanged);
}

void
MainWindow::clear()
{
    this->mesh_quality_tool->done();
    this->model->clear();
    this->select_tool->clear();
}

void
MainWindow::loadFile(const QString & file_name)
{
    QFileInfo fi(file_name);
    if (fi.exists()) {
        this->select_tool->onDeselect();
        this->clear();

        this->progress =
            new QProgressDialog(QString("Loading %1...").arg(fi.fileName()), QString(), 0, 0, this);
        this->progress->setWindowModality(Qt::WindowModal);
        this->progress->show();

        this->model->loadFile(file_name);
    }
    else {
        auto base_file = fi.fileName();
        showNotification(QString("Unable to open '%1': File does not exist.").arg(base_file));
    }
}

int
MainWindow::getRenderWindowWidth() const
{
    int info_width = 0;
    if (this->info_view->isVisible())
        info_width = this->info_view->geometry().width();
    return this->geometry().width() - info_width;
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
    cp_default_color_map["dimensions"] = QColor(255, 255, 255);
    auto * cp_default = new ColorProfile("Default", cp_default_color_map);
    this->color_profiles.push_back(cp_default);

    std::map<QString, QColor> cp_light_color_map;
    cp_light_color_map["bkgnd"] = QColor(255, 255, 255);
    cp_light_color_map["color_bar_label"] = QColor(0, 0, 0);
    cp_light_color_map["dimensions"] = QColor(0, 0, 0);
    auto * cp_light = new ColorProfile("Light", cp_light_color_map);
    this->color_profiles.push_back(cp_light);

    std::map<QString, QColor> cp_dark_color_map;
    cp_dark_color_map["bkgnd"] = QColor(0, 0, 0);
    cp_dark_color_map["color_bar_label"] = QColor(255, 255, 255);
    cp_dark_color_map["dimensions"] = QColor(255, 255, 255);
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
        this->model->resetCameraOnLoad(true);
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
        if (file_names.length() > 0) {
            this->model->resetCameraOnLoad(true);
            loadFile(file_names[0]);
        }
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
    this->settings->setValue("cwd", QDir::currentPath());
    QMainWindow::closeEvent(event);
}

void
MainWindow::onClose()
{
    clear();
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
    hideLoadProgressBar();
    if (this->model->hasValidFile()) {
        update();
        showNormal();
        this->update_timer.start(250);
    }
    else {
        auto fi = this->model->getFileInfo();
        showNotification(QString("Unsupported file '%1'.").arg(fi.fileName()));
    }
    this->updateMenuBar();
}

void
MainWindow::update()
{
    auto file_name = this->model->getFileName();
    updateWindowTitle();
    addToRecentFiles(file_name);
    buildRecentFilesMenu();
    this->file_changed_notification->setFileName(file_name);

    this->select_tool->update();
    this->mesh_quality_tool->update();
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
    auto cwd = QDir::currentPath();
    QString file_name =
        QFileDialog::getOpenFileName(this,
                                     "Open File",
                                     cwd,
                                     "Supported files (*.e *.exo *.msh *.obj *.stl *.vtk *.vtu);;"
                                     "All files (*);;"
                                     "ExodusII files (*.e *.exo);;"
                                     "GMSH mesh files (*.msh);;"
                                     "Wavefront OBJ files (*.obj);;"
                                     "STL files (*.stl);;"
                                     "VTK Unstructured grid files (*.vtk);;"
                                     "VTU Unstructured grid files (*.vtu)");
    if (!file_name.isNull()) {
        this->model->resetCameraOnLoad(true);
        loadFile(file_name);
        auto fi = QFileInfo(file_name);
        QDir::setCurrent(fi.canonicalPath());
    }
}

void
MainWindow::onOpenRecentFile()
{
    auto action = dynamic_cast<QAction *>(this->sender());
    if (action != nullptr) {
        this->model->resetCameraOnLoad(true);
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
    showFileChangedNotification();
}

void
MainWindow::onReloadFile()
{
    this->model->resetCameraOnLoad(false);
    QString file_name = this->model->getFileName();
    loadFile(file_name);
}

void
MainWindow::onClicked(const QPoint & pt)
{
    this->select_tool->onClicked(pt);
}

void
MainWindow::onMouseMove(const QPoint & pt)
{
    if (this->model->hasFile())
        this->select_tool->onMouseMove(pt);
}

void
MainWindow::onViewInfoWindow()
{
    if (this->info_view->isVisible())
        this->info_view->hide();
    else
        this->info_view->show();
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
