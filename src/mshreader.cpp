#include "mshreader.h"
#include "vtkmshreader.h"

MSHReader::MSHReader(const std::string & file_name) : Reader(file_name), reader(nullptr) {}

MSHReader::~MSHReader()
{
    if (this->reader)
        this->reader->Delete();
}

void
MSHReader::load()
{
    this->reader = vtkMshReader::New();

    this->reader->SetFileName(this->file_name.c_str());
    this->reader->UpdateInformation();
    this->reader->Update();

    readBlockInfo();
}

std::size_t
MSHReader::getTotalNumberOfElements() const
{
    return this->reader->GetTotalNumberOfElements();
}

std::size_t
MSHReader::getTotalNumberOfNodes() const
{
    return this->reader->GetTotalNumberOfNodes();
}

int
MSHReader::getDimensionality() const
{
    return this->reader->GetDimensionality();
}

vtkAlgorithmOutput *
MSHReader::getVtkOutputPort()
{
    return this->reader->GetOutputPort(0);
}

std::vector<Reader::BlockInformation>
MSHReader::getBlocks()
{
    std::vector<BlockInformation> blocks;
    for (auto & it : this->block_info[vtkMshReader::ELEM_BLOCK])
        blocks.push_back(it.second);
    return blocks;
}

std::vector<Reader::BlockInformation>
MSHReader::getSideSets()
{
    std::vector<BlockInformation> sidesets;
    for (auto & it : this->block_info[vtkMshReader::SIDE_SET])
        sidesets.push_back(it.second);
    return sidesets;
}

std::vector<Reader::BlockInformation>
MSHReader::getNodeSets()
{
    std::vector<BlockInformation> nodesets;
    return nodesets;
}

void
MSHReader::readBlockInfo()
{
    std::vector<int> obj_types = { vtkMshReader::ELEM_BLOCK, vtkMshReader::SIDE_SET };

    // Index to be used with the vtkExtractBlock::AddIndex method
    int index = 0;
    // Loop over all blocks of the MultiBlockDataSet
    for (auto & otype : obj_types) {
        index += 1;
        this->block_info[otype] = std::map<int, BlockInformation>();
        for (int j = 0; j < this->reader->GetNumberOfObjects(otype); j++) {
            index += 1;
            std::string name = this->reader->GetObjectName(otype, j);
            auto vtkid = this->reader->GetObjectId(otype, j);
            if (name.rfind("Unnamed", 0) == 0)
                name = std::to_string(vtkid);

            BlockInformation binfo;
            binfo.object_type = otype;
            binfo.name = name;
            binfo.number = vtkid;
            binfo.object_index = j;
            binfo.multiblock_index = index;
            this->block_info[otype][vtkid] = binfo;
        }
    }
}
