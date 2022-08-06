#pragma once

#include <QMainWindow>
#include <QThread>
#include <QTimer>

class Reader;
class QSettings;
class QProgressDialog;
class QFileSystemWatcher;
class QMenu;
class QActionGroup;
class QResizeEvent;
class QDragEnterEvent;
class QPushButton;
class QFrame;
class QDockWidget;
class InfoWindow;
class AboutDialog;

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

    enum EModeSelect : int {
        MODE_SELECT_NONE = 0,
        MODE_SELECT_BLOCKS = 1,
        MODE_SELECT_CELLS = 2,
        MODE_SELECT_POINTS = 3
    };

    enum EColorProfile : int {
        COLOR_PROFILE_DEFAULT = 0,
        COLOR_PROFILE_LIGHT = 1,
        COLOR_PROFILE_DARK = 2
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
    void setupExportMenu(QMenu * menu);
    void setupColorProfileMenu(QMenu * menu);
    void setupSelectModeMenu(QMenu * menu);
    void updateMenuBar();

    void updateWindowTitle();
    void connectSignals();
    void clear();
    void loadFile(const QString & file_name);

    void setupVtk();
    void setupOrientationMarker();
    void setupCubeAxesActor();

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
    void onSelectModeTriggered(QAction * action);
    void onDeselect();
    void onColorProfileTriggered(QAction *);
    void onExportAsPng();
    void onExportAsJpg();
    void onToolsExplode();
    void onExplodeValueChanged();
    void updateViewModeLocation();
    void onMinimize();
    void onBringAllToFront();
    void onShowMainWindow();
    void onAbout();

protected:
    QSettings * settings;
    LoadThread * load_thread;
    QProgressDialog * progress;
    QString file_name;
    QTimer update_timer;
    QFileSystemWatcher * file_watcher;
    ERenderMode render_mode;
    EModeSelect select_mode;
    EColorProfile color_profile_id;
    // vtk_interactor;
    QMenuBar * menu_bar;
    QMenu * recent_menu;
    QStringList recent_files;
    QMenu * export_menu;
    QMenu * view_menu;
    QPushButton * view_mode;
    QFrame * vtk_widget;
    QDockWidget * info_dock;
    InfoWindow * info_window;
    AboutDialog * about_dlg;

    QAction * new_action;
    QAction * open_action;
    QAction * close_action;
    QAction * clear_recent_file;
    QAction * shaded_action;
    QAction * shaded_w_edges_action;
    QAction * hidden_edges_removed_action;
    QAction * transluent_action;
    QAction * view_info_wnd_action;
    QAction * tools_explode_action;
    QAction * perspective_action;
    QAction * ori_marker_action;
    QAction * minimize;
    QAction * bring_all_to_front;
    QAction * show_main_window;
    QAction * about_box_action;

    QActionGroup * visual_repr;
    QActionGroup * color_profile_action_group;
    QActionGroup * mode_select_action_group;
    QActionGroup * windows_action_group;
};
