#ifndef MAINWINDOWBASE_H
#define MAINWINDOWBASE_H

#include <QMainWindow>
#include <QDockWidget>
#include <QList>

class MainWindowBase : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindowBase(QWidget *parent = nullptr);

protected:

    QDockWidget *createDockWidget(const QString &title, QWidget *widget);
    void showDockWidgets();
    void hideDockWidgets();

signals:

public slots:

private:
    QList<QDockWidget*> dockWidgets;
};

#endif // MAINWINDOWBASE_H
