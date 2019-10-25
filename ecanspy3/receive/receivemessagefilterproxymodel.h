#ifndef RECEIVEMESSAGEFILTERPROXYMODEL_H
#define RECEIVEMESSAGEFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QList>
#include "filtertablemodel.h"

class ReceiveMessageFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    ReceiveMessageFilterProxyModel(QObject *parent=nullptr);

    void setFilterList(QList<FilterTableModel::Entity> * filterList);
    void refilter();

    // QSortFilterProxyModel interface
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

private:
    QList<FilterTableModel::Entity> *filterList;
};

#endif // RECEIVEMESSAGEFILTERPROXYMODEL_H
