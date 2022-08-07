#include "reader.h"

Reader::Reader(const QString & file_name) : file_name(file_name) {}

Reader::~Reader() {}

const QString &
Reader::getFileName() const
{
    return this->file_name;
}
