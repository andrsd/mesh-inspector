// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "model.h"
#include "mainwindow.h"
#include "view.h"
#include "infoview.h"
#include "vtkExtractBlock.h"
#include "vtkBoundingBox.h"
#include "vtkAlgorithmOutput.h"
#include "blockobject.h"
#include "sidesetobject.h"
#include "nodesetobject.h"
#include "vtkextractmaterialblock.h"
#include <QThread>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include "reader.h"
#include "exodusiireader.h"
#include "vtkreader.h"
#include "objreader.h"
#include "stlreader.h"
#include "mshreader.h"

class LoadThread : public QThread {
public:
    explicit LoadThread(std::shared_ptr<Reader> reader);

protected:
    void run() override;

    std::shared_ptr<Reader> reader;
};

LoadThread::LoadThread(std::shared_ptr<Reader> reader) : QThread(), reader(reader) {}

void
LoadThread::run()
{
    this->reader->load();
}

//

Model::Model(MainWindow * main_win) :
    QObject(),
    main_window(main_win),
    view(main_window->getView()),
    info_view(main_window->getInfoView()),
    center_of_bounds(0., 0., 0.),
    load_thread(nullptr),
    reader(nullptr),
    file_name(),
    file_watcher(new QFileSystemWatcher()),
    reset_camera_on_load(true)
{
    connect(this->file_watcher, &QFileSystemWatcher::fileChanged, this, &Model::onFileChanged);
}

Model::~Model()
{
    delete this->file_watcher;
}

const std::map<int, std::shared_ptr<BlockObject>> &
Model::getBlocks() const
{
    return this->blocks;
}

const std::map<int, std::shared_ptr<SideSetObject>> &
Model::getSideSets() const
{
    return this->side_sets;
}

const std::map<int, std::shared_ptr<NodeSetObject>> &
Model::getNodeSets() const
{
    return this->node_sets;
}

const vtkVector3d &
Model::getCenterOfBounds() const
{
    return this->center_of_bounds;
}

void
Model::clear()
{
    this->bbox.Reset();

    this->blocks.clear();
    this->side_sets.clear();
    this->node_sets.clear();

    this->extract_blocks.clear();
    this->extract_mat_blocks.clear();

    this->file_name = QString();
    auto watched_files = this->file_watcher->files();
    for (auto & file : watched_files)
        this->file_watcher->removePath(file);

    this->view->clear();
    this->info_view->clear();
}

vtkBoundingBox
Model::getTotalBoundingBox()
{
    return this->bbox;
}

void
Model::computeTotalBoundingBox()
{
    for (auto & it : this->blocks) {
        auto block = it.second;
        this->bbox.AddBounds(block->getBounds());
    }

    double center[3];
    this->bbox.GetCenter(center);
    this->center_of_bounds = vtkVector3d(center[0], center[1], center[2]);
}

void
Model::addBlocks()
{
    auto * camera = this->view->getActiveCamera();

    for (auto & binfo : this->reader->getBlocks()) {
        std::shared_ptr<BlockObject> block;
        if (binfo.multiblock_index != -1) {
            auto eb = vtkSmartPointer<vtkExtractBlock>::New();
            eb->SetInputConnection(this->reader->getVtkOutputPort());
            eb->AddIndex(binfo.multiblock_index);
            eb->Update();
            this->extract_blocks.push_back(eb);

            block = std::make_shared<BlockObject>(eb->GetOutputPort(), camera);
        }
        else if (binfo.material_index != -1) {
            auto eb = vtkSmartPointer<vtkExtractMaterialBlock>::New();
            eb->SetInputConnection(this->reader->getVtkOutputPort());
            eb->SetBlockId(binfo.material_index);
            eb->Update();
            this->extract_mat_blocks.push_back(eb);

            block = std::make_shared<BlockObject>(eb->GetOutputPort(), camera);
        }
        else {
            block = std::make_shared<BlockObject>(this->reader->getVtkOutputPort(), camera);
        }
        this->blocks[binfo.number] = block;
        this->view->addBlock(block);
        emit blockAdded(binfo.number, QString::fromStdString(binfo.name));
    }
}

void
Model::addSideSets()
{
    for (auto & finfo : this->reader->getSideSets()) {
        auto eb = vtkSmartPointer<vtkExtractBlock>::New();
        eb->SetInputConnection(this->reader->getVtkOutputPort());
        eb->AddIndex(finfo.multiblock_index);
        eb->Update();
        this->extract_blocks.push_back(eb);

        auto sideset = std::make_shared<SideSetObject>(eb->GetOutputPort());
        this->side_sets[finfo.number] = sideset;
        this->view->addSideSet(sideset);
        emit sideSetAdded(finfo.number, QString::fromStdString(finfo.name));
    }
}

void
Model::addNodeSets()
{
    for (auto & ninfo : reader->getNodeSets()) {
        auto eb = vtkSmartPointer<vtkExtractBlock>::New();
        eb->SetInputConnection(this->reader->getVtkOutputPort());
        eb->AddIndex(ninfo.multiblock_index);
        eb->Update();
        this->extract_blocks.push_back(eb);

        auto nodeset = std::make_shared<NodeSetObject>(eb->GetOutputPort());
        this->node_sets[ninfo.number] = nodeset;
        this->view->addNodeSet(nodeset);
        emit nodeSetAdded(ninfo.number, QString::fromStdString(ninfo.name));
    }
}

std::shared_ptr<BlockObject>
Model::getBlock(int block_id)
{
    const auto & it = this->blocks.find(block_id);
    if (it != this->blocks.end())
        return it->second;
    else
        return nullptr;
}

std::shared_ptr<SideSetObject>
Model::getSideSet(int sideset_id)
{
    const auto & it = this->side_sets.find(sideset_id);
    if (it != this->side_sets.end())
        return it->second;
    else
        return nullptr;
}

std::shared_ptr<NodeSetObject>
Model::getNodeSet(int nodeset_id)
{
    const auto & it = this->node_sets.find(nodeset_id);
    if (it != this->node_sets.end())
        return it->second;
    else
        return nullptr;
}

int
Model::blockActorToId(vtkActor * actor)
{
    // TODO: when we start to have 1000s of actors, this should be done via a
    // map from 'actor' to 'block_id'
    for (auto & [id, block] : this->blocks) {
        if (block->getActor() == actor)
            return id;
    }
    return -1;
}

void
Model::loadFile(const QString & file_name)
{
    this->reader = createReader(file_name);
    if (this->reader) {
        this->file_name = file_name;
        this->load_thread = std::make_shared<LoadThread>(this->reader);
        connect(this->load_thread.get(), &LoadThread::finished, this, &Model::onLoadFinished);
        this->load_thread->start(QThread::IdlePriority);
    }
}

void
Model::onLoadFinished()
{
    if (this->hasValidFile()) {
        this->file_watcher->addPath(this->file_name);
        this->info_view->clear();
        addBlocks();
        addSideSets();
        addNodeSets();
        computeTotalBoundingBox();
        this->view->updateBoundingBox();
        this->view->setInteractorStyle(getDimension());
        if (this->reset_camera_on_load)
            this->view->resetCamera();
        this->info_view->update();
    }
    emit loadFinished();
    this->load_thread = nullptr;
}

bool
Model::hasFile() const
{
    return !this->file_name.isEmpty();
}

bool
Model::hasValidFile() const
{
    return this->reader != nullptr;
}

const QString &
Model::getFileName() const
{
    return this->file_name;
}

QFileInfo
Model::getFileInfo()
{
    return QFileInfo(this->file_name);
}

vtkAlgorithmOutput *
Model::getVtkOutputPort()
{
    return this->reader->getVtkOutputPort();
}

std::size_t
Model::getTotalNumberOfElements() const
{
    return this->reader->getTotalNumberOfElements();
}

std::size_t
Model::getTotalNumberOfNodes() const
{
    return this->reader->getTotalNumberOfNodes();
}

int
Model::getDimension() const
{
    return this->reader->getDimensionality();
}

void
Model::resetCameraOnLoad(bool state)
{
    this->reset_camera_on_load = state;
}

void
Model::onFileChanged(const QString & path)
{
    if (!this->file_watcher->files().contains(path))
        this->file_watcher->addPath(path);
    emit fileChanged(path);
}

std::shared_ptr<Reader>
Model::createReader(const QString & file_name)
{
    if (file_name.endsWith(".e") || file_name.endsWith(".exo"))
        return std::make_shared<ExodusIIReader>(file_name.toStdString());
    else if (file_name.endsWith(".vtk") || file_name.endsWith(".vtu"))
        return std::make_shared<VTKReader>(file_name.toStdString());
    else if (file_name.endsWith(".obj"))
        return std::make_shared<OBJReader>(file_name.toStdString());
    else if (file_name.endsWith(".stl"))
        return std::make_shared<STLReader>(file_name.toStdString());
    else if (file_name.endsWith(".msh"))
        return std::make_shared<MSHReader>(file_name.toStdString());
    else
        return nullptr;
}
