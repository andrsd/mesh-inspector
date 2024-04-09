// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "vtkmshreader.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkObjectFactory.h"
#include "vtkDemandDrivenPipeline.h"
#include "vtkLogger.h"
#include "vtkMutableDirectedGraph.h"
#include "vtkSmartPointer.h"
#include "vtkVariantArray.h"
#include "vtkUnsignedCharArray.h"
#include "vtkDataSetAttributes.h"
#include "vtkStringArray.h"
#include "vtkUnstructuredGrid.h"
#include <deque>

vtkObjectFactoryNewMacro(vtkMshReader);

static std::array<const char *, 2> conn_types_names = { "Element Blocks", "Side Sets" };

static std::map<gmshparsercpp::ElementType, int> msh_cell_type_to_vtk = {
    { gmshparsercpp::LINE2, VTK_LINE },      { gmshparsercpp::TRI3, VTK_TRIANGLE },
    { gmshparsercpp::QUAD4, VTK_QUAD },      { gmshparsercpp::TET4, VTK_TETRA },
    { gmshparsercpp::HEX8, VTK_HEXAHEDRON }, { gmshparsercpp::PRISM6, VTK_WEDGE },
    { gmshparsercpp::PYRAMID5, VTK_PYRAMID }
};

vtkMshReader::vtkMshReader()
{
    this->Dimension = -1;
    this->FileName = nullptr;
    this->SIL = vtkMutableDirectedGraph::New();
    this->SILUpdateStamp = -1;
    this->Msh = nullptr;
    this->SetNumberOfInputPorts(0);
    this->TotalNumOfNodes = -1;
    this->TotalNumOfElems = -1;
}

vtkMshReader::~vtkMshReader()
{
    this->SIL->Delete();
    delete this->Msh;
}

void
vtkMshReader::PrintSelf(ostream & os, vtkIndent indent)
{
}

#define vtkSetStringMacroBody(propName, fname)                        \
    int modified = 0;                                                 \
    if (fname == this->propName)                                      \
        return;                                                       \
    if (fname && this->propName && !strcmp(fname, this->propName))    \
        return;                                                       \
    modified = 1;                                                     \
    delete[] this->propName;                                          \
    if (fname) {                                                      \
        this->propName = vtksys::SystemTools::DuplicateString(fname); \
    }                                                                 \
    else {                                                            \
        this->propName = nullptr;                                     \
    }

void
vtkMshReader::SetFileName(VTK_FILEPATH const char * fname)
{
    vtkLogF(TRACE, "%s: SetFileName old=%s, new=%s", vtkLogIdentifier(this), this->FileName, fname);
    vtkSetStringMacroBody(FileName, fname);
    if (modified) {
        this->FileNameMTime.Modified();
    }
}

int
vtkMshReader::GetDimensionality()
{
    return this->Dimension;
}

vtkIdType
vtkMshReader::GetTotalNumberOfNodes()
{
    return this->TotalNumOfNodes;
}

vtkIdType
vtkMshReader::GetTotalNumberOfEdges()
{
    return 0;
}

vtkIdType
vtkMshReader::GetTotalNumberOfFaces()
{
    return 0;
}

vtkIdType
vtkMshReader::GetTotalNumberOfElements()
{
    return this->TotalNumOfElems;
}

void
vtkMshReader::BuildSIL()
{
    this->SIL->Initialize();

    // Build a minimal SIL with only the blocks.
    vtkSmartPointer<vtkVariantArray> childEdge = vtkSmartPointer<vtkVariantArray>::New();
    childEdge->InsertNextValue(0);

    vtkSmartPointer<vtkVariantArray> crossEdge = vtkSmartPointer<vtkVariantArray>::New();
    crossEdge->InsertNextValue(0);

    // CrossEdge is an edge linking hierarchies.
    vtkUnsignedCharArray * crossEdgesArray = vtkUnsignedCharArray::New();
    crossEdgesArray->SetName("CrossEdges");
    this->SIL->GetEdgeData()->AddArray(crossEdgesArray);
    crossEdgesArray->Delete();

    std::deque<std::string> names;
    int cc;

    // Now build the hierarchy.
    vtkIdType rootId = this->SIL->AddVertex();
    names.emplace_back("SIL");

    // Add the ELEM_BLOCK subtree.
    vtkIdType blocksRoot = this->SIL->AddChild(rootId, childEdge);
    names.emplace_back("Blocks");

    // This is the map of block names to node ids.
    std::map<std::string, vtkIdType> blockIds;
    if (this->Msh) {
        for (auto & physBlk : this->Msh->get_physical_names()) {
            if (physBlk.dimension == this->Dimension) {
                vtkIdType child = this->SIL->AddChild(blocksRoot, childEdge);
                auto blockName = physBlk.name;
                names.push_back(blockName);
                blockIds[blockName] = child;
            }
        }
    }

    // This array is used to assign names to nodes.
    vtkStringArray * namesArray = vtkStringArray::New();
    namesArray->SetName("Names");
    namesArray->SetNumberOfTuples(this->SIL->GetNumberOfVertices());
    this->SIL->GetVertexData()->AddArray(namesArray);
    namesArray->Delete();

    std::deque<std::string>::iterator iter;
    for (cc = 0, iter = names.begin(); iter != names.end(); ++iter, ++cc) {
        namesArray->SetValue(cc, (*iter).c_str());
    }
}

vtkTypeBool
vtkMshReader::ProcessRequest(vtkInformation * request,
                             vtkInformationVector ** inputVector,
                             vtkInformationVector * outputVector)
{
    if (request->Has(vtkDemandDrivenPipeline::REQUEST_DATA())) {
        return this->RequestData(request, inputVector, outputVector);
    }

    // execute information
    if (request->Has(vtkDemandDrivenPipeline::REQUEST_INFORMATION())) {
        return this->RequestInformation(request, inputVector, outputVector);
    }

    return this->Superclass::ProcessRequest(request, inputVector, outputVector);
}

int
vtkMshReader::RequestInformation(vtkInformation * vtkNotUsed(request),
                                 vtkInformationVector ** vtkNotUsed(inputVector),
                                 vtkInformationVector * outputVector)
{
    vtkInformation * outInfo = outputVector->GetInformationObject(0);

    try {
        this->Msh = new gmshparsercpp::MshFile(this->FileName);
        this->Msh->parse();
        ProcessMsh();

        BuildSIL();
        this->SILUpdateStamp++;

        this->Msh->close();
    }
    catch (std::system_error & e) {
        vtkErrorMacro("" << e.what());
        return 0;
    }
    catch (std::domain_error & e) {
        vtkErrorMacro("" << e.what());
        return 0;
    }
    catch (...) {
        vtkErrorMacro("Unknown exception thrown");
        return 0;
    }

    // Advertise the SIL.
    outInfo->Set(vtkDataObject::SIL(), this->GetSIL());

    return 1;
}

int
vtkMshReader::RequestData(vtkInformation * vtkNotUsed(request),
                          vtkInformationVector ** vtkNotUsed(inputVector),
                          vtkInformationVector * outputVector)
{
    if (!this->FileName) {
        vtkErrorMacro("Unable to open file \"" << (this->FileName ? this->FileName : "(null)")
                                               << "\" to read data");
        return 0;
    }

    if (this->Dimension >= 1 && this->Dimension <= 3) {
        vtkInformation * outInfo = outputVector->GetInformationObject(0);
        vtkMultiBlockDataSet * output =
            vtkMultiBlockDataSet::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

        this->ObjectIds.clear();
        this->ObjectNames.clear();

        // Map: {dim -> { physId -> [blockId, blockId, ...] }}
        std::map<int, std::map<int, std::vector<int>>> physBlocksByDim;
        for (int dim = this->Dimension; dim > 0; dim--) {
            for (const auto & ent : *GetEntitiesByDim(dim)) {
                if (!ent.physical_tags.empty())
                    physBlocksByDim[dim][ent.physical_tags[0]].push_back(ent.tag);
                else
                    physBlocksByDim[dim][ent.tag].push_back(ent.tag);
            }
        }

        // Map: { dim -> { blokId -> ElementBlock }}
        std::map<int, std::map<int, const gmshparsercpp::MshFile::ElementBlock *>> blocksByDimById;
        for (int dim = this->Dimension; dim > 0; dim--) {
            for (auto & blk : this->ElemBlkByDim[dim]) {
                auto id = blk->tag;
                blocksByDimById[dim][id] = blk;
            }
        }

        const int nBlockTypes = 2;
        std::array<ObjectType, nBlockTypes> objTypes = { vtkMshReader::ELEM_BLOCK,
                                                         vtkMshReader::SIDE_SET };
        std::array<int, nBlockTypes> dims = { this->Dimension, this->Dimension - 1 };
        output->SetNumberOfBlocks(nBlockTypes);

        for (int i = 0; i < nBlockTypes; i++) {
            ObjectType objType = objTypes[i];
            int dim = dims[i];

            auto mbds = vtkMultiBlockDataSet::New();
            mbds->SetNumberOfBlocks(physBlocksByDim[dim].size());
            output->SetBlock(i, mbds);
            output->GetMetaData(i)->Set(vtkCompositeDataSet::NAME(), conn_types_names[i]);
            mbds->FastDelete();

            long idx = 0;
            for (auto & [physId, blockIds] : physBlocksByDim[dim]) {
                std::vector<const gmshparsercpp::MshFile::ElementBlock *> blocks;
                for (auto & blkId : blockIds) {
                    if (blocksByDimById[dim].count(blkId) == 1)
                        blocks.push_back(blocksByDimById[dim][blkId]);
                }

                if (!blocks.empty()) {
                    std::string blockName = GetMshPhysBlockName(physId);
                    this->ObjectIds[objType].push_back(physId);
                    this->ObjectNames[objType].push_back(blockName);

                    auto ug = CreateUnstructuredGrid(blocks);
                    mbds->SetBlock(idx, ug);
                    mbds->GetMetaData(idx)->Set(vtkCompositeDataSet::NAME(), blockName);
                    ug->FastDelete();

                    idx++;
                }
            }
        }
    }

    return 1;
}

void
vtkMshReader::DetectDimensionality()
{
    if (!this->Msh->get_volume_entities().empty())
        this->Dimension = 3;
    else if (!this->Msh->get_surface_entities().empty())
        this->Dimension = 2;
    else if (!this->Msh->get_curve_entities().empty())
        this->Dimension = 1;
    else
        this->Dimension = -1;
}

void
vtkMshReader::ProcessMsh()
{
    DetectDimensionality();
    BuildCoordinates();
    ReadPhysicalEntities();

    this->ElemBlkByDim.resize(4);
    for (const auto & eb : this->Msh->get_element_blocks())
        this->ElemBlkByDim[eb.dimension].push_back(&eb);

    // count total number of elements and nodes
    std::map<int, int> nodeCnt;
    this->TotalNumOfElems = 0;
    for (const auto & eb : this->Msh->get_element_blocks()) {
        if (eb.dimension == this->Dimension) {
            this->TotalNumOfElems += eb.elements.size();
            for (auto & e : eb.elements) {
                for (auto & nid : e.node_tags)
                    nodeCnt[nid]++;
            }
        }
    }
    this->TotalNumOfNodes = nodeCnt.size();
}

void
vtkMshReader::ReadPhysicalEntities()
{
    for (const auto & pe : this->Msh->get_physical_names()) {
        this->PhysEntByTag[pe.tag] = &pe;
    }
}

void
vtkMshReader::BuildCoordinates()
{
    this->AllPoints = vtkPoints::New();
    for (const auto & nd : this->Msh->get_nodes()) {
        if (!nd.tags.empty()) {
            for (std::size_t j = 0; j < nd.tags.size(); j++) {
                const auto & id = nd.tags[j];
                const auto & c = nd.coordinates[j];
                this->AllPoints->InsertPoint(id, c.x, c.y, c.z);
            }
        }
    }
}

const std::vector<gmshparsercpp::MshFile::MultiDEntity> *
vtkMshReader::GetEntitiesByDim(int dim)
{
    const std::vector<gmshparsercpp::MshFile::MultiDEntity> * ents = nullptr;
    switch (dim) {
    case 1:
        return &this->Msh->get_curve_entities();
    case 2:
        return &this->Msh->get_surface_entities();
    case 3:
        return &this->Msh->get_volume_entities();
    default:
        vtkErrorMacro("Unsupported spatial dimension: " << dim);
        return nullptr;
    }
}

vtkPoints *
vtkMshReader::BuildLocalPoints(const std::map<long, vtkIdType> & nodeMap)
{
    auto pts = vtkPoints::New();
    pts->SetNumberOfPoints(nodeMap.size());
    for (auto & [gid, lid] : nodeMap) {
        auto coord = this->AllPoints->GetPoint(gid);
        pts->SetPoint(lid, coord);
    }
    return pts;
}

vtkIdType
vtkMshReader::GetLocalPointId(std::map<long, vtkIdType> & nodeMap, int nodeId)
{
    vtkIdType pointId;
    auto it = nodeMap.find(nodeId);
    if (it == nodeMap.end()) {
        pointId = nodeMap.size();
        nodeMap[nodeId] = pointId;
    }
    else
        pointId = it->second;
    return pointId;
}

int
vtkMshReader::GetNumberOfObjects(int objectType)
{
    if (this->ObjectIds.count(objectType) == 1)
        return this->ObjectIds[objectType].size();
    else
        return -1;
}

int
vtkMshReader::GetObjectId(int objectType, int objectIndex)
{
    if (this->ObjectIds.count(objectType) == 1) {
        const auto & arr = this->ObjectIds[objectType];
        if (objectIndex >= 0 && objectIndex < arr.size())
            return arr[objectIndex];
        else
            return -1;
    }
    else
        return -1;
}

const char *
vtkMshReader::GetObjectName(int objectType, int objectIndex)
{
    if (this->ObjectIds.count(objectType) == 1) {
        const auto & arr = this->ObjectNames[objectType];
        if (objectIndex >= 0 && objectIndex < arr.size())
            return arr[objectIndex].c_str();
        else
            return nullptr;
    }
    else
        return nullptr;
}

vtkUnstructuredGrid *
vtkMshReader::CreateUnstructuredGrid(
    const std::vector<const gmshparsercpp::MshFile::ElementBlock *> & blocks)
{
    vtkUnstructuredGrid * ug = vtkUnstructuredGrid::New();

    vtkIdType numCells = 0;
    for (auto & blk : blocks)
        numCells += blk->elements.size();

    ug->Allocate(numCells);

    std::map<long, vtkIdType> localNodeMap;
    for (auto & blk : blocks) {
        if (msh_cell_type_to_vtk.count(blk->element_type) == 1) {
            auto cellType = msh_cell_type_to_vtk[blk->element_type];
            for (const auto & elem : blk->elements) {
                auto nNodeTags = elem.node_tags.size();
                std::vector<vtkIdType> cellNodes(nNodeTags);
                for (std::size_t i = 0; i < nNodeTags; i++) {
                    const auto & nid = elem.node_tags[i];
                    vtkIdType pointId = GetLocalPointId(localNodeMap, nid);
                    cellNodes[i] = pointId;
                }
                ug->InsertNextCell(cellType, nNodeTags, cellNodes.data());
            }
        }
        else {
            // unknown element type, so skip this block
        }
    }

    auto pts = BuildLocalPoints(localNodeMap);
    ug->SetPoints(pts);

    return ug;
}

std::string
vtkMshReader::GetMshPhysBlockName(int physId)
{
    std::string blockName = std::to_string(physId);
    auto it = this->PhysEntByTag.find(physId);
    if (it != this->PhysEntByTag.end())
        blockName = it->second->name;
    return blockName;
}
