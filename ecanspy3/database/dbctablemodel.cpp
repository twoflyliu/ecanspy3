#include "dbctablemodel.h"

#include <algorithm>
#include <vector>
#include <functional>

QString DBCTableModel::columnTitle[DBCTableModel::COLUMN_COUNT] = {"Name", "Type", "ID", "Node", "Period", "Len"};

DBCTableModel::DBCTableModel(QObject *parent)
    : QAbstractTableModel(parent), doc(NULL)
{

}

DBCTableModel::~DBCTableModel()
{
}


int DBCTableModel::rowCount(const QModelIndex &parent) const
{
    (void)parent;
    return msgList.size();
}

int DBCTableModel::columnCount(const QModelIndex &parent) const
{
    (void)parent;
    return COLUMN_COUNT;
}

QVariant DBCTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    if (NULL == doc) {
        return QVariant();
    }
    if (index.row() >= msgList.size()) {
        return QVariant();
    }
    if (index.column() >= COLUMN_COUNT) {
        return QVariant();
    }
    if (Qt::DisplayRole != role) {
        return QVariant();
    }

    auto col = index.column();
    auto msg = msgList.at(index.row());
    if (0 == col) {                                             //报文名称
        return msg->getName();
    } else if (1 == col) {                                      //帧类型
        return (msg->getId() <= 0x7ff) ? "Std" : "Extend";
    } else if (2 == col) {                                      //报文ID
        return QString("%1").arg(msg->getId(), 0, 16).toUpper();
    } else if (3 == col) {                                      //发送节点
        return msg->getTransmitter();
    } else if (4 == col) {                                      //报文周期
        const AdditionalAttribute* attr = doc->getMsgAttribute(msg->getId(), "GenMsgCycleTime");
        if (attr) {
            return QString("%1").arg(attr->getIntValue());
        } else {
            return QString("0");
        }
    } else if (5 == col) {                                      //报文长度
        return QString("%1").arg(msg->getByteLen());
    }
    return QVariant();
}

QVariant DBCTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    if (Qt::Horizontal == orientation) {
        return columnTitle[section];
    } else {
        return QString("%1 ").arg(section+1);
    }
}

Qt::ItemFlags DBCTableModel::flags(const QModelIndex &index) const
{
    return QAbstractTableModel::flags(index);
}

const QList<Message *> &DBCTableModel::getMsgList() const
{
    return this->msgList;
}

void DBCTableModel::setMsgList(const QList<Message *> &lst, Document *doc)
{
    beginResetModel();
    this->msgList = lst;
    this->doc = doc;
    endResetModel();
}

void DBCTableModel::setMsgIdList(QList<int> msgIdList, Document *doc)
{
    Q_ASSERT(doc);

    beginResetModel();

    this->msgList.clear();
    foreach (int msgId, msgIdList) {
        Message *msg = doc->getMessage(msgId);
        if (msg) {
            this->msgList.append(msg);
        }
    }

    this->doc = doc;
    endResetModel();
}

void DBCTableModel::clear()
{
    setMsgList(QList<Message*>(), NULL);
}

void DBCTableModel::setDocument(Document *doc)
{
    this->doc = doc;
}

void DBCTableModel::addMsg(Message *msg)
{
#if SEND_MESSAGE_UNIQUE_ENABLED
    if (msgList.indexOf(msg) == -1) {
        beginInsertRows(QModelIndex(), msgList.size(), msgList.size());
        this->msgList.push_back(msg);
        endInsertRows();
    }
#else
    beginInsertRows(QModelIndex(), msgList.size(), msgList.size());
    this->msgList.push_back(msg);
    endInsertRows();
#endif //SEND_MESSAGE_UNIQUE_ENABLED
}

Message *DBCTableModel::getMsg(qint32 index) const
{
    if (index < this->msgList.size()) {
        return this->msgList.at(index);
    }
    return NULL;
}

bool DBCTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    for (int i = 0; i < count; i++) {
        this->msgList.removeAt(row);
    }
    endRemoveRows();
    return true;
}

bool DBCTableModel::removeRows(const QModelIndexList &list)
{
    std::vector<int> rowCntList;
    for (auto iter = list.begin(); iter != list.end(); ++iter) {
        rowCntList.push_back(iter->row());
    }

    std::sort(rowCntList.begin(), rowCntList.end(), std::greater<int>());

    for (auto iter = rowCntList.begin(); iter != rowCntList.end(); ++iter) {
        removeRows(*iter, 1, QModelIndex());
    }
    return true;
}
