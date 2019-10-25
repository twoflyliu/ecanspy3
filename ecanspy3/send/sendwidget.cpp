#include "sendwidget.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>

SendWidget::SendWidget(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout;

    QSplitter *splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Horizontal);

    initMessageTableView(splitter);
    initSignalTableView(splitter);

    layout->addWidget(splitter);
    setLayout(layout);

    defaultMaxHeight = maximumHeight();
    setMaximumHeight(250);  //设置它，主要为了形成默认的布局
    setMinimumHeight(200);
}

void SendWidget::initMessageTableView(QSplitter *splitter)
{
    sendMessageWidget = new SendMessageWidget(this);
    splitter->addWidget(sendMessageWidget);
}

void SendWidget::initSignalTableView(QSplitter *splitter)
{
    signalWidget = new SignalWidget(this);
    splitter->addWidget(signalWidget);
}

SendMessageWidget *SendWidget::getSendMessageWidget()
{
    return sendMessageWidget;
}

SignalWidget *SendWidget::getSignalWidget()
{
    return signalWidget;
}

void SendWidget::resetSizePolicy()
{
    setMaximumHeight(defaultMaxHeight);
    sendMessageWidget->resetSizePolicy();
}
