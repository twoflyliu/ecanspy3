#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "receive/receivewidget.h"
#include "status/statuswidget.h"
#include "log/logwidget.h"
#include "send/sendwidget.h"
#include "database/databasewidget.h"
#include "diag/diagwidget.h"
#include "statistic/statisticswidget.h"
#include "receive/receivemessagewidget.h"
#include "settings/projectfile.h"
#include "settings/canconfigdialog.h"
#include "settings/themesettingdialog.h"

#include "Can/Core.h"

#include <QTextEdit>
#include <QSettings>
#include <aboutecanspy3.h>
#include <QMessageBox>
#include <QFileDialog>
#include <QDataStream>
#include <QInputDialog>
#include <QDebug>

#define COMPANY_NAME tr("TwoFlyLiu")
#define APP_NAME tr("ECanSpy3")
#define MAIN_LAYOUT tr("MainLayout")
#define LAST_PROJECT_FILE tr("LastProjectFile")
#define RECEIVE_TREE_VIEW_COLUMN_WIDTH_LIST tr("ReceiveTreeColumnWidthList")
#define LAST_THEME_NAME tr("LastThemeName")

#define PROJECT_EXTNAME         tr(".ecanspy3")

bool MainWindow::dataHasChageFlg = false;

MainWindow::MainWindow(QWidget *parent) :
    MainWindowBase(parent),
    ui(new Ui::MainWindow)
{
    loadLastTheme();        //主题越早加载越好

    qDebug() << "MainWindow -> setup ui";
    ui->setupUi(this);

    removeCentralWindow();

    initToolBar();
    initCanMonitor();

    initDockWidgets();
    resetDefaultDockWidgetsLayout();

    initDispatchSignal();

    qDebug() << "MainWindow -> setWindowState(Qt::WindowMaximized)";
    setWindowState(Qt::WindowMaximized);

    qDebug() << "MainWindow -> show()";
    //resetDefaultDockWidgetsLayout();
    show(); //显示主要为了显示默认布局

    readSettings();

    resetSizePolicy();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initDockWidgets()
{
    qDebug() << "MainWindow::initDockWidgets";
#if 0
    receiveDockWidget = createDockWidget(tr("接收"), new ReceiveWidget(this));
    diagDockWidget = createDockWidget(tr("诊断"), new DiagWidget(this));
    statusDockWidget = createDockWidget(tr("状态"), new StatusWidget(this));
    statisticsDockWidget = createDockWidget(tr("统计"), new StatisticsWidget(this));
    logDockWidget = createDockWidget(tr("日志"), new LogWidget(this));
    sendDockWidget = createDockWidget(tr("发送"), new SendWidget(this));
    databaseDockWidget = createDockWidget(tr("数据库"), new DatabaseWidget(this));
#else
    receiveDockWidget = createDockWidget(tr("接收"), new ReceiveWidget(this));        //接收
    databaseDockWidget = createDockWidget(tr("数据库"), new DatabaseWidget(this));     //数据库
    sendDockWidget = createDockWidget(tr("发送"), new SendWidget(this));              //发送
    diagDockWidget = createDockWidget(tr("诊断"), new QTextEdit(this));               //诊断
    statusDockWidget = createDockWidget(tr("状态"), new StatusWidget(this));             //状态
    statisticsDockWidget = createDockWidget(tr("统计"), new QTextEdit(this));         //统计
    logDockWidget = createDockWidget(tr("日志"), new QTextEdit(this));                //日志
#endif
}

void MainWindow::initToolBar()
{
    qDebug() << "MainWindow::initToolBar";
    ui->mainToolBar->addAction(ui->newAction);
    connect(ui->newAction, SIGNAL(triggered(bool)), this, SLOT(create()));
    ui->newAction->setStatusTip(tr("新建一个工程"));

    ui->mainToolBar->addAction(ui->openAction);
    connect(ui->openAction, SIGNAL(triggered(bool)), this, SLOT(open()));
    ui->openAction->setStatusTip(tr("打开一个已经存在的工程"));

    ui->mainToolBar->addSeparator();

    ui->mainToolBar->addAction(ui->saveAction);
    connect(ui->saveAction, SIGNAL(triggered(bool)), this, SLOT(save()));
    ui->saveAction->setStatusTip(tr("保存当前工程"));

    ui->mainToolBar->addAction(ui->saveAsAction);
    connect(ui->saveAsAction, SIGNAL(triggered(bool)), this, SLOT(saveAs()));
    ui->saveAsAction->setStatusTip(tr("另存为工程"));

    ui->mainToolBar->addSeparator();

    ui->mainToolBar->addAction(ui->configCanAction);
    connect(ui->configCanAction, SIGNAL(triggered(bool)), this, SLOT(configCan()));
    ui->configCanAction->setStatusTip(tr("配置CAN"));

    ui->mainToolBar->addAction(ui->openCanAction);
    connect(ui->openCanAction, SIGNAL(triggered(bool)), this, SLOT(openCan()));
    ui->openCanAction->setStatusTip(tr("打开CAN"));

    ui->mainToolBar->addAction(ui->closeCanAction);
    connect(ui->closeCanAction, SIGNAL(triggered(bool)), this, SLOT(closeCan()));
    ui->closeCanAction->setEnabled(false);
    ui->closeCanAction->setStatusTip(tr("关闭CAN"));

    ui->mainToolBar->addSeparator();

    ui->mainToolBar->addAction(ui->themeSettingAction);
    connect(ui->themeSettingAction, SIGNAL(triggered(bool)), this, SLOT(configTheme()));
    ui->mainToolBar->addSeparator();
    ui->themeSettingAction->setStatusTip(tr("设置主题"));

    ui->mainToolBar->addAction(ui->resetAllLayoutAction);
    connect(ui->resetAllLayoutAction, SIGNAL(triggered(bool)), this, SLOT(resetDefaultDockWidgetsLayout()));
    ui->mainToolBar->addSeparator();
    ui->resetAllLayoutAction->setStatusTip(tr("恢复Docker所有默认布局"));

    //ui->mainToolBar->addAction(ui->triggerMsgOptionAction);
    connect(ui->triggerMsgOptionAction, SIGNAL(triggered(bool)), this, SLOT(triggerMsgOption()));
    ui->triggerMsgOptionAction->setStatusTip(tr("配置触发报文"));

    ui->mainToolBar->addAction(ui->aboutAction);
    connect(ui->aboutAction, SIGNAL(triggered(bool)), this, SLOT(about()));
    ui->aboutAction->setStatusTip(tr("打开关于对话框"));
}

void MainWindow::initCanMonitor()
{
    qDebug() << "MainWindow::initCanMonitor";
    connect(&UsbCanUtil::GetInstance(), SIGNAL(openCan()), this, SLOT(onOpenCan()));
    connect(&UsbCanUtil::GetInstance(), SIGNAL(closeCan()), this, SLOT(onCloseCan()));
}

void MainWindow::initDispatchSignal()
{
    qDebug() << "MainWindow::initDispatchSignal";
    //转发信号
    DatabaseWidget* databaseWidget = static_cast<DatabaseWidget*>(databaseDockWidget->widget());
    SendMessageWidget *sendMessageWidget = static_cast<SendWidget*>(sendDockWidget->widget())->getSendMessageWidget();
    SignalWidget *signalWidget = static_cast<SendWidget*>(sendDockWidget->widget())->getSignalWidget();
    ReceiveMessageWidget *receiveMessageWidget = static_cast<ReceiveWidget*>(receiveDockWidget->widget())->getReceiveMessageWidget();
    FilterWidget *filterWidget = static_cast<ReceiveWidget*>(receiveDockWidget->widget())->getFilterWidget();

    connect(databaseWidget, SIGNAL(refreshSendMessage(Document*,QList<Message*>)),
            sendMessageWidget, SLOT(resetContent(Document*,QList<Message*>)));
    connect(databaseWidget, SIGNAL(refreshDocument(Document*)),
            filterWidget, SLOT(setFilterList(Document*)));

    connect(sendMessageWidget, SIGNAL(refreshCurrMessage(Message*, CAN_OBJ*)),
            signalWidget, SLOT(setMessage(Message*, CAN_OBJ *)));
    connect(signalWidget, SIGNAL(updateMessageData(Message*)), sendMessageWidget, SLOT(updateMessageData(Message*)));
    connect(databaseWidget, SIGNAL(refreshDocument(Document*)), receiveMessageWidget, SLOT(refreshDocument(Document*)));
}

void MainWindow::resetDefaultDockWidgetsLayout()
{
    qDebug() << "MainWindow::resetDefaultDockWidgetsLayout";
    hideDockWidgets();

    addDockWidget(Qt::TopDockWidgetArea, receiveDockWidget);
    splitDockWidget(receiveDockWidget, databaseDockWidget, Qt::Horizontal);


    addDockWidget(Qt::BottomDockWidgetArea, sendDockWidget);

    tabifyDockWidget(receiveDockWidget, diagDockWidget);

    tabifyDockWidget(databaseDockWidget, statusDockWidget);
    tabifyDockWidget(statusDockWidget, statisticsDockWidget);

    tabifyDockWidget(sendDockWidget, logDockWidget);

    showDockWidgets();
}

void MainWindow::openCan()
{
    if (UsbCanUtil::GetInstance().Open(USBCAN1)) {
    }
}

void MainWindow::closeCan()
{
    UsbCanUtil::GetInstance().Close();
}

void MainWindow::about()
{
    AboutEcanSpy3Diaglog aboutDialog;
    aboutDialog.exec();
}

void MainWindow::configCan()
{
    CanConfigDialog dlg;
    if (QDialog::Accepted == dlg.exec()) {
        QString baurate = dlg.getBaurate();
        qDebug() << "Set baurate:" << baurate;
        auto &inst = UsbCanUtil::GetInstance();
        if (inst.GetBaurate() != baurate) {
            UsbCanUtil::GetInstance().SetBaurate(baurate);
            UPDATE_DATAChANGED_FLAG();
        }
    }
}

void MainWindow::configTheme()
{
    ThemeSettingDialog dlg;
    dlg.setTheme(this->currTheme);
    if (QDialog::Accepted == dlg.exec()) {
        setTheme(dlg.getCurrThemeStyleSheet());
        this->currTheme = dlg.getTheme();
    }
}

void MainWindow::triggerMsgOption()
{
    int contTxCnt = QInputDialog::getInt(this, tr("触发报文选项"), tr("报文连续发送次数"), 3, 1);
    //qDebug() << "contTxCnt=" << contTxCnt;
    MessagePeriodTxMgr::GetInstance().SetContTxCount(contTxCnt);
}

void MainWindow::save()
{
    if (currProjectFileName.isEmpty()) {
        saveAs();
    } else {
        saveProjectFile(currProjectFileName);
    }
}

void MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("保存项目文件"), tr(""),
                                                    tr("EcanSpy3文件(*%1)").arg(PROJECT_EXTNAME));
    if (!fileName.isEmpty()) {
        if (!fileName.endsWith(PROJECT_EXTNAME)) {
            fileName.append(PROJECT_EXTNAME);
        }
        currProjectFileName = fileName;
        saveProjectFile(currProjectFileName);
    }
}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("打开项目文件"), tr(""),
                                                    tr("EcanSpy3文件(*%1)").arg(PROJECT_EXTNAME));
    if (!fileName.isEmpty()) {
        currProjectFileName = fileName;
        openProjectFile(currProjectFileName);
        dataHasChageFlg = false;
    }
}

void MainWindow::create()
{
    if (dataHasChageFlg) {
        dataHasChageFlg = false;
        if (QMessageBox::Yes == QMessageBox::information(this, tr("保存数据"),
                                                         tr("是否需要保存当前项目？"),
                                                         QMessageBox::Yes,
                                                         QMessageBox::No)) {
            save();
        }
    }

    DatabaseWidget* databaseWidget = static_cast<DatabaseWidget*>(databaseDockWidget->widget());
    Q_ASSERT(databaseWidget);

    databaseWidget->clear();
    this->currProjectFileName.clear();
}

void MainWindow::onOpenCan()
{
    ui->openAction->setEnabled(false);
    ui->newAction->setEnabled(false);

    ui->openCanAction->setEnabled(false);
    ui->closeCanAction->setEnabled(true);

    MultiMediaMessagePeriodTxMgr::GetInstance().Start();    //启动定时器
}

void MainWindow::onCloseCan()
{
    ui->openAction->setEnabled(true);
    ui->newAction->setEnabled(true);

    ui->openCanAction->setEnabled(true);
    ui->closeCanAction->setEnabled(false);
    MultiMediaMessagePeriodTxMgr::GetInstance().Stop();     //停止定时器，防止操作DBC的时候出错
}

void MainWindow::removeCentralWindow()
{
    qDebug() << "MainWindow::removeCentralWindow";
    if (ui->centralWidget) {
        delete ui->centralWidget;
        ui->centralWidget = NULL;
    }
}

void MainWindow::readSettings()
{
    qDebug() << "MainWindow::readSettings";
    QSettings settings(COMPANY_NAME, APP_NAME);
    auto byteArray = settings.value(MAIN_LAYOUT).toByteArray();
    restoreState(byteArray);

    currProjectFileName = settings.value(LAST_PROJECT_FILE).toString();
    if (!currProjectFileName.isEmpty()) {
        openProjectFile(currProjectFileName);
        dataHasChageFlg = false;
    }

    // 序列化所有栏目宽度
    ReceiveMessageWidget *receiveMessageWidget =
            static_cast<ReceiveWidget*>(receiveDockWidget->widget())->getReceiveMessageWidget();
    Q_ASSERT(receiveMessageWidget);
    QByteArray arr = settings.value(RECEIVE_TREE_VIEW_COLUMN_WIDTH_LIST).toByteArray();
    QDataStream stream(arr);
    stream.setVersion(QDataStream::Qt_4_0);
    QList<int> columnWidthList;
    stream >> columnWidthList;
    receiveMessageWidget->setColumnWidthList(columnWidthList);

    // 设置最后主题
    //QString theme = settings.value(LAST_THEME_NAME).toString();
    //setTheme(ThemeSettingDialog().getThemeStyleSheet(theme));
}

void MainWindow::writeSettings()
{
    qDebug() << "MainWindow::writeSettings";
    QSettings settings(COMPANY_NAME, APP_NAME);

    qDebug() << "MainWindow::writeSettings -> save main window state";
    settings.setValue(MAIN_LAYOUT, saveState());

    qDebug() << tr("MainWindow::writeSettings -> save curr project file [%1]").arg(currProjectFileName);
    settings.setValue(LAST_PROJECT_FILE, currProjectFileName);

    // 反序列化所有栏目宽度
    qDebug() << "MainWindow::writeSettings -> save receive message column layout";
    ReceiveMessageWidget *receiveMessageWidget =
            static_cast<ReceiveWidget*>(receiveDockWidget->widget())->getReceiveMessageWidget();
    Q_ASSERT(receiveMessageWidget);

    QList<int> columnWidthList = receiveMessageWidget->getColumnWidthList();
    QByteArray arr;
    QDataStream stream(&arr, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_4_0);
    stream << columnWidthList;
    settings.setValue(RECEIVE_TREE_VIEW_COLUMN_WIDTH_LIST, arr);

    // 保存主题
    qDebug() << tr("MainWindow::writeSettings -> save current theme [%1]").arg(currTheme);
    settings.setValue(LAST_THEME_NAME, this->currTheme);
}

void MainWindow::setTheme(QString themeStyleSheet)
{
    qDebug() << tr("MainWindow::setTheme(%1)").arg(themeStyleSheet);
    QString styleSheet;

    if (!themeStyleSheet.isEmpty()) {
        QFile file(themeStyleSheet);
        if (file.open(QFile::ReadOnly)) {
            styleSheet = QObject::tr(file.readAll());
        }
    }

    qApp->setStyleSheet(styleSheet);
}

DatabaseWidget *MainWindow::getDatabaseWidget()
{
    return static_cast<DatabaseWidget*>(databaseDockWidget->widget());
}

SendWidget *MainWindow::getSendWidget()
{
    return static_cast<SendWidget*>(sendDockWidget->widget());
}


void MainWindow::resetSizePolicy()
{
    qDebug() << "MainWindow::resetSizePolicy";
    // 为了打破一些约束，允许自动拉动
    getDatabaseWidget()->resetSizePolicy();
    getSendWidget()->resetSizePolicy();
}

void MainWindow::saveProjectFile(const QString &projectFileName)
{
    DatabaseWidget* databaseWidget = static_cast<DatabaseWidget*>(databaseDockWidget->widget());
    Q_ASSERT(databaseWidget);

    ProjectFile projectFile;
    projectFile.setTransmitTableModel(databaseWidget->getTransmitTableModel());
    projectFile.setReceiveTableModel(databaseWidget->getReceiveTableModel());
    projectFile.setDatabaseTableModel(databaseWidget->getDatabaseTableModel());

    FilterWidget *filterWidget = static_cast<ReceiveWidget*>(receiveDockWidget->widget())->getFilterWidget();
    Q_ASSERT(filterWidget);
    projectFile.setFilterTableModel(filterWidget->getFilterTableModel());

    projectFile.setCurrDatabasePageIndex(databaseWidget->getCurrPageIndex());
    projectFile.setDBCFile(databaseWidget->getCurrDBCFileName());

    projectFile.setBaurate(UsbCanUtil::GetInstance().GetBaurate());

    auto err = projectFile.save(projectFileName);
    if (ProjectFile::ERROR_CODE_FILE_CREATE_FAILED == err) {
        QMessageBox::critical(this, tr("保存失败"), tr("文件%1创建失败").arg(projectFileName));
    } else if (ProjectFile::ERROR_CODE_SUCCESS == err) {
        dataHasChageFlg = false;
    }
}

void MainWindow::openProjectFile(const QString &projectFileName)
{
    ProjectFile projectFile;
    auto err = projectFile.load(projectFileName);
    if (ProjectFile::ERROR_CODE_FILE_NOT_EXIST == err) {
        QMessageBox::critical(this, tr("打开失败"), tr("文件%1打开失败").arg(projectFileName));
    } else if (ProjectFile::ERROR_CODE_SUCCESS == err) {
        qDebug() << QString("Open project file [%1] success!").arg(projectFileName);
    }

    DatabaseWidget* databaseWidget = static_cast<DatabaseWidget*>(databaseDockWidget->widget());
    Q_ASSERT(databaseWidget);

    databaseWidget->loadDBC(projectFile.getDBCFile());
    databaseWidget->loadDatabaseMsgIdList(projectFile.getDatabaseMsgIdList());
    databaseWidget->loadTransmitMsgIdList(projectFile.getTransmitMsgIdList());
    databaseWidget->loadReceiveMsgIdList(projectFile.getReceiveMsgIdList());
    databaseWidget->loadCurrentPageIndex(projectFile.getCurrDatabasePageIndex());

    FilterWidget *filterWidget = static_cast<ReceiveWidget*>(receiveDockWidget->widget())->getFilterWidget();
    Q_ASSERT(filterWidget);
    filterWidget->loadFilterEntList(projectFile.getFilterEntList());

    QString baurate = projectFile.getBaurate();
    if (!baurate.isEmpty()) {
        UsbCanUtil::GetInstance().SetBaurate(baurate);
    }
}

void MainWindow::loadLastTheme()
{
    qDebug() << "MainWindow::loadLastTheme()";

    QSettings settings(COMPANY_NAME, APP_NAME);

    QString theme = settings.value(LAST_THEME_NAME).toString();
    this->currTheme = theme;

    qDebug() << tr("MainWindow::loadLastTheme() -> setTheme(%1)").arg(theme);
    setTheme(ThemeSettingDialog().getThemeStyleSheet(theme));
    qDebug() << "MainWindow::loadLastTheme() -> end";
}


void MainWindow::closeEvent(QCloseEvent *)
{
    if (dataHasChageFlg) {
        dataHasChageFlg = false;
        if (QMessageBox::Yes == QMessageBox::question(this, tr("提示"), tr("当前项目已经改变，是否需要保存"),
                                                      QMessageBox::Yes, QMessageBox::No)) {
            save();
        }
    }
    writeSettings();
}
