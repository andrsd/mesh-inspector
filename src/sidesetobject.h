// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "meshobject.h"

class SideSetObject : public MeshObject {
public:
    explicit SideSetObject(vtkAlgorithmOutput * alg_output);
};
