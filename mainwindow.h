#pragma once

#include <QMainWindow>
#include <QThread>
#include <QTimer>

class Reader;
class QSettings;
class QProgressDialog;
class QFileSystemWatcher;
class QMenu;
class QResizeEvent;
class QDragEnterEvent;

class MainWindow : public QMainWindow {
    Q_OBJECT

protected:
    class LoadThread : public QThread {
    public:
        LoadThread(const QString & file_name);

        virtual Reader * getReader();

    protected:
        virtual void run();

        Reader * reader;
    };

    enum ERenderMode {
        SHADED = 0,
        SHADED_WITH_EDGES = 1,
        HIDDEN_EDGES_REMOVED = 2,
        TRANSLUENT = 3
    };

public:
    explicit MainWindow(QWidget * parent = nullptr);
    virtual ~MainWindow();

protected:
    void setupWidgets();
    void setupViewModeWidget(QMainWindow * wnd);
    void setupNotificationWidget();
    void setupFileChangedNotificationWidget();
    void setupExplodeWidgets();

    void setupMenuBar();
    void setupExportMenu();
    void setupColorProfileMenu();
    void setupSelectModeMenu(QMenu & tools_menu);
    void updateMenuBar();

    void updateWindowTitle();
    void connectSignals();
    void clear();
    void loadFile(const QString & file_name);

    void setupVtk();
    void setupOrientationMarker();
    void setupCubeAxesActor();

    void updateViewModeLocation();

    int getRenderWindowWidth() const;
    bool checkFileExists(const QString & file_name) const;
    void addBlocks();
    void addSideSets();
    void addNodeSets();
    // A getBlock(block_id);
    // A getSideSet(sideset_id);
    // A getNodeSet(nodeset_id);
    ERenderMode getRenderMode();
    void setSelectedBlockProperties();
    void setDeselectedBlockProperties();
    void setBlockProperties();
    void setSideSetProperties();
    void setNodeSetProperties();
    void setSelectionProperties();
    void showNotification(const QString & text, int ms = 5000);
    void showFileChangedNotification();
    void showSelectedMeshEntity();
    void deselectBlocks();
    // void blockActorToId(actor);
    void selectBlock(const QPoint & pt);
    void selectCell(const QPoint & pt);
    void selectPoint(const QPoint & pt);
    void setColorProfile();
    void loadColorProfiles();
    QString getFileName(/*window_title, name_filter, default_suffix*/);
    void buildRecentFilesMenu();
    void addToRecentFiles(const QString & file_name);

    virtual bool event(QEvent * event);
    virtual void resizeEvent(QResizeEvent * event);
    virtual void dragEnterEvent(QDragEnterEvent * event);
    virtual void dropEvent(QDropEvent * event);
    virtual void closeEvent(QCloseEvent * event);

public slots:
    void onClose();
    void onLoadFinished();
    void onBlockVisibilityChanged();
    void onBlockColorChanged();
    void onSidesetVisibilityChanged();
    void onNodesetVisibilityChanged();
    void onCubeAxisVisibilityChanged();
    void onOrientationMarkerVisibilityChanged();
    void onOpenFile();
    void onOpenRecentFile();
    void onClearRecentFiles();
    void onNewFile();
    void onShadedTriggered(bool checked);
    void onShadedWithEdgesTriggered(bool checked);
    void onHiddenEdgesRemovedTriggered(bool checked);
    void onTransluentTriggered(bool checked);
    void onPerspectiveToggled(bool checked);
    void onUpdateWindow();
    void onFileChanged();
    void onReloadFile();
    void onBlockSelectionChanged();
    void onSidesetSelectionChanged();
    void onNodesetSelectionChanged();
    void onClicked(const QPoint & pt);
    void onViewInfoWindow();
    void onSelectModeTriggered();
    void onDeselect();
    void onColorProfileTriggered();
    void onExportAsPng();
    void onExportAsJpg();
    void onToolsExplode();
    void onExplodeValueChanged();

protected:
    QSettings * settings;
    LoadThread * load_thread;
    QProgressDialog * progress;
    QString file_name;
    QTimer update_timer;
    QFileSystemWatcher * file_watcher;
    ERenderMode render_mode;
    // vtk_interactor;
    QMenuBar * menu_bar;
    QMenu * recent_menu;
    QStringList recent_files;
};
