#include "checkedbuttondelegate.h"

#include <QCheckBox>
#include <QApplication>
#include <QMouseEvent>
#include <QPushButton>

CheckedButtonDelegate::CheckedButtonDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}


void CheckedButtonDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem viewOption(option);
    initStyleOption(&viewOption, index);
    if (option.state.testFlag(QStyle::State_HasFocus)) {
        viewOption.state = viewOption.state ^ QStyle::State_HasFocus;
    }

    if (index.column() == checkedButtonColumn) {
        bool data = index.model()->data(index, Qt::UserRole).toBool();

        QStyleOptionButton checkedButtonStyle;
        checkedButtonStyle.state = data ? QStyle::State_On : QStyle::State_Off;
        checkedButtonStyle.iconSize = QSize(20, 20);
        checkedButtonStyle.rect = option.rect;

        QPushButton pushButton;
        pushButton.setCheckable(true);
        pushButton.setChecked(data);

        QApplication::style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &checkedButtonStyle,                                             painter, &pushButton);
        return;
    }

     QStyledItemDelegate::paint(painter, option, index);
}

bool CheckedButtonDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    QRect decorationRect = option.rect;
    QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
    if (event->type() == QEvent::MouseButtonPress && decorationRect.contains(mouseEvent->pos())) {
        if (index.column() == checkedButtonColumn) {
            bool data = model->data(index, Qt::UserRole).toBool();
            model->setData(index, !data, Qt::UserRole);
            emit checkedButtonClicked(index.row(), checkedButtonColumn);
        }
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}
