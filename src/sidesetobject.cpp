// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "sidesetobject.h"
#include "vtkMapper.h"

SideSetObject::SideSetObject(vtkAlgorithmOutput * alg_output) : MeshObject(alg_output)
{
    mapper->SetRelativeCoincidentTopologyLineOffsetParameters(-1, -1);
    mapper->SetRelativeCoincidentTopologyPolygonOffsetParameters(-1, -1);
    mapper->SetRelativeCoincidentTopologyPointOffsetParameter(-1);
}
