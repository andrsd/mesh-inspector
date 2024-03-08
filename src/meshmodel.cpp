// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "meshmodel.h"
#include <QDebug>

MeshModel::MeshModel(int rows, int columns, QObject * parent) :
    QStandardItemModel(rows, columns, parent)
{
}

MeshModel::MeshModel(QObject * parent) : QStandardItemModel(parent) {}

bool
MeshModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
    if (role == Qt::CheckStateRole) {
        auto child_state = value.toBool() ? Qt::Checked : Qt::Unchecked;
        for (int row = 0; row < this->rowCount(index); row++) {
            auto child_index = this->index(row, 0, index);
            this->setData(child_index, child_state, Qt::CheckStateRole);
        }

        auto parent = this->parent(index);
        if (parent.isValid())
            emit this->dataChanged(parent, parent);
    }
    return QStandardItemModel::setData(index, value, role);
}

QVariant
MeshModel::data(const QModelIndex & index, int role) const
{
    if ((role == Qt::CheckStateRole) && (this->flags(index) & Qt::ItemIsAutoTristate)) {
        int n_checked = 0;
        int n_unchecked = 0;
        int n_childs = this->rowCount(index);

        for (int row = 0; row < this->rowCount(index); row++) {
            auto child_index = this->index(row, 0, index);
            auto child_state = this->data(child_index, Qt::CheckStateRole);
            if (child_state == Qt::PartiallyChecked)
                return Qt::PartiallyChecked;
            else if (child_state == Qt::Checked)
                n_checked++;
            else if (child_state == Qt::Unchecked)
                n_unchecked++;
        }
        if (n_checked == n_childs)
            return Qt::Checked;
        else if (n_unchecked == n_childs)
            return Qt::Unchecked;
        else
            return Qt::PartiallyChecked;
    }
    return QStandardItemModel::data(index, role);
}
