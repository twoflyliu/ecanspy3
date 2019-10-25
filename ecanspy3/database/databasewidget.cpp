#include "databasewidget.h"
#include "mainwindow.h"

#include <QVBoxLayout>
#include <QDebug>
#include <QFileDialog>

#include <QDebug>

DatabaseWidget::DatabaseWidget(QWidget *parent)
    : QWidget(parent), doc(NULL)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    initAllButtons(layout);
    initTableView(layout);
    layout->setContentsMargins(2, 2, 2, 2);

    // 监听CAN状态变化
    connect(&UsbCanUtil::GetInstance(), SIGNAL(openCan()), this, SLOT(onOpenCan()));
    connect(&UsbCanUtil::GetInstance(), SIGNAL(closeCan()), this, SLOT(onCloseCan()));

    setLayout(layout);

    oldSizePolicy = sizePolicy();
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding); //主要为了保证初始化QDockWidget布局是想要的布局
}

DatabaseWidget::~DatabaseWidget()
{
    delete doc;
    doc = NULL;
}

void DatabaseWidget::initTableView(QVBoxLayout *layout)
{
    // 初始化表格
    tableView = new QTableView(this);
    transmitTableModel = new DBCTableModel;
    receiveTableModel = new DBCTableModel;
    databaseTableModel = new DBCTableModel;
    tableView->setModel(databaseTableModel);
    tableView->resizeColumnsToContents();

    // 使能自定义上下文菜单
    tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onContextMenu(QPoint)));

    // 设置选择模式
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    layout->addWidget(tableView);

    // 初始化上下文菜单
    intiContextMenu();
}

void DatabaseWidget::intiContextMenu()
{
    contextMenu = new QMenu(this);

    copyToTransmitAction = new QAction(tr("Copy To Transmit"), this);
    connect(copyToTransmitAction, SIGNAL(triggered(bool)), this, SLOT(copyToTransmit()));
    contextMenu->addAction(copyToTransmitAction);

    copyToReceiveAction = new QAction(tr("Copy To Receive"), this);
    connect(copyToReceiveAction, SIGNAL(triggered(bool)), this, SLOT(copyToReceive()));
    contextMenu->addAction(copyToReceiveAction);

    deleteCurrAction = new QAction(tr("Delete"), this);
    connect(deleteCurrAction, SIGNAL(triggered(bool)), this, SLOT(deleteCurr()));
    contextMenu->addAction(deleteCurrAction);

    connect(contextMenu, SIGNAL(aboutToShow()), this, SLOT(onContextMenuAboutToShow()));
}

void DatabaseWidget::initAllButtons(QVBoxLayout *layout)
{
    QHBoxLayout *hboxLayout = new QHBoxLayout;

    buttonGroup = new QButtonGroup(this);
    connect(buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this,
            SLOT(buttonGroupClicked(QAbstractButton*)));

    transmitButton = new QPushButton(tr("Transmit"), this);
    transmitButton->setCheckable(true);
    buttonGroup->addButton(transmitButton);
    transmitButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    hboxLayout->addWidget(transmitButton);

    receiveButton = new QPushButton(tr("Receive"), this);
    receiveButton->setCheckable(true);
    buttonGroup->addButton(receiveButton);
    receiveButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    hboxLayout->addWidget(receiveButton);

    databaseButton = new QPushButton(tr("Database"), this);
    databaseButton->setCheckable(true);
    databaseButton->setChecked(true);
    currPageIndex = PAGE_INDEX_DATABASE;
    buttonGroup->addButton(databaseButton);
    databaseButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    hboxLayout->addWidget(databaseButton);

    loadDBCButton = new QPushButton(tr("Load DBC..."), this);
    databaseButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(loadDBCButton, SIGNAL(clicked(bool)), this, SLOT(loadDBC()));
    hboxLayout->addWidget(loadDBCButton);

    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed);
    hboxLayout->addSpacerItem(spacer);

    hboxLayout->setSpacing(10);

    layout->addLayout(hboxLayout);
}

void DatabaseWidget::resetSizePolicy()
{
    setSizePolicy(oldSizePolicy);
}

void DatabaseWidget::setCurrPageIndex(int currPageIndex)
{
    if (this->currPageIndex != currPageIndex) {
        this->currPageIndex = currPageIndex;
        UPDATE_DATAChANGED_FLAG();

        switch (this->currPageIndex) {
        case PAGE_INDEX_DATABASE:
            switchToDatabase();
            break;
        case PAGE_INDEX_TRANSMIT:
            switchToTransmit();
            break;
        case PAGE_INDEX_RECEIVE:
            switchToReceive();
            break;
        default:
            break;
        }
    }
}

void DatabaseWidget::clear()
{
    databaseTableModel->clear();
    transmitTableModel->clear();
    receiveTableModel->clear();

    emit refreshSendMessage(NULL, QList<Message*>());
    emit refreshDocument(NULL);
}

// 响应Copy To Transmit菜单
void DatabaseWidget::copyToTransmit()
{
#if SEND_MESSAGE_UNIQUE_ENABLED
    auto oldSize = transmitTableModel->getMsgList().size();
    copyTo(transmitTableModel);
    if (oldSize != transmitTableModel->getMsgList().size()) {
        emit refreshSendMessage(doc, transmitTableModel->getMsgList());
    }
#else
    copyTo(transmitTableModel);
    emit refreshSendMessage(doc, transmitTableModel->getMsgList());
#endif
}

// 响应Copy To Receive菜单
void DatabaseWidget::copyToReceive()
{
    copyTo(receiveTableModel);
}

void DatabaseWidget::copyTo(DBCTableModel *model)
{
    QItemSelectionModel *selectionModel = tableView->selectionModel();
    QModelIndexList indexes = selectionModel->selectedRows();
    DBCTableModel *currModel = static_cast<DBCTableModel *>(tableView->model());

    model->setDocument(doc);
    for (auto iter = indexes.begin(); iter != indexes.end(); ++iter) {
        model->addMsg(currModel->getMsg(iter->row()));
    }
    tableView->setModel(model);

    if (transmitTableModel == model) {
        transmitButton->setChecked(true);
        currPageIndex = PAGE_INDEX_TRANSMIT;
    } else {
        receiveButton->setChecked(true);
        currPageIndex = PAGE_INDEX_RECEIVE;
    }

    UPDATE_DATAChANGED_FLAG();
}

// 响应Delete菜单
void DatabaseWidget::deleteCurr()
{
    QItemSelectionModel *selectionModel = tableView->selectionModel();
    QModelIndexList indexes = selectionModel->selectedRows();
    DBCTableModel *model = static_cast<DBCTableModel *>(tableView->model());
    model->removeRows(indexes);

    if (model == this->transmitTableModel) {
        emit refreshSendMessage(doc, transmitTableModel->getMsgList());
        UPDATE_DATAChANGED_FLAG();
    }
}

// 切换到发送
void DatabaseWidget::switchToTransmit()
{
    transmitButton->setChecked(true);
    tableView->setModel(transmitTableModel);
}

// 切换到接收
void DatabaseWidget::switchToReceive()
{
    receiveButton->setChecked(true);
    tableView->setModel(receiveTableModel);
}

// 切换到数据库
void DatabaseWidget::switchToDatabase()
{
    databaseButton->setChecked(true);
    tableView->setModel(databaseTableModel);
}

void DatabaseWidget::buttonGroupClicked(QAbstractButton *button)
{
    if (databaseButton == button) {
        setCurrPageIndex(PAGE_INDEX_DATABASE);
    } else if (transmitButton == button) {
        setCurrPageIndex(PAGE_INDEX_TRANSMIT);
    } else if (receiveButton == button) {
        setCurrPageIndex(PAGE_INDEX_RECEIVE);
    }
}

// 加载dbc
void DatabaseWidget::loadDBC()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("打开DBC"), tr(""), tr("DBC文件(*.dbc)"));
    if (!fileName.isEmpty()) {
        UPDATE_DATAChANGED_FLAG();
        loadDBC(fileName);
    }
}

void DatabaseWidget::loadDBC(const QString &dbcFile)
{
    DBCParser parser;
    DocumentBuilder builder;

    if (parser.parse(dbcFile, builder)) {
        currDBCFileName = dbcFile;
        loadDocument(builder.getDocument());
        loadCurrentPageIndex(PAGE_INDEX_DATABASE);
    } else {
        qDebug() << tr("Parse Failed:") << parser.getErrMsg();
    }

}

void DatabaseWidget::loadDocument(Document *doc)
{
    if (doc == this->doc) return;
    transmitTableModel->clear();
    receiveTableModel->clear();
    databaseTableModel->clear();
    emit refreshSendMessage(NULL, QList<Message*>());
    emit refreshDocument(doc);

    delete this->doc;
    this->doc = doc;
    databaseTableModel->setMsgList(doc->GetMsgList(), doc);
    tableView->setModel(databaseTableModel);

    tableView->resizeColumnsToContents();
}

void DatabaseWidget::loadTransmitMsgIdList(QList<int> transmitMsgIdList)
{
    Q_ASSERT(transmitTableModel);
    loadMsgIdList(transmitMsgIdList, *transmitTableModel);
    emit refreshSendMessage(doc, transmitTableModel->getMsgList());
}

void DatabaseWidget::loadReceiveMsgIdList(QList<int> receiveMsgIdList)
{
    Q_ASSERT(receiveTableModel);
    loadMsgIdList(receiveMsgIdList, *receiveTableModel);
}

void DatabaseWidget::loadDatabaseMsgIdList(QList<int> databaseMsgIdList)
{
    Q_ASSERT(databaseTableModel);
    loadMsgIdList(databaseMsgIdList, *databaseTableModel);
}

void DatabaseWidget::loadMsgIdList(QList<int> msgIdList, DBCTableModel &model)
{
    if (!this->doc) return;
    model.clear();
    model.setMsgIdList(msgIdList, this->doc);
}

void DatabaseWidget::loadCurrentPageIndex(int currPageIndex)
{
    Q_ASSERT(currPageIndex != -1);
    if (PAGE_INDEX_TRANSMIT == currPageIndex) {
        switchToTransmit();
    } else if (PAGE_INDEX_RECEIVE == currPageIndex) {
        switchToReceive();
    } else if (PAGE_INDEX_DATABASE == currPageIndex) {
        switchToDatabase();
    }
    this->currPageIndex = currPageIndex;
}

// 上下文菜单
void DatabaseWidget::onContextMenu(const QPoint &pos)
{
    QModelIndex index = tableView->indexAt(pos);
    if (index.isValid()) {
        contextMenu->exec(QCursor::pos());
    }
}

void DatabaseWidget::onOpenCan()
{
    this->setEnabled(false);
}

void DatabaseWidget::onCloseCan()
{
    this->setEnabled(true);
}

void DatabaseWidget::onContextMenuAboutToShow()
{
    deleteCurrAction->setEnabled(currPageIndex != PAGE_INDEX_DATABASE);
}
