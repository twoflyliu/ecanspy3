#ifndef CHECKEDBUTTONDELEGATE_H
#define CHECKEDBUTTONDELEGATE_H

#include <QStyledItemDelegate>

class CheckedButtonDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit CheckedButtonDelegate(QObject *parent = nullptr);

    void setCheckedButotnColumn(int column)
    {
        checkedButtonColumn = column;
    }

signals:
    void checkedButtonClicked(int row, int col);

public slots:

    // QAbstractItemDelegate interface
public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

private:
    int checkedButtonColumn = -1;
};

#endif // CHECKEDBUTTONDELEGATE_H
