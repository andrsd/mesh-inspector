// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "reader.h"
#include "vtkSmartPointer.h"
#include <map>

class vtkUnstructuredGridReader;
class vtkXMLUnstructuredGridReader;

class VTKReader : public Reader {
public:
    explicit VTKReader(const std::string & file_name);
    ~VTKReader();

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

    vtkSmartPointer<vtkUnstructuredGridReader> reader;
    vtkSmartPointer<vtkXMLUnstructuredGridReader> xml_reader;
    std::map<int, BlockInformation> block_info;
};
