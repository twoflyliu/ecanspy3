#ifndef DBCMESSAGETABLEMODEL_H
#define DBCMESSAGETABLEMODEL_H

#include <QAbstractTableModel>
#include <QString>

#include <dbc4cpp/parser.h>

#define SEND_MESSAGE_UNIQUE_ENABLED     (1)

class DBCTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:

    enum {
        COLUMN_COUNT = 6
    };
    DBCTableModel(QObject *parent = 0);
    ~DBCTableModel();

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool removeRows(int row, int count, const QModelIndex &parent);
    bool removeRows(const QModelIndexList &list);

    const QList<Message*> &getMsgList() const;

    void setMsgList(const QList<Message*> &lst, Document *doc);
    void setMsgIdList(QList<int> msgIdList, Document *doc);

    void clear();
    void setDocument(Document *doc);


    void addMsg(Message *msg);
    Message *getMsg(qint32 index) const;

protected:
    Document *doc;
    QList<Message*> msgList;
    static QString columnTitle[COLUMN_COUNT];
};

#endif // DBCMESSAGETABLEMODEL_H
