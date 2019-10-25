#include "titletableviewwidget.h"

TitleTableViewWidget::TitleTableViewWidget(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(2, 0, 0, 0);
    initTitle(layout);
    initTableView(layout);
    setLayout(layout);
}

void TitleTableViewWidget::initTitle(QVBoxLayout *layout)
{
    title = new QLabel(this);
    //title->setStyleSheet(QString("background-color:rgb(200,200,200);padding:5;color:black;"));
    layout->addWidget(title);
}

void TitleTableViewWidget::initTableView(QVBoxLayout *layout)
{
    tableView = new QTableView(this);
    layout->addWidget(tableView);
}
