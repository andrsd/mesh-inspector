#include "vtkextractmaterialblock.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkDataSet.h"
#include "vtkCellData.h"
#include "vtkExtractCells.h"
#include "vtkIdList.h"
#include "vtkUnstructuredGrid.h"
#include "vtkCellIterator.h"

vtkStandardNewMacro(vtkExtractMaterialBlock);

vtkExtractMaterialBlock::vtkExtractMaterialBlock()
{
    this->FieldName = "MaterialIds";
}

vtkExtractMaterialBlock::~vtkExtractMaterialBlock() {}

void
vtkExtractMaterialBlock::SetBlockId(int blockId)
{
    this->BlockId = blockId;
}

int
vtkExtractMaterialBlock::RequestData(vtkInformation * request,
                                     vtkInformationVector ** inputVector,
                                     vtkInformationVector * outputVector)
{
    // get the info objects
    auto in_info = inputVector[0]->GetInformationObject(0);
    auto out_info = outputVector->GetInformationObject(0);

    // get the input and output
    auto input = vtkDataSet::SafeDownCast(in_info->Get(vtkDataObject::DATA_OBJECT()));
    auto output = vtkUnstructuredGrid::SafeDownCast(out_info->Get(vtkDataObject::DATA_OBJECT()));

    auto cd = input->GetCellData();
    auto mat_ids = cd->GetScalars(this->FieldName.c_str());

    auto output_cd = output->GetCellData();

    // As this filter is doing a subsetting operation, set the Copy Tuple flag
    // for GlobalIds array so that, if present, it will be copied to the output.
    output_cd->CopyGlobalIdsOn();

    auto new_cell_pts = vtkIdList::New();
    new_cell_pts->Allocate(VTK_CELL_SIZE);

    vtkIdType n_pts = input->GetNumberOfPoints();
    vtkIdType n_cells = input->GetNumberOfCells();
    vtkIdType * point_map = new vtkIdType[n_pts]; // maps old point ids into new
    for (vtkIdType i = 0; i < n_pts; i++)
        point_map[i] = -1;

    output->Allocate(n_cells / 4); // allocate storage for geometry/topology
    auto new_pts = vtkPoints::New();
    new_pts->Allocate(n_pts / 4, n_pts);
    output_cd->CopyAllocate(cd);

    auto cell_iter = vtkSmartPointer<vtkCellIterator>::Take(input->NewCellIterator());
    for (cell_iter->InitTraversal(); !cell_iter->IsDoneWithTraversal(); cell_iter->GoToNextCell()) {
        auto cell_type = cell_iter->GetCellType();
        auto num_cell_pts = cell_iter->GetNumberOfPoints();
        auto point_id_list = cell_iter->GetPointIds();
        auto cell_id = cell_iter->GetCellId();
        auto val = mat_ids->GetTuple1(cell_id);
        if (val == this->BlockId) {
            for (vtkIdType npts = 0, i = 0; i < num_cell_pts; i++, npts++) {
                auto pt_id = point_id_list->GetId(i);

                if (point_map[pt_id] < 0) {
                    double x[3];
                    input->GetPoint(pt_id, x);
                    auto new_id = new_pts->InsertNextPoint(x);
                    point_map[pt_id] = new_id;
                }

                new_cell_pts->InsertId(i, point_map[pt_id]);
            }

            auto new_cell_id = output->InsertNextCell(cell_type, new_cell_pts);
            output_cd->CopyData(cd, cell_id, new_cell_id);
        }
        new_cell_pts->Reset();
    }

    delete[] point_map;
    new_cell_pts->Delete();
    output->SetPoints(new_pts);
    new_pts->Delete();

    output->Squeeze();

    return 1;
}

int
vtkExtractMaterialBlock::FillInputPortInformation(int, vtkInformation * info)
{
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
    return 1;
}
