#include "vtkreader.h"
#include "vtkUnstructuredGridReader.h"
#include "vtkUnstructuredGrid.h"

VTKReader::VTKReader(const std::string & file_name) : Reader(file_name), reader(nullptr) {}

VTKReader::~VTKReader()
{
    if (this->reader)
        this->reader->Delete();
}

void
VTKReader::load()
{
    this->reader = vtkUnstructuredGridReader::New();

    this->reader->SetFileName(this->file_name.c_str());
    this->reader->Update();

    readBlockInfo();
    readVariableInfo();
}

std::size_t
VTKReader::getTotalNumberOfElements() const
{
    return this->reader->GetOutput()->GetNumberOfCells();
}

std::size_t
VTKReader::getTotalNumberOfNodes() const
{
    return this->reader->GetOutput()->GetNumberOfPoints();
}

int
VTKReader::getDimensionality() const
{
    // FIXME: get this from the cell type
    return 3;
}

vtkAlgorithmOutput *
VTKReader::getVtkOutputPort()
{
    return this->reader->GetOutputPort(0);
}

std::vector<Reader::BlockInformation>
VTKReader::getBlocks()
{
    std::vector<BlockInformation> blocks;
    for (auto & it : this->block_info)
        blocks.push_back(it.second);
    return blocks;
}

std::vector<Reader::BlockInformation>
VTKReader::getSideSets()
{
    std::vector<BlockInformation> sidesets;
    return sidesets;
}

std::vector<Reader::BlockInformation>
VTKReader::getNodeSets()
{
    std::vector<BlockInformation> nodesets;
    return nodesets;
}

void
VTKReader::readBlockInfo()
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

void
VTKReader::readVariableInfo()
{
}
