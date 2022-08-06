#include "mainwindow.h"
#include <QLabel>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QString>
#include <QProgressDialog>
#include <QFileSystemWatcher>
#include <QMenu>
#include <QSettings>
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
    settings(new QSettings("name.", "MeshInspector")),
    load_thread(nullptr),
    progress(nullptr),
    file_name(),
    file_watcher(new QFileSystemWatcher()),
    render_mode(SHADED_WITH_EDGES),
    menu_bar(new QMenuBar(nullptr)),
    recent_menu(nullptr)
{
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

    // this->update_timer = QTimer();
    // this->update_timer.timeout.connect(self.onUpdateWindow);
    this->update_timer.start(250);

    // QTimer::singleShot(1, self._updateViewModeLocation)
}

MainWindow::~MainWindow()
{
    delete this->settings;
    delete this->file_watcher;
    delete this->menu_bar;
}

void
MainWindow::setupWidgets()
{
}

void
MainWindow::setupViewModeWidget(QMainWindow * wnd)
{
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
}

void
MainWindow::setupExportMenu()
{
}

void
MainWindow::setupColorProfileMenu()
{
}

void
MainWindow::setupSelectModeMenu(QMenu & tools_menu)
{
}

void
MainWindow::updateMenuBar()
{
}

void
MainWindow::updateWindowTitle()
{
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

void
MainWindow::updateViewModeLocation()
{
}

int
MainWindow::getRenderWindowWidth() const
{
    return 0;
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
}

void
MainWindow::addToRecentFiles(const QString & file_name)
{
}

bool
MainWindow::event(QEvent * event)
{
    return false;
}

void
MainWindow::resizeEvent(QResizeEvent * event)
{
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
MainWindow::onSelectModeTriggered()
{
}

void
MainWindow::onDeselect()
{
}

void
MainWindow::onColorProfileTriggered()
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
