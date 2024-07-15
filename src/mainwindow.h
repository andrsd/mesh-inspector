// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QMainWindow>
#include <QTimer>
#include <QColor>

class Reader;
class QSettings;
class QProgressDialog;
class QMenu;
class QActionGroup;
class QResizeEvent;
class QDragEnterEvent;
class QPushButton;
class Model;
class View;
class InfoView;
class QShortcut;
class AboutDialog;
class LicenseDialog;
class NotificationWidget;
class SelectTool;
class ExportTool;
class ExplodeTool;
class MeshQualityTool;
class CheckForUpdateTool;
class FileChangedNotificationWidget;
class BlockObject;
class SideSetObject;
class NodeSetObject;
class ColorProfile;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget * parent = nullptr);
    ~MainWindow() override;

    int getRenderWindowWidth() const;
    void updateMenuBar();
    void showNotification(const QString & text, int ms = 5000);
    QSettings * getSettings();
    View *& getView();
    InfoView *& getInfoView();
    Model *& getModel();
    const BlockObject * getSelectedBlock();

    template <typename T>
    inline qreal
    HIDPI(T value) const
    {
        return devicePixelRatio() * value;
    }

signals:
    void colorProfileChanged(ColorProfile * profile);

protected:
    void setupWidgets();
    void setupNotificationWidget();
    void setupFileChangedNotificationWidget();
    void setupMenuBar();
    void setupColorProfileMenu(QMenu * menu);
    void updateWindowTitle();
    void connectSignals();
    void clear();
    void loadFile(const QString & file_name);
    void updateInfoWindow();
    void showFileChangedNotification();
    void setColorProfile();
    void loadColorProfiles();
    void buildRecentFilesMenu();
    void addToRecentFiles(const QString & file_name);
    void update();
    void hideLoadProgressBar();

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
    QTimer update_timer;
    NotificationWidget * notification;
    FileChangedNotificationWidget * file_changed_notification;
    QMenuBar * menu_bar;
    QMenu * recent_menu;
    QStringList recent_files;
    QMenu * export_menu;
    View * view;
    InfoView * info_view;
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

    Model * model;

    std::size_t color_profile_idx;
    std::vector<ColorProfile *> color_profiles;

    QProgressDialog * progress;
};
