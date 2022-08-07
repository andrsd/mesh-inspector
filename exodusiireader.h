#pragma once

#include "reader.h"

class vtkExodusIIReader;

class ExodusIIReader : public Reader {
public:
    explicit ExodusIIReader(const QString & file_name);
    virtual ~ExodusIIReader();

    virtual void load() override;
    virtual std::size_t getTotalNumberOfElements() const override;
    virtual std::size_t getTotalNumberOfNodes() const override;
    virtual int getDimensionality() const override;

protected:
    void readBlockInfo();
    void readVariableInfo();
    void readTimeInfo();

    vtkExodusIIReader * reader;
};
