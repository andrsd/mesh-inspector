// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "reader.h"
#include <map>

class vtkSTLReader;

class STLReader : public Reader {
public:
    explicit STLReader(const std::string & file_name);
    ~STLReader() override;

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

    vtkSTLReader * reader;
    std::map<int, BlockInformation> block_info;
};
