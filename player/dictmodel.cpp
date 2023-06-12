#include "dictmodel.h"

DictModel::DictModel(QObject *parent):
    QAbstractTableModel(parent)
{

}

void DictModel::setInfo(const QVector<QStringList> &dictData) {
    beginResetModel();
    this->dictData = dictData;
    endResetModel();
}

int DictModel::rowCount(const QModelIndex &parent) const {
    return dictData.count();
}

int DictModel::columnCount(const QModelIndex &parent) const {
    return 3;  //
}


QVariant DictModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }
    if (role == Qt::TextAlignmentRole) {
        return int(Qt::AlignLeft | Qt::AlignVCenter);
    } else if (role == Qt::DisplayRole) {
        int row = index.row();
        int column = index.column();
        return dictData.at(row).at(column);
    }
    return QVariant();
}


QVariant DictModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    if (orientation == Qt::Vertical) {
        return QVariant();
    }
    const static QStringList header{"单词", "读音", "音标"};
    return header.at(section);
}
