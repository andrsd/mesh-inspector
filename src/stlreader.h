#pragma once

#include "reader.h"
#include <map>

class vtkSTLReader;

class STLReader : public Reader {
public:
    explicit STLReader(const std::string & file_name);
    virtual ~STLReader();

    virtual void load() override;
    virtual std::size_t getTotalNumberOfElements() const override;
    virtual std::size_t getTotalNumberOfNodes() const override;
    virtual int getDimensionality() const override;

    virtual vtkAlgorithmOutput * getVtkOutputPort() override;
    virtual std::vector<Reader::BlockInformation> getBlocks() override;
    virtual std::vector<Reader::BlockInformation> getSideSets() override;
    virtual std::vector<Reader::BlockInformation> getNodeSets() override;

protected:
    void readBlockInfo();

    vtkSTLReader * reader;
    std::map<int, BlockInformation> block_info;
};
