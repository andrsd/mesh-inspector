#include "reader.h"

Reader::Reader(const std::string & file_name) : file_name(file_name) {}

Reader::~Reader() {}

const std::string &
Reader::getFileName() const
{
    return this->file_name;
}
