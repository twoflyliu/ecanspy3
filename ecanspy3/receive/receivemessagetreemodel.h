#ifndef RECEIVEMESSAGETABLEMODEL_H
#define RECEIVEMESSAGETABLEMODEL_H

#include <QAbstractItemModel>
#include <dbc4cpp/parser.h>

#include "UsbCanUtil.h"
#include "filtertablemodel.h"

#include <QList>
#include <QTime>
#include <QMap>
#include <QPair>
#include <deque>
#include <QTimer>
#include <QTreeView>



class ReceiveMessageTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum {
        COLUMN_COUNT = 9,

        MAX_MESSAGE_COUNT_UNRESTRICTED = -1,                            //!< 内存没有限制
        MAX_MESSAGE_COUNT = 350000,             //!< 内存中最多只存5000行

        DEFAULT_REFRESH_PREIOD_MS = 250,                                //!< 默认刷新周期，单位：毫秒
        SIGNAL_LEVEL = -1,

        DEFAULT_SCROLL_UPDATE = 0,                                      //!< 默认的滚动更新，1表示滚动更新，0表示不进行滚动更新
    };

    enum UpdateMode {
        UPDATE_MODE_NONE = 0,
        UPDATE_MODE_INSERT,                                         //插入若干数据
        UPDATE_MODE_UPDATE                                              //数据更新，长度未变
    };

    struct Entity {
        CAN_OBJ obj;
        DWORD periodUs;
        DWORD serialCount;
    };

    ReceiveMessageTreeModel(QObject *parent=NULL);

    // QAbstractItemModel interface
public:
    QModelIndex index(int row, int column, const QModelIndex &parent=QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    QVariant messageColumnData(const Entity &entity, int column, int role) const;

    Signal *getSignal(int msgId, int index) const;

    void setDocument(Document *doc);
    Entity* addCanObj(CAN_OBJ &obj);

    void stopReceive();
    void startReceive();

    int signalCount(int msgRow) const;
    QString msgName(int msgID) const;

    void setTreeView(QTreeView *treeView)
    {
        this->treeView = treeView;
    } 

    void setCanObjFilter(CanObjFilter *filter)
    {
        this->filter = filter;
    }

    const std::deque<Entity>& getObjList() const
    {
        return objList;
    }

    bool isScrollUpdate() const
    {
        return this->scrollUpdateFlag;
    }

private:
    int mostTopRowCount() const;
    const Entity &entity(int row) const;

public slots:
    void refresh();
    void forceUpdate();
    void clear();
    void setScroll(bool scroll);

protected:
    QVariant messageData(const QModelIndex &index, int role) const;

    QVariant signalData(const QModelIndex &index, int role) const;

    void initRefreshTimer();

protected:
    Document *doc = nullptr;
    static QString columnTitles[COLUMN_COUNT];

    std::deque<Entity> objList;
    QMap<UINT, Entity> newestObjMap;

    QMap<UINT, QPair<UINT, UINT> > msgTimeStampMap;
    QMap<UINT, QString> msgNameMap;                 //内部缓存，报文id和报文名称关系

    QTimer *timer;                                  //刷新数据定时器
    UpdateMode updateMode = UPDATE_MODE_NONE;       //数据变化标识
    bool stopReceiveFlag = false;
    bool scrollUpdateFlag = DEFAULT_SCROLL_UPDATE;

    std::size_t oldSize = 0;

    QTreeView *treeView = nullptr;
    UINT serialCount = 0;

    CanObjFilter* filter = nullptr;

    inline int encodeSignal(int rowParent) const
    {
        return SIGNAL_LEVEL - rowParent;
    }

    inline int decodeSignal(int internData) const
    {
        return SIGNAL_LEVEL - internData;
    }

};

#endif // RECEIVEMESSAGETABLEMODEL_H
