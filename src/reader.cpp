// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "reader.h"

Reader::Reader(const std::string & file_name) : file_name(file_name) {}

const std::string &
Reader::getFileName() const
{
    return this->file_name;
}
