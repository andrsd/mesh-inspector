// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QFileInfo>
#include "vtkVector.h"
#include "vtkBoundingBox.h"
#include <vector>

class MainWindow;
class vtkExtractBlock;
class vtkExtractMaterialBlock;
class vtkActor;
class vtkAlgorithmOutput;
class BlockObject;
class SideSetObject;
class NodeSetObject;
class QString;
class LoadThread;
class Reader;
class View;
class InfoView;
class QFileSystemWatcher;
class FileChangedNotificationWidget;

class Model : public QObject {
    Q_OBJECT;

public:
    explicit Model(MainWindow * main_window);
    ~Model() override;

    const std::map<int, BlockObject *> & getBlocks() const;
    const std::map<int, SideSetObject *> & getSideSets() const;
    const std::map<int, NodeSetObject *> & getNodeSets() const;
    const vtkVector3d & getCenterOfBounds() const;

    BlockObject * getBlock(int block_id);
    SideSetObject * getSideSet(int sideset_id);
    NodeSetObject * getNodeSet(int nodeset_id);

    int blockActorToId(vtkActor * actor);

    void clear();
    void loadFile(const QString & file_name);
    vtkBoundingBox getTotalBoundingBox();

    bool hasFile() const;
    bool hasValidFile() const;
    const QString & getFileName() const;
    QFileInfo getFileInfo();

    vtkAlgorithmOutput * getVtkOutputPort();
    std::size_t getTotalNumberOfElements() const;
    std::size_t getTotalNumberOfNodes() const;
    int getDimension() const;

    void resetCameraOnLoad(bool state);

signals:
    void blockAdded(int id, const QString & name);
    void sideSetAdded(int id, const QString & name);
    void nodeSetAdded(int id, const QString & name);
    void loadFinished();
    void fileChanged(const QString & path);

public slots:
    void onLoadFinished();
    void onFileChanged(const QString & path);

protected:
    void addBlocks();
    void addSideSets();
    void addNodeSets();
    void computeTotalBoundingBox();
    Reader * createReader(const QString & file_name);

    MainWindow * main_window;
    View *& view;
    InfoView *& info_view;

    std::vector<vtkExtractBlock *> extract_blocks;
    std::vector<vtkExtractMaterialBlock *> extract_mat_blocks;
    std::map<int, BlockObject *> blocks;
    std::map<int, SideSetObject *> side_sets;
    std::map<int, NodeSetObject *> node_sets;

    /// Bounding box
    vtkBoundingBox bbox;
    /// center of bounding box of the whole mesh
    vtkVector3d center_of_bounds;

    LoadThread * load_thread;
    Reader * reader;
    QString file_name;
    QFileSystemWatcher * file_watcher;
    bool reset_camera_on_load;
};
