#ifndef SENDWIDGET_H
#define SENDWIDGET_H

#include <QWidget>
#include <QSplitter>
#include <QTableView>

#include "sendmessagewidget.h"
#include "signalwidget.h"

class SendWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SendWidget(QWidget *parent = nullptr);

    void initMessageTableView(QSplitter *splitter);
    void initSignalTableView(QSplitter *splitter);

    SendMessageWidget *getSendMessageWidget();
    SignalWidget *getSignalWidget();

    void resetSizePolicy();

signals:

public slots:

private:
    SendMessageWidget *sendMessageWidget;
    SignalWidget *signalWidget;

    int defaultMaxHeight;
};

#endif // SENDWIDGET_H
