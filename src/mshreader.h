// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "reader.h"
#include "vtkSmartPointer.h"
#include <map>

class vtkMshReader;

class MSHReader : public Reader {
public:
    explicit MSHReader(const std::string & file_name);
    ~MSHReader() override;

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

    vtkSmartPointer<vtkMshReader> reader;
    std::map<int, std::map<int, BlockInformation>> block_info;
};
