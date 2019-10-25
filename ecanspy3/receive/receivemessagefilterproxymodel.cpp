#include "receivemessagefilterproxymodel.h"

ReceiveMessageFilterProxyModel::ReceiveMessageFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{

}

void ReceiveMessageFilterProxyModel::setFilterList(QList<FilterTableModel::Entity> *filterList)
{
    this->filterList = filterList;
    invalidateFilter();
}

void ReceiveMessageFilterProxyModel::refilter()
{
    invalidateFilter();
}


bool ReceiveMessageFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (!filterList)    return true;

    QModelIndex idIndex = sourceModel()->index(source_row, 1, source_parent);

    bool allUnchecked = false;  //如果所有行都没有选中，则说明不需要过滤，如果有一行都选中，则说明需要过滤
    QString idStr = sourceModel()->data(idIndex).toString();
    QByteArray byteArray(idStr.toAscii());
    quint32 id = byteArray.toUInt(NULL, 16);

    for (int i = 0; i < this->filterList->size(); i++) {
        auto &entity = this->filterList->at(i);
        if (entity.enabled) {
            allUnchecked = true;
            if (id >= entity.idStart && id <= entity.idEnd) {
                return true;
            }
        }
    }

    return !allUnchecked;
}
