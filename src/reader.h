// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <string>
#include <vector>
#include "vtkAlgorithmOutput.h"

class vtkPolyData;

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
        int material_index;
    };

    struct VariableInformation {
        std::string name;
        int object_type;
        int num_components;
    };

public:
    explicit Reader(const std::string & file_name);
    virtual ~Reader() = default;

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
