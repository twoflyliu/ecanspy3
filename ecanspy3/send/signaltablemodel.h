#ifndef SIGNALTABLEMODEL_H
#define SIGNALTABLEMODEL_H

#include <QList>
#include <QPair>
#include <QPushButton>

#include <QAbstractTableModel>
#include <dbc4cpp/parser.h>
#include "UsbCanUtil.h"


class SignalTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum {COLUMN_COUNT=7};

    SignalTableModel(QObject *parent = nullptr);

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;

    void incStep(int row);
    void decStep(int row);

    double getPhysicalValue(int row) const;
    void setPhysicalValue(int row, double value);
    QString getPhysicalColumnValue(int row) const;

    quint64 getRawValue(int row) const;
    void setRawValue(int row, quint64 value);

    Signal* getSignal(int row) const
    {
        return signalAt(row);
    }

    void setMessage(Message *msg, CAN_OBJ *obj);
    Message *getMessage()
    {
        return this->msg;
    }

protected:
    Signal *signalAt(int index) const;
    void setMores();

protected:
    Message *msg;
    CAN_OBJ *obj;
    static const QString columnTitles[COLUMN_COUNT];

    struct MoreInfo {
        double step = 1.0;
    };

    QList<MoreInfo> mores;
};

#endif // SIGNALTABLEMODEL_H
