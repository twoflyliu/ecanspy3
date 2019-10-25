#include "send/signaltablemodel.h"
#include "utils/signalaccessor.h"

const QString SignalTableModel::columnTitles[SignalTableModel::COLUMN_COUNT] = {
    "Description", "In", "Dc", "Step", "Physical Value", "Physical Unit", "Raw Value(Hex)"
};

SignalTableModel::SignalTableModel(QObject *parent)
    : QAbstractTableModel(parent), msg(NULL), obj(NULL)
{

}


int SignalTableModel::rowCount(const QModelIndex &parent) const
{
    (void)parent;
    if (!msg) return 0;
    int size = msg->getSignalList().size();
    return size;
}

int SignalTableModel::columnCount(const QModelIndex &parent) const
{
    (void)parent;
    return COLUMN_COUNT;
}

QVariant SignalTableModel::data(const QModelIndex &index, int role) const
{
    if (!msg || !index.isValid() || (Qt::DisplayRole != role && Qt::EditRole != role)) return QVariant();
    const Signal *signal = signalAt(index.row());
    int column = index.column();
    if (!signal) return QVariant();

    if (0 == column) {                          //信号名称
        return signal->getName();
    } else if (1 == column || 2 == column) {    //+,-按钮
        return QVariant();
    } else if (3 == column) {                   //步长
        return QString("%1").arg(mores.at(index.row()).step);
    } else if (4 == column) {                   //物理值
        return getPhysicalColumnValue(index.row());
    } else if (5 == column) {
        return signal->getUnit();
    } else if (6 == column) {                   //原始值
        return QString("%1").arg(getRawValue(index.row()), 0, 16).toUpper();
    } else {
        return QVariant();
    }
}

QVariant SignalTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (Qt::DisplayRole == role) {
        if (Qt::Horizontal == orientation) {
            return columnTitles[section];
        } else {
            return QString("%1 ").arg(section+1);
        }
    }
    return QVariant();
}

void SignalTableModel::setMessage(Message *msg, CAN_OBJ *obj)
{
    beginResetModel();
    this->msg = msg;
    this->obj = obj;
    setMores();
    endResetModel();
}

Signal *SignalTableModel::signalAt(int index) const
{
    if (!msg || index >= msg->getSignalList().size()) return NULL;
    return (msg->getSignalList().begin() + index).value();
}

void SignalTableModel::setMores()
{
    int rowCnt = rowCount();
    for (int i = 0; i < rowCnt; ++i) {
        mores.append(MoreInfo());
    }
}


bool SignalTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() && role != Qt::EditRole) {
        return false;
    }

    bool flag = false;
    bool ok = false;
    int column = index.column();

    if (3 == column) {                      //设置步长
        auto step = value.toDouble(&ok);
        if (ok) {
            mores[index.row()].step = step;
            flag = true;
        }
    } else if (6 == column) {               //物理值
        QByteArray array;
        array.append(value.toString());
        auto rawValue = array.toUInt(&ok, 16);
        if (ok) {
            setRawValue(index.row(), rawValue);
            flag = true;
        }
    } else if (4 == column) {
        switch (value.type()) {
        case QVariant::Int:                 //设置的是状态
        {
            auto idx = value.toInt(&ok);
            auto &tbl = signalAt(index.row())->getValueDescTable();
            if (ok) {
                setRawValue(index.row(), (tbl.begin()+idx).key());
            }
            break;
        }
        case QVariant::Double:              //设置的是物理值
        {
            auto phyVal = value.toDouble(&ok);
            if (ok) {
                setPhysicalValue(index.row(), phyVal);
            }
            break;
        }
        default:
            break;
        }
    }

    if (flag) {
        emit dataChanged(index, index);
    }

    return flag;
}

Qt::ItemFlags SignalTableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags oldFlags = QAbstractTableModel::flags(index);
    if (!index.isValid()) return oldFlags;

    int column = index.column();
    if (3 == column || 6 == column || 4 == column) {
        return oldFlags | Qt::ItemIsEditable;
    }

    return oldFlags;
}

void SignalTableModel::incStep(int row)
{
    if (row >= rowCount()) return;
    auto rawVal = getRawValue(row);
    rawVal += mores.at(row).step;
    setRawValue(row, rawVal);
}

void SignalTableModel::decStep(int row)
{
    if (row >= rowCount()) return;
    auto rawVal = getRawValue(row);
    rawVal -= mores.at(row).step;
    setRawValue(row, rawVal);
}

double SignalTableModel::getPhysicalValue(int row) const
{
    auto signal = signalAt(row);
    auto rawValue = SignalAccessor::getSignalValue(this->obj, signal);
    return rawValue * signal->getFactor() + signal->getOffset();
}

void SignalTableModel::setPhysicalValue(int row, double phyVal)
{
    auto signal = signalAt(row);
    double rawValue = (phyVal - signal->getOffset()) / signal->getFactor();
    if (fabs(rawValue - (int)rawValue) < 0.000001) {
        setRawValue(row, rawValue);
    } else {
        double phyValL = ((int)rawValue) * signal->getFactor() + signal->getOffset();
        double phyValR = ((int)rawValue+1) * signal->getFactor() + signal->getOffset();

        if (fabs(phyValL - phyVal) < fabs(phyValR - phyVal)) {
            setRawValue(row, (int)rawValue);
        } else {
            setRawValue(row, (int)rawValue+1);
        }
    }
}

QString SignalTableModel::getPhysicalColumnValue(int row) const
{
    auto signal = signalAt(row);
    auto rawValue = getRawValue(row);

    QString ret = signal->getValueDescTable().value(rawValue, "");
    if (ret.isEmpty()) {
        return QString("%1").arg(getPhysicalValue(row));
    } else {
        return ret.trimmed();
    }
}

quint64 SignalTableModel::getRawValue(int row) const
{
    auto signal = signalAt(row);
    return SignalAccessor::getSignalValue(this->obj, signal);
}

void SignalTableModel::setRawValue(int row, quint64 value)
{
    auto signal = signalAt(row);
    SignalAccessor::setSignalValue(this->obj, signal, value);
    emit dataChanged(index(row, 4), index(row, 6));
}
