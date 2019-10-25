#include "receive/receivemessagetreemodel.h"
#include "utils/signalaccessor.h"

#include <QDebug>
#include <algorithm>
#include <sys/time.h>
#include <QThread>

#define PARSE_SIGNAL_ENABLED (1)

class RunTimeMonitor
{
public:
    RunTimeMonitor(QString name)
    {
        this->name = name;
        gettimeofday(&start, NULL);
    }

    ~RunTimeMonitor()
    {
        struct timeval end;
        gettimeofday(&end, NULL);

        qDebug() << "[" << this->name << "]: elapsed" << (1000000*(end.tv_sec-start.tv_sec) + end.tv_usec-start.tv_usec) << " us";
    }

protected:
    QString name;
    QTime time;
    struct timeval start;
};

#define RUNTIME_MONITOR()    \
    RunTimeMonitor rtm_##__LINE__(__FUNCTION__); \
    Q_UNUSED(rtm_##__LINE__)


enum { COLUMN_SERIAL_NUM=0, COLUMN_PERIOD_MS, COLUMN_INDEX_ID, COLUMN_INDEX_NAME, COLUMN_INDEX_LEN,
       COLUMN_INDEX_DATA, COLUMN_FRAME_TYPE, COLUMN_FRAME_FMT, COLUMN_INDEX_ABS_TIME};

QString ReceiveMessageTreeModel::columnTitles[ReceiveMessageTreeModel::COLUMN_COUNT] =
{
    QObject::tr("Seria"), QObject::tr("Period(ms)"), QObject::tr("ID   "), QObject::tr("Name"), QObject::tr("DLC"),
    QObject::tr("Data"), QObject::tr("Frame Type"), QObject::tr("Frame Fmt"), QObject::tr("Abs Time")
};

ReceiveMessageTreeModel::ReceiveMessageTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    initRefreshTimer();
}


QModelIndex ReceiveMessageTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    //RUNTIME_MONITOR();
    if (row < 0 || column < 0) {
        return QModelIndex();
    }

    if (!parent.isValid()) {                                             //第一层数据
        if (row >= mostTopRowCount()) return QModelIndex();
        return createIndex(row, column, row);                            //报文，内部数据是其索引位置
    }  else {
        int parentRow = parent.internalId();
        if (parentRow >= 0) {                                            //父亲是报文
            return createIndex(row, column, encodeSignal(parentRow));    //这儿将将父亲和信号类型编码起来
        } else {
            return QModelIndex();                                        //信号没有孩子
        }
    }
}

QModelIndex ReceiveMessageTreeModel::parent(const QModelIndex &child) const
{
    if (!child.isValid()) return QModelIndex();
    int level = child.internalId();
    if (level >= 0) {                                               //这儿是报文
        return QModelIndex();
    } else {                                                        //这儿是信号
        int parentRow = decodeSignal(child.internalId());
        Q_ASSERT(parentRow >= 0);
        return createIndex(parentRow, 0, parentRow);
    }
}

int ReceiveMessageTreeModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return mostTopRowCount();
    } else {
        int internalData = parent.internalId();
        if (internalData >= 0) {                                            //这儿是报文
            //当停止接收的时候或者当前不是滚动更新模式时候，显示信号，否则不显示
            return (stopReceiveFlag||!scrollUpdateFlag) ? signalCount(internalData) : 0;
        } else {                                                            //这儿是信号
            return 0;
        }
    }
}

int ReceiveMessageTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return COLUMN_COUNT;
}

QVariant ReceiveMessageTreeModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole) return QVariant();
    if (!index.isValid()) return QVariant();

    int internalData = index.internalId();

    if (internalData >= 0) {
        return messageData(index, role);
    } else {
        return signalData(index, role);
    }
    return QVariant();
}

QVariant ReceiveMessageTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (Qt::DisplayRole != role) return QVariant();

    if (Qt::Horizontal == orientation) {
        return columnTitles[section];
    } else {
        return QString("%1").arg(section+1, 8, 10, QChar('0'));
    }
}

Signal *ReceiveMessageTreeModel::getSignal(int msgId, int index) const
{
    if (doc) {
        Message *msg = doc->getMessage(msgId);
        if (msg) {
            auto &signalList = msg->getSignalList();
            if (index < signalList.size()) {
                return (signalList.begin()+index).value();
            }
        }
    }
    return NULL;
}

void ReceiveMessageTreeModel::setDocument(Document *doc)
{
    this->doc = doc;
}

ReceiveMessageTreeModel::Entity* ReceiveMessageTreeModel::addCanObj(CAN_OBJ &obj)
{
    //RUNTIME_MONITOR();
    if (stopReceiveFlag) return NULL;
    if (filter) {
        if (!filter->filterCanObj(obj)) return NULL;
    }

    int rowCnt = rowCount((QModelIndex()));

    auto &pair = msgTimeStampMap[obj.ID];
    pair.first = pair.second;                       //first存放老的时间
    pair.second = obj.TimeStamp;                    //second存放新的时间

    Entity entity;
    entity.obj = obj;
    entity.periodUs = pair.second - pair.first;        //为了减少内存占用，这儿使用SendType来存放当前报文周期, 单位微秒
    entity.serialCount = ++serialCount;

    newestObjMap.insert(obj.ID, entity);
    objList.push_back(entity);

    if (MAX_MESSAGE_COUNT != MAX_MESSAGE_COUNT_UNRESTRICTED && rowCnt >= MAX_MESSAGE_COUNT) {
        objList.pop_front();
        updateMode = UPDATE_MODE_UPDATE;
    } else {
        updateMode = UPDATE_MODE_INSERT;
    }

    return &objList.back();
}

void ReceiveMessageTreeModel::stopReceive()
{
    stopReceiveFlag = true;
    forceUpdate();
}

void ReceiveMessageTreeModel::startReceive()
{
    stopReceiveFlag = false;
    forceUpdate();
}

int ReceiveMessageTreeModel::signalCount(int row) const
{
    if (row <= mostTopRowCount() && doc != NULL) {
        Message *msg = doc->getMessage(entity(row).obj.ID);
        if (msg) {
            return msg->getSignalList().size();
        }
    }
    return 0;
}

QString ReceiveMessageTreeModel::msgName(int msgID) const
{
    auto iter = msgNameMap.find(msgID);
    if (iter != msgNameMap.end()) {
        return iter.value();
    } else {
        if (!doc) return QString("");
        Message *msg = doc->getMessage(msgID);
        if (msg) {
            const_cast<ReceiveMessageTreeModel*>(this)->msgNameMap.insert(msgID, msg->getName());
            return msg->getName();
        }
    }
    return QString("");
}

int ReceiveMessageTreeModel::mostTopRowCount() const
{
    if (scrollUpdateFlag) {
        return objList.size();
    } else {
        return newestObjMap.size();
    }
}

const ReceiveMessageTreeModel::Entity &ReceiveMessageTreeModel::entity(int row) const
{
    if (scrollUpdateFlag) {
        return objList[row];
    } else {
        return (newestObjMap.begin() + row).value();
    }
}

void ReceiveMessageTreeModel::refresh()
{
    RUNTIME_MONITOR();

    if (scrollUpdateFlag) {
        if (updateMode != UPDATE_MODE_NONE) {
            if (UPDATE_MODE_INSERT == updateMode) {
                int newSize = objList.size();
                if ((int)oldSize > newSize - 1) return;
                beginInsertRows(QModelIndex(), oldSize, newSize - 1);
                endInsertRows();
                oldSize = newSize;

                if (this->treeView) {
                    this->treeView->scrollTo(index(newSize-1, 0));
                }
            } else if (UPDATE_MODE_UPDATE == updateMode){
                emit dataChanged(index(0, 0), index(objList.size()-1, COLUMN_COUNT-1));
            }

            updateMode = UPDATE_MODE_NONE;
        }
    } else { //因为报文数目不多，所以这儿直接重置模型, 这儿强制更新所有数据
        if (updateMode != UPDATE_MODE_NONE) {
            updateMode = UPDATE_MODE_NONE;

            static UINT oldCount = 0;
            if ((int)oldCount != newestObjMap.size()) {
                oldCount = newestObjMap.size();
                beginResetModel();                  //更新所有数据
                endResetModel();
            } else {
                emit dataChanged(index(0, 0), index(mostTopRowCount()-1, COLUMN_COUNT-1)); //所有数据都变化了
                //emit dataChanged(index(0, 0), index(0, 0)); //所有数据都变化了
            }

        }
    }
}

void ReceiveMessageTreeModel::forceUpdate()
{
    beginResetModel();
    endResetModel();
}

void ReceiveMessageTreeModel::clear()
{
    qDebug() << "ReceiveMessageTreeModel::clear()";
    beginResetModel();
    objList.clear();
    newestObjMap.clear();
    serialCount = 0;
    oldSize = 0;
    endResetModel();
}

void ReceiveMessageTreeModel::setScroll(bool scroll)
{
    if (scrollUpdateFlag != scroll) {
        beginResetModel();
        scrollUpdateFlag = scroll;
        oldSize = 0;
        endResetModel();
    }
}

QVariant ReceiveMessageTreeModel::messageData(const QModelIndex &index, int role) const
{
    //RUNTIME_MONITOR();
    Q_UNUSED(role);
    auto row = index.row();
    auto column = index.column();

    if (row >= mostTopRowCount()) return QVariant();

    auto &ent = entity(index.row());
    return messageColumnData(ent, column, role);
}

QVariant ReceiveMessageTreeModel::messageColumnData(const ReceiveMessageTreeModel::Entity &ent, int column, int role) const
{
    Q_UNUSED(role);
    auto &obj = ent.obj;
    if (COLUMN_INDEX_ID == column) {                //ID
        return QString("%1").arg(obj.ID, 0, 16).toUpper();
    } else if (COLUMN_INDEX_NAME == column) {       //NAME
        return msgName(obj.ID);
    } else if (COLUMN_INDEX_LEN == column) {        //LEN
        return QString("%1").arg(obj.DataLen);
    } else if (COLUMN_INDEX_DATA == column) {       //DATA
        QString msgData;
#if 1
        for (int i = 0; i < obj.DataLen; ++i) {
            msgData.append(QString("%1 ").arg(obj.Data[i], 2, 16, QChar('0')).toUpper());
        }
#endif
        return msgData;
    } else if (COLUMN_INDEX_ABS_TIME == column) {   //ABS Time
        return QString("%1").arg(obj.TimeStamp);
    } else if (COLUMN_FRAME_TYPE == column) {       //Frame type
        return QString("%1").arg((obj.RemoteFlag) ? tr("REMOTE") : tr("DATA "));
    } else if (COLUMN_FRAME_FMT == column) {        //Frame fmt
        return QString("%1").arg((obj.ExternFlag) ? tr("EXTENDED") : tr("STANDARD"));
    } else if (COLUMN_PERIOD_MS == column) {        //Period ms
        return QString("%1").arg(ent.periodUs/1000.0, 7, 'f', 3, QChar('0'));
    } else if (COLUMN_SERIAL_NUM == column) {
        return QString("%1").arg(ent.serialCount, 10, 10, QChar('0'));
    }
    return QVariant();
}

QVariant ReceiveMessageTreeModel::signalData(const QModelIndex &index, int role) const
{
    //RUNTIME_MONITOR();
    Q_UNUSED(role);
    int parentRow = decodeSignal(index.internalId());
    Q_ASSERT(parentRow >= 0);
    auto &obj = entity(parentRow).obj;
    Signal *signal = getSignal(obj.ID, index.row());
    if (!signal) return QVariant();

    if (COLUMN_INDEX_NAME == index.column()) {              //NAME
        return signal->getName();
    } else if (COLUMN_INDEX_DATA == index.column()) {       //DATA
        return SignalAccessor::getSignalPhysicalValue(&obj, signal);
    } else if (COLUMN_INDEX_ID == index.column()) {
        return QString("%1").arg(obj.ID, 0, 16).toUpper();
    }
    return QVariant();
}

void ReceiveMessageTreeModel::initRefreshTimer()
{
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(refresh()));
    timer->start(DEFAULT_REFRESH_PREIOD_MS);
}
