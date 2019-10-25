#ifndef SENDMESSAGEWIDGET_H
#define SENDMESSAGEWIDGET_H

#include "base/titletableviewwidget.h"
#include "send/txmessagetablemodel.h"
#include "send/checkedbuttondelegate.h"

#include <dbc4cpp/parser.h>
#include <QPushButton>
#include <QList>
#include <QComboBox>
#include <QLineEdit>
#include <QSortFilterProxyModel>

#define SEND_MESSAGE_TABLE_VIEW_FILTER_ENABLED     1

class SendMessageWidget : public TitleTableViewWidget
{
    Q_OBJECT
public:
    explicit SendMessageWidget(QWidget *parent = nullptr);
    ~SendMessageWidget();

    void resetSizePolicy();

public slots:
    void resetContent(Document *doc, QList<Message*> msgList);
    void sendButtonClicked(int row, int col);
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);
    void updateMessageData(Message *msg);

    void onToggleAllCyclicButton(bool checked);

#if SEND_MESSAGE_TABLE_VIEW_FILTER_ENABLED
    void filterTextChanged(QString text);
    void filterColumnChanged(int index);
#endif

signals:
    void refreshCurrMessage(Message *msg, CAN_OBJ *obj);

protected:
    void initTitle();
    void initModel();
    void initTableView();
    void initCan();

    void initToolButtons();

#if SEND_MESSAGE_TABLE_VIEW_FILTER_ENABLED
    void initFilter(QHBoxLayout *toolButtonsLayout);
#endif

private:
    void enableSendMessage(int msgId, bool enabled);
    int maxWidth = -1;

protected:
    TxMessageTableModel *model = nullptr;

    QPushButton *enableAllCyclicButton;

#if SEND_MESSAGE_TABLE_VIEW_FILTER_ENABLED
    QComboBox *filterColumnCombo = nullptr;
    QLineEdit *filterTextEdit = nullptr;
    QSortFilterProxyModel *proxyModel = nullptr;
#endif
    CheckedButtonDelegate *checkedItemDelegate;

    QPushButton *resizeColumnsButton;
};

#endif // SENDMESSAGEWIDGET_H
