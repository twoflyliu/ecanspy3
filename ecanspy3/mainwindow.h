#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "base/mainwindowbase.h"
#include "database/databasewidget.h"
#include "receive/filterwidget.h"
#include "send/sendwidget.h"
#include <dbc4cpp/parser.h>

#include "settings/themesettingdialog.h"

namespace Ui {
class MainWindow;
}

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

#define UPDATE_DATAChANGED_FLAG()   MainWindow::dataHasChageFlg = true

class MainWindow : public MainWindowBase
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void initDockWidgets();
    void initToolBar();

    void initCanMonitor();

    void initDispatchSignal();
    void initDataChanged();

    void removeCentralWindow();

    void readSettings();
    void writeSettings();

    void setTheme(QString themeStyleSheet);

    DatabaseWidget *getDatabaseWidget();
    SendWidget *getSendWidget();

protected slots:
    void resetDefaultDockWidgetsLayout();
    void openCan();
    void closeCan();
    void about();
    void configCan();
    void configTheme();

    void triggerMsgOption();

    void save();
    void saveAs();
    void open();
    void create();

    void onOpenCan();
    void onCloseCan();

protected:
    void resetSizePolicy();
    void saveProjectFile(const QString &projectFileName);
    void openProjectFile(const QString &projectFileName);
    void loadLastTheme();

protected:
    QDockWidget *receiveDockWidget;
    QDockWidget *diagDockWidget;
    QDockWidget *statusDockWidget;
    QDockWidget *statisticsDockWidget;
    QDockWidget *logDockWidget;
    QDockWidget *sendDockWidget;
    QDockWidget *databaseDockWidget;

    QString currTheme;                      //当前主题
    QString currProjectFileName;            //当前项目
    QTimer *uiUpdateTimer = nullptr;

private:
    Ui::MainWindow *ui;

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *);

public:
    static bool dataHasChageFlg;     //表示用户有过操作
};


#endif // MAINWINDOW_H
