#pragma once

#include <QObject>

class MainWindow;
class Model;
class View;
class InfoWidget;
class QShortcut;
class QMenu;
class QAction;
class QActionGroup;
class QSettings;
class BlockObject;
class Selection;

class SelectTool : public QObject {
protected:
    enum EModeSelect {
        MODE_SELECT_NONE = 0,
        MODE_SELECT_BLOCKS = 1,
        MODE_SELECT_CELLS = 2,
        MODE_SELECT_POINTS = 3
    };

public:
    explicit SelectTool(MainWindow * main_wnd);
    ~SelectTool() override;

    void setupWidgets();
    void setupMenu(QMenu * menu);
    void update();
    void clear();
    void saveSettings(QSettings * settings);
    void onClicked(const QPoint & pt);
    void onMouseMove(const QPoint & pt);
    const BlockObject * getSelectedBlock() const;

public slots:
    void onDeselect();
    void onBlockSelectionChanged(int block_id);
    void onSideSetSelectionChanged(int sideset_id);
    void onNodeSetSelectionChanged(int nodeset_id);
    void onSelectModeTriggered(QAction * action);

protected:
    void showSelectedMeshEntity(const QString & info);
    void hideSelectedMeshEntity();
    void deselectBlocks();
    void selectBlock(const QPoint & pt);
    void selectCell(const QPoint & pt);
    void selectPoint(const QPoint & pt);
    void setSelectionProperties();
    void highlightBlock(const QPoint & pt);
    void highlightCell(const QPoint & pt);
    void highlightPoint(const QPoint & pt);
    void setHighlightProperties();

    MainWindow * main_window;
    Model *& model;
    View *& view;
    InfoWidget * selected_mesh_ent_info;
    QShortcut * deselect_sc;
    QActionGroup * mode_select_action_group;

    EModeSelect select_mode;
    Selection * selection;
    BlockObject * selected_block;
    Selection * highlight;
    BlockObject * highlighted_block;

public:
    static QColor SELECTION_CLR;
    static QColor SELECTION_EDGE_CLR;
    static QColor HIGHLIGHT_CLR;
};
