#pragma once

#include <QString>

/// Base class for file readers
///
class Reader {
public:
    Reader(const QString & file_name);

    virtual void load() = 0;

    // virtual getVtkOutputPort() const = 0;

    // virtual getBlocks() const = 0;

    // virtual A getSideSets() const = 0;

    // virtual A getNodeSets() const = 0;

    const QString & getFileName() const;

    // virtual A getVariableInfo() const = 0;

    virtual void getTotalNumberOfElements() const = 0;

    virtual std::size_t getTotalNumberOfNodes() const = 0;

    virtual void getDimensionality() const = 0;

protected:
    QString file_name;
};
