#pragma once

#include <QMainWindow>
#include <QThread>
#include <QTimer>
#include <QColor>
#include "vtkVector.h"
#include <vector>

class Reader;
class QSettings;
class QProgressDialog;
class QFileSystemWatcher;
class QMenu;
class QActionGroup;
class QResizeEvent;
class QDragEnterEvent;
class QPushButton;
class View;
class QDockWidget;
class QShortcut;
class InfoWindow;
class AboutDialog;
class LicenseDialog;
class NotificationWidget;
class SelectTool;
class ExportTool;
class ExplodeTool;
class MeshQualityTool;
class CheckForUpdateTool;
class FileChangedNotificationWidget;
class vtkActor;
class vtkExtractBlock;
class BlockObject;
class SideSetObject;
class NodeSetObject;
class ColorProfile;
class InfoWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

protected:
    class LoadThread : public QThread {
    public:
        explicit LoadThread(const QString & file_name);
        ~LoadThread() override;

        virtual Reader * getReader();
        bool hasValidFile();
        const QString & getFileName();

    protected:
        void run() override;

        QString file_name;
        Reader * reader;
    };

public:
    explicit MainWindow(QWidget * parent = nullptr);
    ~MainWindow() override;

    int getRenderWindowWidth() const;
    const std::map<int, BlockObject *> & getBlocks() const;
    const std::map<int, SideSetObject *> & getSideSets() const;
    const std::map<int, NodeSetObject *> & getNodeSets() const;
    const vtkVector3d & getCenterOfBounds() const;
    void updateMenuBar();
    void activateRenderMode();
    void showNotification(const QString & text, int ms = 5000);
    QSettings * getSettings();
    int blockActorToId(vtkActor * actor);
    BlockObject * getBlock(int block_id);
    SideSetObject * getSideSet(int sideset_id);
    NodeSetObject * getNodeSet(int nodeset_id);
    View * getView();
    const BlockObject * getSelectedBlock();

    template <typename T>
    inline qreal
    HIDPI(T value) const
    {
        return devicePixelRatio() * value;
    }

signals:
    void blockAdded(int id, const QString & name);
    void sideSetAdded(int id, const QString & name);
    void nodeSetAdded(int id, const QString & name);

protected:
    void setupWidgets();
    void setupViewModeWidget(QMainWindow * wnd);
    void setupNotificationWidget();
    void setupFileChangedNotificationWidget();
    void setupMenuBar();
    void setupColorProfileMenu(QMenu * menu);

    void updateWindowTitle();
    void connectSignals();
    void clear();
    void loadFile(const QString & file_name);
    bool hasFile();

    void setupOrientationMarker();
    void setupCubeAxesActor();
    void computeTotalBoundingBox();

    bool checkFileExists(const QString & file_name);
    void addBlocks();
    void addSideSets();
    void addNodeSets();
    void showFileChangedNotification();
    void setColorProfile();
    void loadColorProfiles();
    void buildRecentFilesMenu();
    void addToRecentFiles(const QString & file_name);
    QString cellTypeToName(int cell_type);
    void hideLoadProgressBar();
    void loadIntoVtk();

    bool event(QEvent * event) override;
    void customEvent(QEvent * event) override;
    void resizeEvent(QResizeEvent * event) override;
    void dragEnterEvent(QDragEnterEvent * event) override;
    void dropEvent(QDropEvent * event) override;
    void closeEvent(QCloseEvent * event) override;

public slots:
    void onClose();
    void onLoadFinished();
    void onBlockVisibilityChanged(int block_id, bool visible);
    void onBlockOpacityChanged(int block_id, double opacity);
    void onBlockColorChanged(int block_id, QColor color);
    void onSideSetVisibilityChanged(int sideset_id, bool visible);
    void onNodeSetVisibilityChanged(int nodeset_id, bool visible);
    void onCubeAxisVisibilityChanged(bool visible);
    void onOpenFile();
    void onOpenRecentFile();
    void onClearRecentFiles();
    void onNewFile();
    void onUpdateWindow();
    void onFileChanged(const QString & path);
    void onReloadFile();
    void onClicked(const QPoint & pt);
    void onMouseMove(const QPoint & pt);
    void onViewInfoWindow();
    void onColorProfileTriggered(QAction * action);
    void updateViewModeLocation();
    void onMinimize();
    void onBringAllToFront();
    void onShowMainWindow();
    void onAbout();
    void onViewLicense();

protected:
    QSettings * settings;
    LoadThread * load_thread;
    QProgressDialog * progress;
    QString file_name;
    QTimer update_timer;
    QFileSystemWatcher * file_watcher;
    NotificationWidget * notification;
    FileChangedNotificationWidget * file_changed_notification;
    QMenuBar * menu_bar;
    QMenu * recent_menu;
    QStringList recent_files;
    QMenu * export_menu;
    View * view;
    QDockWidget * info_dock;
    InfoWindow * info_window;
    AboutDialog * about_dlg;
    LicenseDialog * license_dlg;
    SelectTool * select_tool;
    ExportTool * export_tool;
    ExplodeTool * explode_tool;
    MeshQualityTool * mesh_quality_tool;
    CheckForUpdateTool * update_tool;

    QAction * new_action;
    QAction * open_action;
    QAction * close_action;
    QAction * clear_recent_file;
    QAction * view_info_wnd_action;
    QAction * tools_explode_action;
    QAction * tools_mesh_quality_action;
    QAction * minimize;
    QAction * bring_all_to_front;
    QAction * show_main_window;
    QAction * about_box_action;
    QAction * view_license_action;

    QActionGroup * color_profile_action_group;
    QActionGroup * windows_action_group;

    std::vector<vtkExtractBlock *> extract_blocks;
    std::map<int, BlockObject *> blocks;
    std::map<int, SideSetObject *> side_sets;
    std::map<int, NodeSetObject *> node_sets;

    /// center of bounding box of the whole mesh
    vtkVector3d center_of_bounds;

    std::size_t color_profile_idx;
    std::vector<ColorProfile *> color_profiles;
};
