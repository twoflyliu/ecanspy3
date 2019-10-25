#include "filtertablemodel.h"
#include "mainwindow.h"

#include <QDebug>

QString FilterTableModel::columnTitles[FilterTableModel::COLUMN_COUNT] = {
    QObject::tr("BegID(Hex)"), QObject::tr("EndID(Hex)"), QObject::tr("En")
};

FilterTableModel::FilterTableModel(QObject *parent) : QAbstractTableModel(parent)
{

}

void FilterTableModel::setChecked(int row, bool enabled)
{
    if (row < filterList.size()) {
        if (filterList[row].enabled != enabled) {
            filterList[row].enabled = enabled;
            emit updateFilter();
        }
    }
}

QList<FilterTableModel::Entity> *FilterTableModel::getFilterList()
{
    return &filterList;
}


int FilterTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return filterList.size();
}

int FilterTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return COLUMN_COUNT;
}

QVariant FilterTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();

    if (Qt::TextAlignmentRole == role) {
        return int(Qt::AlignRight|Qt::AlignVCenter);
    }

    if (role != Qt::DisplayRole && role != Qt::EditRole) return QVariant();

    int row = index.row();
    int column = index.column();

    if (0 == column) {
        return QString("%1").arg(filterList[row].idStart, 3, 16, QChar('0')).toUpper();
    } else if (1 == column) {
        return QString("%1").arg(filterList[row].idEnd, 3, 16, QChar('0')).toUpper();
    }

    return QVariant();
}

QVariant FilterTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) return QVariant();
    if (Qt::Horizontal == orientation) {
        return columnTitles[section];
    } else {
        return QString("%1 ").arg(section+1);
    }
}

bool FilterTableModel::insertRows(int position, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    beginInsertRows(QModelIndex(), position, position + count - 1);
    for (int row = 0; row < count; ++row) {
        filterList.insert(position, Entity());
    }
    endInsertRows();
    return true;
}

bool FilterTableModel::removeRows(int position, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    beginRemoveRows(QModelIndex(), position, position + count - 1);
    for (int row = 0; row < count; ++row) {
        filterList.removeAt(position);
    }
    endRemoveRows();
    return true;
}

static inline quint32 hexToUINT(const QString str, bool *ok)
{
    QByteArray stream(str.toAscii());
    return stream.toUInt(ok, 16);
}

bool FilterTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole) return false;
    int column = index.column();
    bool ok = false;

    UPDATE_DATAChANGED_FLAG();

    if (0 == column) {
        auto &entity = filterList[index.row()];
        auto id = hexToUINT(value.toString(), &ok);
        if (ok) {
            if (entity.idStart != id) {
                entity.idStart = id;
                emit updateFilter();
            }
        }
        return true;
    } else if (1 == column) {
        auto &entity = filterList[index.row()];
        auto id = hexToUINT(value.toString(), &ok);
        if (ok) {
            if (entity.idEnd != id) {
                entity.idEnd = id;
                emit updateFilter();
            }
        }
        return true;
    }
    return false;
}

Qt::ItemFlags FilterTableModel::flags(const QModelIndex &index) const
{
    int column = index.column();
    if (0 == column || 1 == column) {
        return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
    }
    return QAbstractTableModel::flags(index);
}

bool FilterTableModel::filterCanObj(const CAN_OBJ &canObj)
{
    bool allUnchecked = false;
    UINT id = canObj.ID;

    for (int i = 0; i < filterList.size(); i++) {
        auto &entity = filterList.at(i);
        if (entity.enabled) {
            allUnchecked = true;
            if (id >= entity.idStart && id <= entity.idEnd) {
                return true;
            }
        }
    }

    return !allUnchecked;
}
