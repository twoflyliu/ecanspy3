#include "mainwindowbase.h"

MainWindowBase::MainWindowBase(QWidget *parent) : QMainWindow(parent)
{

}

QDockWidget *MainWindowBase::createDockWidget(const QString &title, QWidget *widget)
{
    QDockWidget *dockWidget = new QDockWidget(title);
    dockWidget->setObjectName(title);
    dockWidget->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    dockWidget->setWidget(widget);
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea
                                |Qt::TopDockWidgetArea|Qt::BottomDockWidgetArea);
    dockWidget->hide();
    dockWidgets.push_back(dockWidget);
    return dockWidget;
}

void MainWindowBase::showDockWidgets()
{
    foreach (QDockWidget *dockWidget, dockWidgets) {
        if (dockWidget) {
            dockWidget->show();
        }
    }
}

void MainWindowBase::hideDockWidgets()
{
    foreach (QDockWidget *dockWidget, dockWidgets) {
        if (dockWidget) {
            dockWidget->hide();
        }
    }
}
