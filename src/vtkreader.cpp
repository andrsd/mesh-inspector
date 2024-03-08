#include "vtkreader.h"
#include "vtkUnstructuredGridReader.h"
#include "vtkXMLUnstructuredGridReader.h"
#include "vtkUnstructuredGrid.h"

namespace {

bool
endsWith(const std::string & str, const std::string & suffix)
{
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

}; // namespace

VTKReader::VTKReader(const std::string & file_name) :
    Reader(file_name),
    reader(nullptr),
    xml_reader(nullptr)
{
}

VTKReader::~VTKReader()
{
    if (this->reader)
        this->reader->Delete();
    if (this->xml_reader)
        this->xml_reader->Delete();
}

void
VTKReader::load()
{
    if (endsWith(this->file_name, ".vtk")) {
        this->reader = vtkUnstructuredGridReader::New();
        this->reader->SetFileName(this->file_name.c_str());
        this->reader->Update();
    }
    else if (endsWith(this->file_name, ".vtu")) {
        this->xml_reader = vtkXMLUnstructuredGridReader::New();
        this->xml_reader->SetFileName(this->file_name.c_str());
        this->xml_reader->Update();

        this->xml_reader->PrintSelf(std::cerr, vtkIndent());
    }

    readBlockInfo();
}

std::size_t
VTKReader::getTotalNumberOfElements() const
{
    if (this->reader)
        return this->reader->GetOutput()->GetNumberOfCells();
    else if (this->xml_reader)
        return this->xml_reader->GetOutput()->GetNumberOfCells();
    else
        return 0;
}

std::size_t
VTKReader::getTotalNumberOfNodes() const
{
    if (this->reader)
        return this->reader->GetOutput()->GetNumberOfPoints();
    else if (this->xml_reader)
        return this->xml_reader->GetOutput()->GetNumberOfPoints();
    else
        return 0;
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
    if (this->reader)
        return this->reader->GetOutputPort(0);
    else if (this->xml_reader)
        return this->xml_reader->GetOutputPort(0);
    else
        return nullptr;
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
    binfo.material_index = -1;
    this->block_info[vtkid] = binfo;
}
