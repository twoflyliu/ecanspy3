#ifndef FILTERTABLEMODEL_H
#define FILTERTABLEMODEL_H

#include <QAbstractTableModel>
#include <QStringList>
#include "UsbCanUtil.h"

struct CanObjFilter
{
    virtual ~CanObjFilter() {}
    virtual bool filterCanObj(const CAN_OBJ &canObj) =  0;
};

class FilterTableModel : public QAbstractTableModel, public CanObjFilter
{
    Q_OBJECT
public:
    struct Entity {
        UINT idStart;
        UINT idEnd;
        bool enabled;
        Entity() : idStart(0), idEnd(0), enabled(false) {}
    };

    enum {COLUMN_COUNT=3};

    FilterTableModel(QObject *parent=nullptr);

    void setChecked(int row, bool enabled);
    QList<Entity> *getFilterList();

signals:
    void updateFilter();

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool insertRows(int row, int count, const QModelIndex &parent);
    bool removeRows(int row, int count, const QModelIndex &parent);
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;

    CanObjFilter* getCanObjFilter()
    {
        return this;
    }

    bool filterCanObj(const CAN_OBJ &canObj);

    void clear()
    {
        beginResetModel();
        filterList.clear();
        endResetModel();
    }

    void setFilterEntList(QList<Entity> filterList)
    {
        beginResetModel();
        this->filterList = filterList;
        endResetModel();
    }

protected:    
    QList<Entity> filterList;
    static QString columnTitles[COLUMN_COUNT];
};

#endif // FILTERTABLEMODEL_H
