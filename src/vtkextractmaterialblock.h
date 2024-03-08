// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "vtkUnstructuredGridAlgorithm.h"
#include "vtkSmartPointer.h"

class vtkInformation;
class vtkInformationVector;

class vtkExtractMaterialBlock : public vtkUnstructuredGridAlgorithm {
public:
    vtkTypeMacro(vtkExtractMaterialBlock, vtkUnstructuredGridAlgorithm);

    static vtkExtractMaterialBlock * New();

    void SetBlockId(int blockId);

protected:
    vtkExtractMaterialBlock();
    ~vtkExtractMaterialBlock() override;

    int RequestData(vtkInformation * request,
                    vtkInformationVector ** inputVector,
                    vtkInformationVector * outputVector) override;
    int FillInputPortInformation(int port, vtkInformation * info) override;

    std::string FieldName;
    double BlockId;

private:
    vtkExtractMaterialBlock(const vtkExtractMaterialBlock &) = delete;
    void operator=(const vtkExtractMaterialBlock &) = delete;
};
