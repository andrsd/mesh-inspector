#include "objreader.h"
#include "vtkOBJReader.h"
#include "vtkPolyData.h"

OBJReader::OBJReader(const std::string & file_name) : Reader(file_name), reader(nullptr) {}

OBJReader::~OBJReader()
{
    if (this->reader)
        this->reader->Delete();
}

void
OBJReader::load()
{
    this->reader = vtkOBJReader::New();

    this->reader->SetFileName(this->file_name.c_str());
    this->reader->Update();

    readBlockInfo();
}

std::size_t
OBJReader::getTotalNumberOfElements() const
{
    return this->reader->GetOutput()->GetNumberOfCells();
}

std::size_t
OBJReader::getTotalNumberOfNodes() const
{
    return this->reader->GetOutput()->GetNumberOfPoints();
}

int
OBJReader::getDimensionality() const
{
    auto output = this->reader->GetOutput();
    auto * bounds = output->GetBounds();
    if (std::abs(bounds[4] - bounds[5]) > 1e-15)
        return 3;
    else if (std::abs(bounds[2] - bounds[3]) > 1e-15)
        return 2;
    else if (std::abs(bounds[0] - bounds[1]) > 1e-15)
        return 1;
    else
        return -1;
}

vtkAlgorithmOutput *
OBJReader::getVtkOutputPort()
{
    return this->reader->GetOutputPort(0);
}

std::vector<Reader::BlockInformation>
OBJReader::getBlocks()
{
    std::vector<BlockInformation> blocks;
    for (auto & it : this->block_info)
        blocks.push_back(it.second);
    return blocks;
}

std::vector<Reader::BlockInformation>
OBJReader::getSideSets()
{
    std::vector<BlockInformation> sidesets;
    return sidesets;
}

std::vector<Reader::BlockInformation>
OBJReader::getNodeSets()
{
    std::vector<BlockInformation> nodesets;
    return nodesets;
}

void
OBJReader::readBlockInfo()
{
    int vtkid = 0;
    BlockInformation binfo;
    binfo.object_type = 0;
    binfo.name = "block";
    binfo.number = vtkid;
    binfo.object_index = 0;
    binfo.multiblock_index = -1;
    this->block_info[vtkid] = binfo;
}
