#ifndef DICTMODEL_H
#define DICTMODEL_H

#include <QAbstractTableModel>
#include <QMap>


class DictModel : public QAbstractTableModel
{
public:
    DictModel(QObject *parent=nullptr);

    void setInfo(const QVector<QStringList> &dictData);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private:
    QVector<QStringList> dictData;
};

#endif // DICTMODEL_H
