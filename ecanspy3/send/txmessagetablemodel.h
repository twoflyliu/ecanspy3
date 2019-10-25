#ifndef TXMESSAGETABLEMODEL_H
#define TXMESSAGETABLEMODEL_H

#include <QAbstractTableModel>
#include <can/ECanVci.H>
#include <QList>
#include <QString>
#include <dbc4cpp/parser.h>

#define TX_MESSAGE_TABLE_VIEW_COLUMN_INDEX_NAME       (0)
#define TX_MESSAGE_TABLE_VIEW_TX_EN                   (1)
#define TX_MESSAGE_TABLE_VIEW_MESSAGE_TYPE            (2)
#define TX_MESSAGE_TABLE_VIEW_COLUMN_INDEX_ID         (4)

class TxMessageTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum {
        COLUMN_COUNT = 15
    };

    /*!
     * \brief MoreInfo 存放报文附加信息
     */
    struct MoreInfo{
        QString name;
        bool tx;
        QString type;
        qint32 period;
    };

    TxMessageTableModel(Document *doc = NULL, QObject *parent=NULL);

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    void reset();
    void appendRow(CAN_OBJ &obj, const MoreInfo &more);
    void setDocument(Document *doc);

    Message *getMessage(QModelIndex index);
    CAN_OBJ *getCanObj(QModelIndex index);

    void refreshMessageData(Message *msg);

    void beginResetModel()
    {
        QAbstractTableModel::beginResetModel();
    }

    void endResetModel()
    {
        QAbstractTableModel::endResetModel();
    }

    QList<CAN_OBJ*> objs;            //!< 存放要发送的数据信号
    QList<MoreInfo> mores;          //!< 存放附加信息

    Document *doc;
private:
    static QString columnTitles[COLUMN_COUNT];

    // QAbstractItemModel interface
public:


};

#endif // TXMESSAGETABLEMODEL_H
