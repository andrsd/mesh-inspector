#pragma once

#include <QStandardItemModel>

class MeshModel : public QStandardItemModel {
public:
    MeshModel(int rows, int columns, QObject * parent = nullptr);
    explicit MeshModel(QObject * parent = nullptr);

public:
    bool setData(const QModelIndex & index, const QVariant & value, int role) override;
    QVariant data(const QModelIndex & index, int role) const override;
};
