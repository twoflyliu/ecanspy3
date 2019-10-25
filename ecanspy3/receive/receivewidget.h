#ifndef RECEIVEWIDGET_H
#define RECEIVEWIDGET_H

#include <QWidget>
#include <QSplitter>

#include "filterwidget.h"
#include "receivemessagewidget.h"

class ReceiveWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ReceiveWidget(QWidget *parent = nullptr);

    void initFilterWidget(QSplitter &splitter);
    void initReceiveMessageWidget(QSplitter &splitter);
    void initDispatchSignal();

    ReceiveMessageWidget *getReceiveMessageWidget();
    FilterWidget *getFilterWidget();

signals:

public slots:

protected:
    FilterWidget *filterWidget;
    ReceiveMessageWidget *receiveMessageWidget;
};

#endif // RECEIVEWIDGET_H
