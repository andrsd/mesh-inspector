#pragma once

#include "reader.h"
#include <map>

class vtkUnstructuredGridReader;

class VTKReader : public Reader {
public:
    explicit VTKReader(const std::string & file_name);
    virtual ~VTKReader();

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
    void readVariableInfo();

    vtkUnstructuredGridReader * reader;
    std::map<int, BlockInformation> block_info;
};
