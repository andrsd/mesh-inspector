#pragma once

#include "reader.h"
#include <map>

class vtkExodusIIReader;

class ExodusIIReader : public Reader {
public:
    explicit ExodusIIReader(const std::string & file_name);
    virtual ~ExodusIIReader();

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

    vtkExodusIIReader * reader;
    std::map<int, std::map<int, BlockInformation>> block_info;
};
