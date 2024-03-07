#include "stlreader.h"
#include "vtkSTLReader.h"

STLReader::STLReader(const std::string & file_name) : Reader(file_name), reader(nullptr) {}

STLReader::~STLReader()
{
    if (this->reader)
        this->reader->Delete();
}

void
STLReader::load()
{
    this->reader = vtkSTLReader::New();

    this->reader->SetFileName(this->file_name.c_str());
    this->reader->Update();

    readBlockInfo();
}

std::size_t
STLReader::getTotalNumberOfElements() const
{
    return this->reader->GetOutput()->GetNumberOfCells();
}

std::size_t
STLReader::getTotalNumberOfNodes() const
{
    return this->reader->GetOutput()->GetNumberOfPoints();
}

int
STLReader::getDimensionality() const
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
STLReader::getVtkOutputPort()
{
    return this->reader->GetOutputPort(0);
}

std::vector<Reader::BlockInformation>
STLReader::getBlocks()
{
    std::vector<BlockInformation> blocks;
    for (auto & it : this->block_info)
        blocks.push_back(it.second);
    return blocks;
}

std::vector<Reader::BlockInformation>
STLReader::getSideSets()
{
    std::vector<BlockInformation> sidesets;
    return sidesets;
}

std::vector<Reader::BlockInformation>
STLReader::getNodeSets()
{
    std::vector<BlockInformation> nodesets;
    return nodesets;
}

void
STLReader::readBlockInfo()
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
