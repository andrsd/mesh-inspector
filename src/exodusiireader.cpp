// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "exodusiireader.h"
#include "vtkExodusIIReader.h"

ExodusIIReader::ExodusIIReader(const std::string & file_name) : Reader(file_name), reader(nullptr)
{
}

ExodusIIReader::~ExodusIIReader()
{
    if (this->reader)
        this->reader->Delete();
}

void
ExodusIIReader::load()
{
    this->reader = vtkExodusIIReader::New();

    this->reader->SetFileName(this->file_name.c_str());
    this->reader->UpdateInformation();
    this->reader->Update();

    readBlockInfo();
    for (auto & it : this->block_info) {
        for (auto & jt : it.second) {
            auto & info = jt.second;
            this->reader->SetObjectStatus(info.object_type, info.object_index, 1);
        }
    }
}

std::size_t
ExodusIIReader::getTotalNumberOfElements() const
{
    return this->reader->GetTotalNumberOfElements();
}

std::size_t
ExodusIIReader::getTotalNumberOfNodes() const
{
    return this->reader->GetTotalNumberOfNodes();
}

int
ExodusIIReader::getDimensionality() const
{
    return this->reader->GetDimensionality();
}

vtkAlgorithmOutput *
ExodusIIReader::getVtkOutputPort()
{
    return this->reader->GetOutputPort(0);
}

std::vector<Reader::BlockInformation>
ExodusIIReader::getBlocks()
{
    std::vector<BlockInformation> blocks;
    for (auto & it : this->block_info[vtkExodusIIReader::ELEM_BLOCK])
        blocks.push_back(it.second);
    return blocks;
}

std::vector<Reader::BlockInformation>
ExodusIIReader::getSideSets()
{
    std::vector<BlockInformation> sidesets;
    for (auto & it : this->block_info[vtkExodusIIReader::SIDE_SET])
        sidesets.push_back(it.second);
    return sidesets;
}

std::vector<Reader::BlockInformation>
ExodusIIReader::getNodeSets()
{
    std::vector<BlockInformation> nodesets;
    for (auto & it : this->block_info[vtkExodusIIReader::NODE_SET])
        nodesets.push_back(it.second);
    return nodesets;
}

void
ExodusIIReader::readBlockInfo()
{
    std::vector<int> obj_types = { vtkExodusIIReader::ELEM_BLOCK, vtkExodusIIReader::FACE_BLOCK,
                                   vtkExodusIIReader::EDGE_BLOCK, vtkExodusIIReader::ELEM_SET,
                                   vtkExodusIIReader::SIDE_SET,   vtkExodusIIReader::FACE_SET,
                                   vtkExodusIIReader::EDGE_SET,   vtkExodusIIReader::NODE_SET };

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
            binfo.material_index = -1;
            this->block_info[otype][vtkid] = binfo;
        }
    }
}
