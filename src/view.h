// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "QVTKOpenGLNativeWidget.h"
#include "vtkSmartPointer.h"

class MainWindow;
class Model;
class QMenu;
class QPushButton;
class QAction;
class QActionGroup;
class vtkOrientationMarkerWidget;
class BlockObject;
class SideSetObject;
class NodeSetObject;
class OInteractorStyle2D;
class OInteractorStyle3D;
class vtkCamera;
class vtkCubeAxesActor;
class ColorProfile;

class View : public QVTKOpenGLNativeWidget {
protected:
    enum ERenderMode {
        SHADED = 0,
        SHADED_WITH_EDGES = 1,
        HIDDEN_EDGES_REMOVED = 2,
        TRANSLUENT = 3
    };

public:
    explicit View(MainWindow * main_wnd);
    ~View() override;

    void setupWidgets();
    void setMenu(QMenu * menu);
    void updateMenuBar(bool enabled);
    void setupVtk();
    void clear();
    void setupOrientationMarker();
    vtkCamera * getActiveCamera();
    void addBlock(std::shared_ptr<BlockObject> block);
    void addSideSet(std::shared_ptr<SideSetObject> sideset);
    void addNodeSet(std::shared_ptr<NodeSetObject> nodeset);
    void setInteractorStyle(int dim);
    void resetCamera();
    void setBlockProperties(std::shared_ptr<BlockObject> block,
                            bool selected = false,
                            bool highlighted = false);
    void setBlockVisibility(int block_id, bool visible);
    void setBlockOpacity(int block_id, double opacity);
    void setBlockColor(int block_id, QColor color);
    void setSideSetVisibility(int sideset_id, bool visible);
    void setNodeSetVisibility(int nodeset_id, bool visible);
    void render();
    void updateLocation();
    void activateRenderMode();
    void updateBoundingBox();
    void setCubeAxisVisibility(bool visible);
    vtkRenderer * getRenderer() const;
    vtkGenericOpenGLRenderWindow * getRenderWindow() const;

public slots:
    void onShadedTriggered(bool checked);
    void onShadedWithEdgesTriggered(bool checked);
    void onHiddenEdgesRemovedTriggered(bool checked);
    void onTransluentTriggered(bool checked);
    void onPerspectiveToggled(bool checked);
    void onOrientationMarkerVisibilityChanged(bool visible);
    void onColorProfileChanged(ColorProfile * profile);

protected:
    void setupViewModeWidget();
    void setSelectedBlockProperties(std::shared_ptr<BlockObject> block, bool highlighted = false);
    void setDeselectedBlockProperties(std::shared_ptr<BlockObject> block, bool highlighted = false);
    void setHighlightedBlockProperties(std::shared_ptr<BlockObject> block, bool highlighted);
    void setSideSetProperties(std::shared_ptr<SideSetObject> sideset);
    void setNodeSetProperties(std::shared_ptr<NodeSetObject> nodeset);
    void setupCubeAxesActor();
    void setCubeAxesColors(ColorProfile * profile);

    MainWindow * main_window;
    Model *& model;
    QMenu * view_menu;
    QPushButton * view_mode;
    QAction * shaded_action;
    QAction * shaded_w_edges_action;
    QAction * hidden_edges_removed_action;
    QAction * transluent_action;
    ERenderMode render_mode;
    QActionGroup * visual_repr;
    QAction * perspective_action;
    QAction * ori_marker_action;
    vtkSmartPointer<vtkOrientationMarkerWidget> ori_marker;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> render_window;
    vtkSmartPointer<vtkRenderer> renderer;
    OInteractorStyle2D * interactor_style_2d;
    OInteractorStyle3D * interactor_style_3d;
    vtkSmartPointer<vtkCubeAxesActor> cube_axes_actor;

public:
    static QColor SIDESET_CLR;
    static QColor SIDESET_EDGE_CLR;
    static QColor NODESET_CLR;

    static float EDGE_WIDTH;
    static float SIDESET_EDGE_WIDTH;
    static float OUTLINE_WIDTH;
};
