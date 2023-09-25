#pragma once

#include <QMainWindow>
#include <QThread>
#include <QTimer>
#include <QColor>
#include <QVersionNumber>
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
class QVTKOpenGLNativeWidget;
class QDockWidget;
class QShortcut;
class QNetworkAccessManager;
class QNetworkReply;
class InfoWindow;
class AboutDialog;
class LicenseDialog;
class NotificationWidget;
class ExplodeWidget;
class MeshQualityWidget;
class FileChangedNotificationWidget;
class vtkGenericOpenGLRenderWindow;
class vtkRenderer;
class vtkRenderWindowInteractor;
class vtkOrientationMarkerWidget;
class vtkCompositeDataGeometryFilter;
class vtkActor;
class vtkCubeAxesActor;
class vtkExtractBlock;
class vtkLookupTable;
class vtkScalarBarActor;
class BlockObject;
class SideSetObject;
class NodeSetObject;
class ColorProfile;
class InfoWidget;
class OInteractorStyle2D;
class OInteractorStyle3D;
class Selection;

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

    enum ERenderMode {
        SHADED = 0,
        SHADED_WITH_EDGES = 1,
        HIDDEN_EDGES_REMOVED = 2,
        TRANSLUENT = 3
    };

    enum EModeSelect {
        MODE_SELECT_NONE = 0,
        MODE_SELECT_BLOCKS = 1,
        MODE_SELECT_CELLS = 2,
        MODE_SELECT_POINTS = 3
    };

public:
    explicit MainWindow(QWidget * parent = nullptr);
    ~MainWindow() override;

signals:
    void blockAdded(int id, const QString & name);
    void sideSetAdded(int id, const QString & name);
    void nodeSetAdded(int id, const QString & name);

protected:
    void setupWidgets();
    void setupViewModeWidget(QMainWindow * wnd);
    void setupNotificationWidget();
    void setupFileChangedNotificationWidget();
    void setupExplodeWidgets();
    void setupMeshQualityWidget();
    void setupLookupTable();
    void setupMenuBar();
    void setupExportMenu(QMenu * menu);
    void setupColorProfileMenu(QMenu * menu);
    void setupSelectModeMenu(QMenu * menu);
    void updateMenuBar();

    void updateWindowTitle();
    void connectSignals();
    void clear();
    void loadFile(const QString & file_name);
    bool hasFile();

    void setupVtk();
    void setupOrientationMarker();
    void setupCubeAxesActor();
    void setupColorBar();
    void computeTotalBoundingBox();

    int getRenderWindowWidth() const;
    bool checkFileExists(const QString & file_name);
    void addBlocks();
    void addSideSets();
    void addNodeSets();
    BlockObject * getBlock(int block_id);
    SideSetObject * getSideSet(int sideset_id);
    NodeSetObject * getNodeSet(int nodeset_id);
    void setSelectedBlockProperties(BlockObject * block, bool highlighted = false);
    void setDeselectedBlockProperties(BlockObject * block, bool highlighted = false);
    void setHighlightedBlockProperties(BlockObject * block, bool highlighted);
    void setBlockProperties(BlockObject * block, bool selected = false, bool highlighted = false);
    void setBlockMeshQualityProperties(BlockObject * block, double range[]);
    void setSideSetProperties(SideSetObject * sideset);
    void setNodeSetProperties(NodeSetObject * nodeset);
    void setSelectionProperties();
    void setHighlightProperties();
    void showNotification(const QString & text, int ms = 5000);
    void showFileChangedNotification();
    void showSelectedMeshEntity(const QString & info);
    void hideSelectedMeshEntity();
    void deselectBlocks();
    int blockActorToId(vtkActor * actor);
    void highlightBlock(const QPoint & pt);
    void highlightCell(const QPoint & pt);
    void highlightPoint(const QPoint & pt);
    void selectBlock(const QPoint & pt);
    void selectCell(const QPoint & pt);
    void selectPoint(const QPoint & pt);
    void setColorProfile();
    void loadColorProfiles();
    QString getFileName(const QString & window_title,
                        const QString & name_filter,
                        const QString & default_suffix);
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

    QVersionNumber getVersionFromReply(QNetworkReply * reply);

    void getCellQualityRange(double range[]);

    template <typename T>
    inline qreal
    HIDPI(T value) const
    {
        return devicePixelRatio() * value;
    }

public slots:
    void onClose();
    void onLoadFinished();
    void onBlockVisibilityChanged(int block_id, bool visible);
    void onBlockOpacityChanged(int block_id, double opacity);
    void onBlockColorChanged(int block_id, QColor color);
    void onSideSetVisibilityChanged(int sideset_id, bool visible);
    void onNodeSetVisibilityChanged(int nodeset_id, bool visible);
    void onCubeAxisVisibilityChanged(bool visible);
    void onOrientationMarkerVisibilityChanged(bool visible);
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
    void onFileChanged(const QString & path);
    void onReloadFile();
    void onBlockSelectionChanged(int block_id);
    void onSideSetSelectionChanged(int sideset_id);
    void onNodeSetSelectionChanged(int nodeset_id);
    void onClicked(const QPoint & pt);
    void onMouseMove(const QPoint & pt);
    void onViewInfoWindow();
    void onSelectModeTriggered(QAction * action);
    void onDeselect();
    void onColorProfileTriggered(QAction * action);
    void onExportAsPng();
    void onExportAsJpg();
    void onToolsExplode();
    void onToolsMeshQuality();
    void onExplodeValueChanged(double value);
    void updateViewModeLocation();
    void updateExplodeWidgetLocation();
    void updateMeshQualityWidgetLocation();
    void onMinimize();
    void onBringAllToFront();
    void onShowMainWindow();
    void onAbout();
    void onViewLicense();
    void onCheckForUpdate();
    void onHttpReply(QNetworkReply * reply);
    void onMetricChanged(int metric_id);
    void onMeshQualityClosed();

protected:
    QSettings * settings;
    LoadThread * load_thread;
    QProgressDialog * progress;
    QString file_name;
    QTimer update_timer;
    QFileSystemWatcher * file_watcher;
    NotificationWidget * notification;
    FileChangedNotificationWidget * file_changed_notification;
    ERenderMode render_mode;
    EModeSelect select_mode;
    QMenuBar * menu_bar;
    QMenu * recent_menu;
    QStringList recent_files;
    QMenu * export_menu;
    QMenu * view_menu;
    QPushButton * view_mode;
    QVTKOpenGLNativeWidget * vtk_widget;
    vtkGenericOpenGLRenderWindow * render_window;
    vtkRenderer * renderer;
    vtkRenderWindowInteractor * vtk_interactor;
    vtkOrientationMarkerWidget * ori_marker;
    vtkCubeAxesActor * cube_axes_actor;
    OInteractorStyle2D * interactor_style_2d;
    OInteractorStyle3D * interactor_style_3d;
    Selection * selection;
    Selection * highlight;
    vtkLookupTable * lut;
    vtkScalarBarActor * color_bar;
    QDockWidget * info_dock;
    InfoWindow * info_window;
    AboutDialog * about_dlg;
    LicenseDialog * license_dlg;
    ExplodeWidget * explode;
    MeshQualityWidget * mesh_quality;
    InfoWidget * selected_mesh_ent_info;

    QAction * new_action;
    QAction * open_action;
    QAction * export_as_png;
    QAction * export_as_jpg;
    QAction * close_action;
    QAction * clear_recent_file;
    QAction * shaded_action;
    QAction * shaded_w_edges_action;
    QAction * hidden_edges_removed_action;
    QAction * transluent_action;
    QAction * view_info_wnd_action;
    QAction * tools_explode_action;
    QAction * tools_mesh_quality_action;
    QAction * perspective_action;
    QAction * ori_marker_action;
    QAction * minimize;
    QAction * bring_all_to_front;
    QAction * show_main_window;
    QAction * about_box_action;
    QAction * check_update_action;
    QAction * view_license_action;

    QActionGroup * visual_repr;
    QActionGroup * color_profile_action_group;
    QActionGroup * mode_select_action_group;
    QActionGroup * windows_action_group;

    QShortcut * deselect_sc;

    std::vector<vtkExtractBlock *> extract_blocks;
    std::map<int, BlockObject *> blocks;
    BlockObject * selected_block;
    BlockObject * highlighted_block;
    std::map<int, SideSetObject *> side_sets;
    std::map<int, NodeSetObject *> node_sets;

    /// center of bounding box of the whole mesh
    vtkVector3d center_of_bounds;

    std::size_t color_profile_idx;
    std::vector<ColorProfile *> color_profiles;

    QNetworkAccessManager * namgr;

protected:
    static QColor SIDESET_CLR;
    static QColor SIDESET_EDGE_CLR;
    static QColor NODESET_CLR;
    static QColor SELECTION_CLR;
    static QColor SELECTION_EDGE_CLR;
    static QColor HIGHLIGHT_CLR;

    static float EDGE_WIDTH;
    static float OUTLINE_WIDTH;

public:
    static const char * MESH_QUALITY_FIELD_NAME;
};
