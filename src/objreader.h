#pragma once

#include "reader.h"
#include <map>

class vtkOBJReader;

class OBJReader : public Reader {
public:
    explicit OBJReader(const std::string & file_name);
    ~OBJReader() override;

    void load() override;
    std::size_t getTotalNumberOfElements() const override;
    std::size_t getTotalNumberOfNodes() const override;
    int getDimensionality() const override;

    vtkAlgorithmOutput * getVtkOutputPort() override;
    std::vector<Reader::BlockInformation> getBlocks() override;
    std::vector<Reader::BlockInformation> getSideSets() override;
    std::vector<Reader::BlockInformation> getNodeSets() override;

protected:
    void readBlockInfo();

    vtkOBJReader * reader;
    std::map<int, BlockInformation> block_info;
};
