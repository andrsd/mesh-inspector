// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "vtkMultiBlockDataSetAlgorithm.h"
#include "vtkPoints.h"
#include "vtkSmartPointer.h"
#include "gmshparsercpp/MshFile.h"

class vtkMutableDirectedGraph;
class vtkUnstructuredGrid;

/// VTK object to read GMSH mesh files
///
/// NOTE: coding style is closer to VTK in case this will ever gets moved there
class vtkMshReader : public vtkMultiBlockDataSetAlgorithm {
public:
    static vtkMshReader * New();
    vtkTypeMacro(vtkMshReader, vtkMultiBlockDataSetAlgorithm);
    void PrintSelf(ostream & os, vtkIndent indent) override;

    virtual void SetFileName(VTK_FILEPATH const char * fname);
    vtkGetFilePathMacro(FileName);

    int GetDimensionality();

    virtual vtkIdType GetTotalNumberOfNodes();
    virtual vtkIdType GetTotalNumberOfEdges();
    virtual vtkIdType GetTotalNumberOfFaces();
    virtual vtkIdType GetTotalNumberOfElements();

    vtkMutableDirectedGraph *
    GetSIL()
    {
        return this->SIL;
    }

    enum ObjectType { ELEM_BLOCK = 0, SIDE_SET = 1 };

    int GetNumberOfObjects(int objectType);
    int GetObjectId(int objectType, int objectIndex);
    const char * GetObjectName(int objectType, int objectIndex);

protected:
    vtkMshReader();
    ~vtkMshReader() override;

    void BuildSIL();
    vtkTypeBool ProcessRequest(vtkInformation * request,
                               vtkInformationVector ** inputVector,
                               vtkInformationVector * outputVector) override;
    int
    RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;
    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;
    void DetectDimensionality();
    void ReadPhysicalEntities();
    void BuildCoordinates();
    void ProcessMsh();
    const std::vector<gmshparsercpp::MshFile::MultiDEntity> * GetEntitiesByDim(int dim);
    vtkPoints * BuildLocalPoints(const std::map<long, vtkIdType> & nodeMap);
    vtkIdType GetLocalPointId(std::map<long, vtkIdType> & nodeMap, int nodeId);
    vtkUnstructuredGrid * CreateUnstructuredGrid(
        const std::vector<const gmshparsercpp::MshFile::ElementBlock *> & blocks);
    std::string GetMshPhysBlockName(int physId);

    char * FileName;
    vtkTimeStamp FileNameMTime;
    vtkMutableDirectedGraph * SIL;
    int SILUpdateStamp;
    gmshparsercpp::MshFile * Msh;
    /// Spatial dimension
    int Dimension;
    ///
    std::map<long, const gmshparsercpp::MshFile::PhysicalName *> PhysEntByTag;
    ///
    std::vector<std::vector<const gmshparsercpp::MshFile::ElementBlock *>> ElemBlkByDim;
    ///
    vtkSmartPointer<vtkPoints> AllPoints;
    ///
    std::map<int, std::vector<int>> ObjectIds;
    std::map<int, std::vector<std::string>> ObjectNames;
    ///
    vtkIdType TotalNumOfNodes;
    vtkIdType TotalNumOfElems;

private:
    vtkMshReader(const vtkMshReader &) = delete;
    void operator=(const vtkMshReader &) = delete;
};
