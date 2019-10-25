#ifndef SIGNALWIDGET_H
#define SIGNALWIDGET_H

#include "base/titletableviewwidget.h"
#include "send/signaltablemodel.h"
#include "can/UsbCanUtil.h"

class SignalWidget : public TitleTableViewWidget
{
    Q_OBJECT
public:
    explicit SignalWidget(QWidget *parent = nullptr);

    void initModel();
    void setAllButtons();
    void setRowButtons(int row);
    void setAllPhyDelegate();

signals:
    void updateMessageData(Message *msg);

public slots:
    void setMessage(Message *msg, CAN_OBJ *obj);
    void onIncButton();
    void onDecButton();
    void modelDataChanged(QModelIndex, QModelIndex);

protected:
    SignalTableModel *model;
};

#endif // SIGNALWIDGET_H
