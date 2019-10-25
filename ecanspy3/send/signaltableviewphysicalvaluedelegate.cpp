#include "signaltableviewphysicalvaluedelegate.h"

#include <QComboBox>
#include <QDoubleSpinBox>

#include "signaltablemodel.h"

SignalTableViewPhysicalValueDelegate::SignalTableViewPhysicalValueDelegate(QObject *parent) : QItemDelegate(parent)
{

}


QWidget *SignalTableViewPhysicalValueDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);

    const SignalTableModel *model = static_cast<const SignalTableModel *>(index.model());
    auto signal = model->getSignal(index.row());
    if (NULL == signal) return NULL;

    auto &tbl = signal->getValueDescTable();
    if (tbl.size() != 0 && signal->getUnit().isEmpty()) {
        return createComboBoxEditor(parent, signal);
    } else {
        return createDoubleSpinBoxEditor(parent, signal);
    }
}

//Model -> Editor
void SignalTableViewPhysicalValueDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString value = index.model()->data(index, Qt::EditRole).toString();
    QComboBox *comboBox = qobject_cast<QComboBox*>(editor);
    if (comboBox) {
        for (int i = 0; i < comboBox->count(); i++) {
            if (value == comboBox->itemText(i)) {
                comboBox->setCurrentIndex(i);
                break;
            }
        }
    } else {
        QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(editor);
        if (spinBox) {
            spinBox->setValue(value.toDouble());
        }
    }
}

// Editor -> Model
void SignalTableViewPhysicalValueDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
{
    QComboBox *comboBox = qobject_cast<QComboBox*>(editor);
    if (comboBox) {
        auto curIndex = comboBox->currentIndex();
        if (curIndex != -1) {
            model->setData(index, curIndex, Qt::EditRole);
        }
    } else {
        QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(editor);
        spinBox->interpretText();
        model->setData(index, spinBox->value(), Qt::EditRole);
    }
}

void SignalTableViewPhysicalValueDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(editor);
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}

QWidget *SignalTableViewPhysicalValueDelegate::createComboBoxEditor(QWidget *parent, Signal *signal) const
{
    auto &tbl = signal->getValueDescTable();
    QComboBox *editor = new QComboBox(parent);
    for (auto iter = tbl.begin(); iter != tbl.end(); ++iter) {
        editor->insertItem(editor->count(), iter.value());
    }
    return editor;
}

QWidget *SignalTableViewPhysicalValueDelegate::createDoubleSpinBoxEditor(QWidget *parent, Signal *signal) const
{
    QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
    editor->setMinimum(signal->getMinimum());
    editor->setMaximum(signal->getMaximum());
    editor->setDecimals(calucSignalFactor(signal));
    return editor;
}

int SignalTableViewPhysicalValueDelegate::calucSignalFactor(Signal *signal) const
{
    QString str = QString("%1").arg(signal->getFactor());
    auto dotPos = str.indexOf(".");
    if (-1 == dotPos) {
        return 0;
    } else {
        return str.size() - dotPos - 1;
    }
}
