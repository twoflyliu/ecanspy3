#include "txmessagetablemodel.h"

QString TxMessageTableModel::columnTitles[TxMessageTableModel::COLUMN_COUNT] = {
    "Description", "Tx", "AutoTx", "Rates(ms)", "ID   ", "Len", "B1 ", "B2 ", "B3 ", "B4 ", "B5 ", "B6 ", "B7 ", "B8 ", "Type"
};

TxMessageTableModel::TxMessageTableModel(Document *_doc, QObject *parent)
    : QAbstractTableModel(parent), doc(_doc)
{

}

int TxMessageTableModel::rowCount(const QModelIndex &parent) const
{
    (void)parent;
    return objs.size();
}

int TxMessageTableModel::columnCount(const QModelIndex &parent) const
{
    (void)parent;
    return COLUMN_COUNT;
}

QVariant TxMessageTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    if (NULL == doc) {
        return QVariant();
    }
    if (index.row() >= objs.size()) {
        return QVariant();
    }
    if (index.column() >= COLUMN_COUNT) {
        return QVariant();
    }
    if (!(Qt::DisplayRole == role || Qt::UserRole == role)) {
        return QVariant();
    }

    auto row = index.row();
    auto col = index.column();
    auto &obj = *objs.at(row);
    auto &more = mores.at(row);

    if (0 == col) {                                             //描述
        return more.name;
    } else if (1 == col) {                                      //发送按钮使能
        return more.tx;
    } else if (2 == col) {                                      //周期类型
        return more.type;
    } else if (3 == col) {                                      //周期
        return more.period;
    } else if (4 == col) {                                      //ID
        return QString("%1").arg(obj.ID, 0, 16).toUpper();
    } else if (5 == col) {                                      //报文长度
        return obj.DataLen;
    } else if (col >= 6 && col <= 13) {                         //数据
        return QString("%1").arg(obj.Data[col-6], 2, 16, QChar('0')).toUpper();
    } else {                                                    //帧类型
        return obj.ExternFlag ? "Ext Frame" : "Std Frame";
    }
}

QVariant TxMessageTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    if (Qt::Horizontal == orientation) {
        return columnTitles[section];
    } else {
        return QString("%1 ").arg(section+1);
    }
}

Qt::ItemFlags TxMessageTableModel::flags(const QModelIndex &index) const
{
    return QAbstractTableModel::flags(index);
}


bool TxMessageTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() >= mores.size()) return false;
    if (role == Qt::UserRole) {
        if (index.column() == TX_MESSAGE_TABLE_VIEW_TX_EN) {
            bool val = value.toBool();
            if (val != mores[index.row()].tx) {
                mores[index.row()].tx = val;
                emit dataChanged(index, index);
            }
            return true;
        }
    }
    return false;
}

void TxMessageTableModel::reset()
{
    beginResetModel();
    objs.clear();
    mores.clear();
    endResetModel();
}

void TxMessageTableModel::appendRow(CAN_OBJ &obj, const TxMessageTableModel::MoreInfo &more)
{
    objs.append(&obj);
    mores.append(more);
}

void TxMessageTableModel::setDocument(Document *doc)
{
    this->doc = doc;
}

Message *TxMessageTableModel::getMessage(QModelIndex index)
{
    if (!index.isValid()||!doc) return NULL;
    CAN_OBJ *obj = objs.at(index.row());
    return doc->getMessage(obj->ID);
}

CAN_OBJ *TxMessageTableModel::getCanObj(QModelIndex index)
{
    if (!index.isValid()||!doc) return NULL;
    CAN_OBJ *obj = const_cast<CAN_OBJ*>(objs.at(index.row()));
    return obj;
}

void TxMessageTableModel::refreshMessageData(Message *msg)
{
    for (int i = 0; i < objs.size(); i++) {
        if (objs[i]->ID == msg->getId()) {
            emit dataChanged(index(i,6), index(i,13));
        }
    }
}

