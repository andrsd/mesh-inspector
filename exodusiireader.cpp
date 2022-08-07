#include "exodusiireader.h"
#include "vtkExodusIIReader.h"

ExodusIIReader::ExodusIIReader(const QString & file_name) : Reader(file_name), reader(nullptr)
{
    // self._block_info = dict()
    // self._variable_info = dict()
    // self._times = None
}

ExodusIIReader::~ExodusIIReader()
{
}

void
ExodusIIReader::load()
{
    this->reader = vtkExodusIIReader::New();
}

std::size_t
ExodusIIReader::getTotalNumberOfElements() const
{
    return this->reader->GetTotalNumberOfElements();
}

std::size_t
ExodusIIReader::getTotalNumberOfNodes() const
{
    return this->reader->GetTotalNumberOfNodes();
}

int
ExodusIIReader::getDimensionality() const
{
    return this->reader->GetDimensionality();
}

void
ExodusIIReader::readBlockInfo()
{
}

void
ExodusIIReader::readVariableInfo()
{
}

void
ExodusIIReader::readTimeInfo()
{
}
