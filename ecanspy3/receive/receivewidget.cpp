#include "receivewidget.h"

#include <QVBoxLayout>
#include <QDebug>

ReceiveWidget::ReceiveWidget(QWidget *parent)
    : QWidget(parent), filterWidget(NULL), receiveMessageWidget(NULL)
{
    QSplitter *splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Horizontal);

    initFilterWidget(*splitter);
    initReceiveMessageWidget(*splitter);
    initDispatchSignal();

    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 4);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(splitter);
    layout->setMargin(0);
    setLayout(layout);
}

void ReceiveWidget::initFilterWidget(QSplitter &splitter)
{
    filterWidget = new FilterWidget(this);
    splitter.addWidget(filterWidget);
}

void ReceiveWidget::initReceiveMessageWidget(QSplitter &splitter)
{
    receiveMessageWidget = new ReceiveMessageWidget(this);
    splitter.addWidget(receiveMessageWidget);

    receiveMessageWidget->setFilterList(filterWidget->getFilterList());
    receiveMessageWidget->setCanObjFilter(filterWidget->getCanObjFilter());
}

void ReceiveWidget::initDispatchSignal()
{
    connect(filterWidget->getFilterTableModel(), SIGNAL(updateFilter()), receiveMessageWidget,
            SLOT(updateFilter()));
}

ReceiveMessageWidget *ReceiveWidget::getReceiveMessageWidget()
{
    return receiveMessageWidget;
}

FilterWidget *ReceiveWidget::getFilterWidget()
{
    return filterWidget;
}
