#pragma once

#include "vtkType.h"

class vtkActor;
class vtkDataSetMapper;
class vtkPolyDataAlgorithm;
class vtkExtractSelection;
class vtkUnstructuredGrid;
class vtkSelection;
class vtkSelectionNode;
class vtkAlgorithmOutput;

class Selection {
public:
    Selection(vtkAlgorithmOutput * input_data);
    virtual ~Selection();

    vtkActor * getActor() const;
    vtkUnstructuredGrid * getSelected() const;
    void clear();
    void selectPoint(const vtkIdType & point_id);

protected:
    void setSelection(vtkSelectionNode * selection_node);

    vtkPolyDataAlgorithm * geometry;
    vtkExtractSelection * extract_selection;
    vtkUnstructuredGrid * selected;
    vtkDataSetMapper * mapper;
    vtkActor * actor;
    vtkSelection * selection;
};
