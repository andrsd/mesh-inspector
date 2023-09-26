#include "model.h"
#include "mainwindow.h"
#include "view.h"
#include "vtkExtractBlock.h"
#include "vtkBoundingBox.h"
#include "vtkAlgorithmOutput.h"
#include "blockobject.h"
#include "sidesetobject.h"
#include "nodesetobject.h"
#include <QThread>
#include <QFileInfo>
#include <QProgressDialog>
#include <QFileSystemWatcher>
#include "reader.h"
#include "exodusiireader.h"
#include "vtkreader.h"
#include "stlreader.h"

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

LoadThread::LoadThread(const QString & file_name) : QThread(), file_name(file_name), reader(nullptr)
{
    if (file_name.endsWith(".e") || file_name.endsWith(".exo"))
        this->reader = new ExodusIIReader(file_name.toStdString());
    else if (file_name.endsWith(".vtk"))
        this->reader = new VTKReader(file_name.toStdString());
    else if (file_name.endsWith(".stl"))
        this->reader = new STLReader(file_name.toStdString());
    else
        this->reader = nullptr;
}

LoadThread::~LoadThread()
{
    delete this->reader;
}

Reader *
LoadThread::getReader()
{
    return this->reader;
}

bool
LoadThread::hasValidFile()
{
    return this->reader != nullptr;
}

const QString &
LoadThread::getFileName()
{
    return this->file_name;
}

void
LoadThread::run()
{
    if (this->reader != nullptr)
        this->reader->load();
}

Model::Model(MainWindow * main_win) :
    QObject(),
    main_window(main_win),
    view(main_window->getView()),
    center_of_bounds(0., 0., 0.),
    progress(nullptr),
    load_thread(nullptr),
    file_name(),
    file_watcher(new QFileSystemWatcher())
{
    connect(this->file_watcher, &QFileSystemWatcher::fileChanged, this, &Model::onFileChanged);
}

Model::~Model()
{
    delete this->load_thread;
    delete this->file_watcher;
}

const std::map<int, BlockObject *> &
Model::getBlocks() const
{
    return this->blocks;
}

const std::map<int, SideSetObject *> &
Model::getSideSets() const
{
    return this->side_sets;
}

const std::map<int, NodeSetObject *> &
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
    for (auto & it : this->blocks)
        delete it.second;
    this->blocks.clear();

    for (auto & it : this->side_sets)
        delete it.second;
    this->side_sets.clear();

    for (auto & it : this->node_sets)
        delete it.second;
    this->node_sets.clear();

    for (auto & eb : this->extract_blocks)
        eb->Delete();
    this->extract_blocks.clear();

    this->file_name = QString();
    auto watched_files = this->file_watcher->files();
    for (auto & file : watched_files)
        this->file_watcher->removePath(file);

    this->view->clear();
}

vtkBoundingBox
Model::getTotalBoundingBox()
{

    return this->bbox;
}

void
Model::computeTotalBoundingBox()
{
    vtkBoundingBox bbox;
    for (auto & it : this->blocks) {
        auto block = it.second;
        bbox.AddBounds(block->getBounds());
    }

    double center[3];
    bbox.GetCenter(center);
    this->center_of_bounds = vtkVector3d(center[0], center[1], center[2]);
}

void
Model::addBlocks()
{
    auto * camera = this->view->getActiveCamera();
    auto * reader = this->load_thread->getReader();

    for (auto & binfo : reader->getBlocks()) {
        BlockObject * block = nullptr;
        if (binfo.multiblock_index != -1) {
            vtkExtractBlock * eb = vtkExtractBlock::New();
            eb->SetInputConnection(reader->getVtkOutputPort());
            eb->AddIndex(binfo.multiblock_index);
            eb->Update();
            this->extract_blocks.push_back(eb);

            block = new BlockObject(eb->GetOutputPort(), camera);
        }
        else
            block = new BlockObject(reader->getVtkOutputPort(), camera);
        this->blocks[binfo.number] = block;
        this->view->addBlock(block);
        emit blockAdded(binfo.number, QString::fromStdString(binfo.name));
    }
}

void
Model::addSideSets()
{
    auto * reader = this->load_thread->getReader();

    for (auto & finfo : reader->getSideSets()) {
        auto * eb = vtkExtractBlock::New();
        eb->SetInputConnection(reader->getVtkOutputPort());
        eb->AddIndex(finfo.multiblock_index);
        eb->Update();
        this->extract_blocks.push_back(eb);

        auto sideset = new SideSetObject(eb->GetOutputPort());
        this->side_sets[finfo.number] = sideset;
        this->view->addSideSet(sideset);
        emit sideSetAdded(finfo.number, QString::fromStdString(finfo.name));
    }
}

void
Model::addNodeSets()
{
    auto * reader = this->load_thread->getReader();

    for (auto & ninfo : reader->getNodeSets()) {
        auto * eb = vtkExtractBlock::New();
        eb->SetInputConnection(reader->getVtkOutputPort());
        eb->AddIndex(ninfo.multiblock_index);
        eb->Update();
        this->extract_blocks.push_back(eb);

        auto * nodeset = new NodeSetObject(eb->GetOutputPort());
        this->node_sets[ninfo.number] = nodeset;
        this->view->addNodeSet(nodeset);
        emit nodeSetAdded(ninfo.number, QString::fromStdString(ninfo.name));
    }
}

BlockObject *
Model::getBlock(int block_id)
{
    const auto & it = this->blocks.find(block_id);
    if (it != this->blocks.end())
        return it->second;
    else
        return nullptr;
}

SideSetObject *
Model::getSideSet(int sideset_id)
{
    const auto & it = this->side_sets.find(sideset_id);
    if (it != this->side_sets.end())
        return it->second;
    else
        return nullptr;
}

NodeSetObject *
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
    for (auto & it : this->blocks) {
        auto * block = it.second;
        if (block->getActor() == actor)
            return it.first;
    }
    return -1;
}

void
Model::loadFile(const QString & file_name)
{
    QFileInfo fi(file_name);
    this->progress =
        new QProgressDialog(QString("Loading %1...").arg(fi.fileName()), QString(), 0, 0, this->main_window);
    this->progress->setWindowModality(Qt::WindowModal);
    this->progress->show();

    delete this->load_thread;
    this->load_thread = new LoadThread(file_name);
    connect(this->load_thread, &LoadThread::finished, this, &Model::onLoadFinished);
    this->load_thread->start(QThread::IdlePriority);
}

void
Model::onLoadFinished()
{
    hideLoadProgressBar();
    if (this->load_thread->hasValidFile()) {
        Reader * reader = this->load_thread->getReader();
        this->file_name = QString(reader->getFileName().c_str());
        this->file_watcher->addPath(this->file_name);
        addBlocks();
        addSideSets();
        addNodeSets();
        computeTotalBoundingBox();
        this->view->updateBoundingBox();
        this->view->setInteractorStyle(getDimension());
        this->view->resetCamera();
    }
    emit loadFinished();
}

void
Model::hideLoadProgressBar()
{
    this->progress->hide();
    delete this->progress;
    this->progress = nullptr;
}

bool
Model::hasFile() const
{
    return !this->file_name.isEmpty();
}

bool
Model::hasValidFile() const
{
    return this->load_thread->hasValidFile();
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
    Reader * reader = this->load_thread->getReader();
    return reader->getVtkOutputPort();
}

std::size_t
Model::getTotalNumberOfElements() const
{
    Reader * reader = this->load_thread->getReader();
    return reader->getTotalNumberOfElements();
}

std::size_t
Model::getTotalNumberOfNodes() const
{
    Reader * reader = this->load_thread->getReader();
    return reader->getTotalNumberOfNodes();
}

int
Model::getDimension() const
{
    Reader * reader = this->load_thread->getReader();
    return reader->getDimensionality();
}

void
Model::onFileChanged(const QString & path)
{
    if (!this->file_watcher->files().contains(path))
        this->file_watcher->addPath(path);
    emit fileChanged(path);
}
