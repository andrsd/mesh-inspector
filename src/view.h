#pragma once

#include "QVTKOpenGLNativeWidget.h"

class MainWindow;
class Model;
class QMenu;
class QPushButton;
class QAction;
class QActionGroup;
class vtkOrientationMarkerWidget;
class vtkRenderWindowInteractor;
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
    void addBlock(BlockObject * block);
    void addSideSet(SideSetObject * sideset);
    void addNodeSet(NodeSetObject * nodeset);
    void setColorProfile(ColorProfile * profile);
    void setInteractorStyle(int dim);
    void resetCamera();
    void setBlockProperties(BlockObject * block, bool selected = false, bool highlighted = false);
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

protected:
    void setupViewModeWidget();
    void setSelectedBlockProperties(BlockObject * block, bool highlighted = false);
    void setDeselectedBlockProperties(BlockObject * block, bool highlighted = false);
    void setHighlightedBlockProperties(BlockObject * block, bool highlighted);
    void setSideSetProperties(SideSetObject * sideset);
    void setNodeSetProperties(NodeSetObject * nodeset);
    void setupCubeAxesActor();

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
    vtkOrientationMarkerWidget * ori_marker;
    vtkGenericOpenGLRenderWindow * render_window;
    vtkRenderer * renderer;
    vtkRenderWindowInteractor * interactor;
    OInteractorStyle2D * interactor_style_2d;
    OInteractorStyle3D * interactor_style_3d;
    vtkCubeAxesActor * cube_axes_actor;

public:
    static QColor SIDESET_CLR;
    static QColor SIDESET_EDGE_CLR;
    static QColor NODESET_CLR;

    static float EDGE_WIDTH;
    static float OUTLINE_WIDTH;
};
