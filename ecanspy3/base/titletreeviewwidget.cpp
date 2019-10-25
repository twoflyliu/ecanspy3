#include "titletreeviewwidget.h"

TitleTreeViewWidget::TitleTreeViewWidget(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    initTitle(layout);
    initTreeView(layout);
    setLayout(layout);
}

void TitleTreeViewWidget::initTitle(QVBoxLayout *layout)
{
    title = new QLabel(this);
    //title->setStyleSheet(QString("background-color:rgb(200,200,200);padding:5;color:black;"));
    layout->addWidget(title);
}

void TitleTreeViewWidget::initTreeView(QVBoxLayout *layout)
{
    treeView = new QTreeView(this);
    layout->addWidget(treeView);
}
