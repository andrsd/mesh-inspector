#pragma once

#include <string>
#include <vector>
#include "vtkAlgorithmOutput.h"

/// Base class for file readers
///
class Reader {
public:
    struct BlockInformation {
        std::string name;
        int object_type;
        int object_index;
        int number;
        int multiblock_index;
    };

    struct VariableInformation {
        std::string name;
        int object_type;
        int num_components;
    };

public:
    Reader(const std::string & file_name);
    virtual ~Reader();

    virtual void load() = 0;

    virtual vtkAlgorithmOutput * getVtkOutputPort() = 0;

    virtual std::vector<BlockInformation> getBlocks() = 0;

    virtual std::vector<BlockInformation> getSideSets() = 0;

    virtual std::vector<BlockInformation> getNodeSets() = 0;

    const std::string & getFileName() const;

    virtual std::size_t getTotalNumberOfElements() const = 0;

    virtual std::size_t getTotalNumberOfNodes() const = 0;

    virtual int getDimensionality() const = 0;

protected:
    std::string file_name;
};
