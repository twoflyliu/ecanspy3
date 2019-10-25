#ifndef COMBOBOXDELEGATE_H
#define COMBOBOXDELEGATE_H

#include <QItemDelegate>
#include <dbc4cpp/parser.h>

class SignalTableViewPhysicalValueDelegate : public QItemDelegate
{
public:
    SignalTableViewPhysicalValueDelegate(QObject *parent=nullptr);

    // QAbstractItemDelegate interface
public:
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

protected:
    QWidget *createComboBoxEditor(QWidget *parent, Signal *signal) const;
    QWidget *createDoubleSpinBoxEditor(QWidget *parent, Signal *signal) const;
    int calucSignalFactor(Signal *signal) const;
};

#endif // COMBOBOXDELEGATE_H
