#pragma once

#include <QStandardItemModel>

class MeshModel : public QStandardItemModel {
public:
    MeshModel(int rows, int columns, QObject * parent = nullptr);
    MeshModel(QObject * parent = nullptr);

public:
    virtual bool
    setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;
    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
};
