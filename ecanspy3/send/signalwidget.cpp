#include "signalwidget.h"

#include <QPushButton>
#include <QDebug>

#include "signaltableviewphysicalvaluedelegate.h"

SignalWidget::SignalWidget(QWidget *parent)
    : TitleTableViewWidget(parent)
{
    title->setText(tr("信号"));

    initModel();
}

void SignalWidget::initModel()
{
    model = new SignalTableModel(this);
    tableView->setModel(model);
    tableView->resizeColumnsToContents(); 
    connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(modelDataChanged(QModelIndex,QModelIndex)));
}

void SignalWidget::setMessage(Message *msg, CAN_OBJ *obj)
{
    model->setMessage(msg, obj);
    setAllButtons();
    setAllPhyDelegate();
    tableView->scrollToTop();
    tableView->resizeColumnsToContents();
}

void SignalWidget::onIncButton()
{
    model->incStep(sender()->property("row").toInt());
    emit updateMessageData(model->getMessage());
}

void SignalWidget::onDecButton()
{
    model->decStep(sender()->property("row").toInt());
    emit updateMessageData(model->getMessage());
}

void SignalWidget::modelDataChanged(QModelIndex leftTopIndex, QModelIndex rightBottomIndex)
{
    Q_UNUSED(leftTopIndex);
    Q_UNUSED(rightBottomIndex);
    emit updateMessageData(model->getMessage());
}

void SignalWidget::setRowButtons(int row)
{
    QPushButton *incButton = new QPushButton(tr("+"), this);
    incButton->setProperty("row", row);
    connect(incButton, SIGNAL(clicked(bool)), this, SLOT(onIncButton()));

    QPushButton *decButton = new QPushButton(tr("-"), this);
    decButton->setProperty("row", row);
    connect(decButton, SIGNAL(clicked(bool)), this, SLOT(onDecButton()));

    tableView->setIndexWidget(model->index(row, 1), incButton);
    tableView->setIndexWidget(model->index(row, 2), decButton);
}

void SignalWidget::setAllPhyDelegate()
{
    int rows = model->rowCount();
    for (int i = 0; i < rows; i++) {
        tableView->setItemDelegateForColumn(4, new SignalTableViewPhysicalValueDelegate(this));
    }
}

void SignalWidget::setAllButtons()
{
    int rows = model->rowCount();
    for (int i = 0; i < rows; i++) {
        setRowButtons(i);
    }
}
