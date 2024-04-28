// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "gmshparsercpp/Exception.h"

namespace gmshparsercpp {

const char *
Exception::what() const noexcept
{
    return this->msg.c_str();
}

} // namespace gmshparsercpp
