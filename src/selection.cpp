#include "selection.h"
#include "vtkActor.h"
#include "vtkDataSetMapper.h"
#include "vtkCompositeDataGeometryFilter.h"
#include "vtkGeometryFilter.h"
#include "vtkExtractSelection.h"
#include "vtkUnstructuredGrid.h"
#include "vtkSelection.h"
#include "vtkSelectionNode.h"
#include "vtkAlgorithmOutput.h"
#include "vtkMultiBlockDataSet.h"

Selection::Selection(vtkAlgorithmOutput * input_data) :
    geometry(nullptr),
    extract_selection(vtkExtractSelection::New()),
    selected(vtkUnstructuredGrid::New()),
    mapper(vtkDataSetMapper::New()),
    actor(vtkActor::New()),
    selection(vtkSelection::New())
{
    auto * algoritm = input_data->GetProducer();
    auto * data_object = algoritm->GetOutputDataObject(0);
    if (dynamic_cast<vtkMultiBlockDataSet *>(data_object))
        this->geometry = vtkCompositeDataGeometryFilter::New();
    else
        this->geometry = vtkGeometryFilter::New();
    this->geometry->SetInputConnection(0, input_data);
    this->geometry->Update();
    this->extract_selection->SetInputData(0, this->geometry->GetOutput());
    this->mapper->SetInputData(this->selected);
    this->mapper->SetScalarModeToUsePointFieldData();
    this->mapper->InterpolateScalarsBeforeMappingOn();
    this->actor->SetMapper(this->mapper);
}

Selection::~Selection()
{
    this->geometry->Delete();
    this->extract_selection->Delete();
    this->selected->Delete();
    this->mapper->Delete();
    this->actor->Delete();
    this->selection->Delete();
}

vtkActor *
Selection::getActor() const
{
    return this->actor;
}

vtkUnstructuredGrid *
Selection::getSelected() const
{
    return this->selected;
}

void
Selection::clear()
{
    this->selected->Initialize();
    this->mapper->SetInputData(this->selected);
}

void
Selection::selectPoint(const vtkIdType & point_id)
{
    auto * ids = vtkIdTypeArray::New();
    ids->SetNumberOfComponents(1);
    ids->InsertNextValue(point_id);

    auto * selection_node = vtkSelectionNode::New();
    selection_node->SetFieldType(vtkSelectionNode::POINT);
    selection_node->SetContentType(vtkSelectionNode::INDICES);
    selection_node->SetSelectionList(ids);

    setSelection(selection_node);
}

void
Selection::setSelection(vtkSelectionNode * selection_node)
{
    this->selection->Delete();
    this->selection = vtkSelection::New();
    this->selection->AddNode(selection_node);

    this->extract_selection->SetInputData(1, this->selection);
    this->extract_selection->Update();

    this->selected->Delete();
    this->selected = vtkUnstructuredGrid::New();
    this->selected->ShallowCopy(this->extract_selection->GetOutput());

    this->mapper->SetInputData(this->selected);
}
