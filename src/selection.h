// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

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
    explicit Selection(vtkAlgorithmOutput * input_data);
    virtual ~Selection();

    vtkActor * getActor() const;
    vtkUnstructuredGrid * getSelected() const;
    void clear();
    void selectPoint(const vtkIdType & point_id);
    void selectCell(const vtkIdType & cell_id);

protected:
    void setSelection(vtkSelectionNode * selection_node);
    void freeSelection();

    vtkPolyDataAlgorithm * geometry;
    vtkExtractSelection * extract_selection;
    vtkUnstructuredGrid * selected;
    vtkDataSetMapper * mapper;
    vtkActor * actor;
    vtkSelection * selection;
};
