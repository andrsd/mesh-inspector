#include "sidesetobject.h"
#include "vtkMapper.h"

SideSetObject::SideSetObject(vtkAlgorithmOutput * alg_output) : MeshObject(alg_output)
{
    mapper->SetRelativeCoincidentTopologyLineOffsetParameters(-1, -1);
    mapper->SetRelativeCoincidentTopologyPolygonOffsetParameters(-1, -1);
    mapper->SetRelativeCoincidentTopologyPointOffsetParameter(-1);
}
