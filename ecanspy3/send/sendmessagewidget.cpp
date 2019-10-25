#include "sendmessagewidget.h"
#include "UsbCanUtil.h"


#include "Core.h"

#include <QPushButton>
#include <QDebug>
#include <QHeaderView>
#include <QGroupBox>
#include <QApplication>
#include <QDesktopWidget>

#include <QMessageBox>

#define FILTER_TEXT_MESSAGE_NAME  QObject::tr("Name")
#define FILTER_TEXT_MESSAGE_ID    QObject::tr("ID")

#define FILTER_INDEX_MESSAGE_NAME 0
#define FILTER_INDEX_MESSAGE_ID   1

#define MESSAGE_TYPE_CYCLIC       QObject::tr("Cyclic")
#define MESSAGE_TYPE_IFACTIVE     QObject::tr("IfActive")

SendMessageWidget::SendMessageWidget(QWidget *parent)
    : TitleTableViewWidget(parent)
{
    initTitle();
    initTableView();
    initModel();
    initCan();

    initToolButtons();

    maxWidth = maximumWidth();
    QRect deskRect = QApplication::desktop()->availableGeometry();
    setMaximumWidth(deskRect.width()/2 - 10);
}

SendMessageWidget::~SendMessageWidget()
{
    MultiMediaMessagePeriodTxMgr::GetInstance().Close();
}

void SendMessageWidget::resetSizePolicy()
{
    //setMaximumWidth(-1);
}

void SendMessageWidget::resetContent(Document *doc, QList<Message *> msgList)
{
    qDebug() << "SendMessageWidget::resetContent->" << "Step 1";
    model->reset();                         //清空里面的内容

    qDebug() << "SendMessageWidget::resetContent->" << "Step 2";
    emit refreshCurrMessage(NULL, NULL);

    //QMessageBox::information(this, tr("调试"), tr("延时"));

    auto &txMgr = MessagePeriodTxMgr::GetInstance();
    qDebug() << "SendMessageWidget::resetContent->" << "Step 3" << QString(", Size: %1").arg(txMgr.size());
    txMgr.clear();

    qDebug() << "SendMessageWidget::resetContent->" << "Step 4";

    // 添加数据
    model->setDocument(doc);
    model->beginResetModel();
    foreach (Message *msg, msgList) {

        TxMessageTableModel::MoreInfo more;
        CAN_OBJ obj;
        memset(&obj, 0, sizeof(obj));
        obj.ID = msg->getId();
        obj.DataLen = msg->getByteLen();
        obj.ExternFlag = (obj.ID > 0x7ff);

        more.name = msg->getName();
        more.type = QString("");
        more.tx = false;

        const AdditionalAttribute* attr = doc->getMsgAttribute(msg->getId(), "GenMsgCycleTime");
        if (attr) {
            more.period = attr->getIntValue();
            more.type = MESSAGE_TYPE_CYCLIC;

            // 将报文添加到发送管理中
            txMgr.AddMessagePeriodTx(obj.ID, obj.DataLen, more.period, qrand() % 200);
        } else {
            more.period = 0;
            more.type = MESSAGE_TYPE_IFACTIVE;

            // 将报文添加到发送管理中
            txMgr.AddMessageTriggerTx(obj.ID, obj.DataLen, more.period, qrand() % 200);
        }

        txMgr.Stop(obj.ID);

        auto txObj = txMgr.GetCanObj(obj.ID);
        if (txObj) {
            txObj->ExternFlag = (obj.ID > 0x7ff);
            model->appendRow(*txObj, more);
        } else {
            Q_ASSERT(false && "Program logic error!");
            continue;
        }
    }
    model->endResetModel();
    tableView->resizeColumnsToContents();


    // 选中第一行
    if (msgList.size() > 0) {
#if SEND_MESSAGE_TABLE_VIEW_FILTER_ENABLED
        tableView->setCurrentIndex(proxyModel->mapFromSource(model->index(0, 0)));
#else
        tableView->setCurrentIndex(model->index(0, 0));
#endif
    }
}

void SendMessageWidget::sendButtonClicked(int row, int col)
{
    auto currModel = tableView->model();
    if (row >= currModel->rowCount() || col != TX_MESSAGE_TABLE_VIEW_TX_EN) return;

    QString idStr = currModel->data(currModel->index(row, TX_MESSAGE_TABLE_VIEW_COLUMN_INDEX_ID)).toString();
    QByteArray ba(idStr.toAscii());
    int id = ba.toInt(0, 16);

    auto type = currModel->data(currModel->index(row, TX_MESSAGE_TABLE_VIEW_MESSAGE_TYPE)).toString();
    auto checked = currModel->data(currModel->index(row, TX_MESSAGE_TABLE_VIEW_TX_EN)).toBool();

    if (MESSAGE_TYPE_CYCLIC == type) {
        enableSendMessage(id, checked);
    } else if (MESSAGE_TYPE_IFACTIVE == type) {
        enableSendMessage(id, true);      //对于触发报文需要额外处理
        currModel->setData(currModel->index(row, TX_MESSAGE_TABLE_VIEW_TX_EN), false, Qt::UserRole);
    }
}

void SendMessageWidget::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
#if SEND_MESSAGE_TABLE_VIEW_FILTER_ENABLED
    QModelIndex currIndex = proxyModel->mapToSource(current);
#else
    const QModelIndex &currIndex = current;
#endif
    Message *msg = model->getMessage(currIndex);
    CAN_OBJ *obj = model->getCanObj(currIndex);

    if (msg != NULL && obj != NULL) {
        emit refreshCurrMessage(msg, obj);
    }
}

void SendMessageWidget::updateMessageData(Message *msg)
{
    model->refreshMessageData(msg);
}

void SendMessageWidget::onToggleAllCyclicButton(bool checked)
{
    QModelIndex index;
    int rowCount = model->rowCount(index);
    for (int i = 0; i < rowCount; i++) {
        index = model->index(i, TX_MESSAGE_TABLE_VIEW_TX_EN);
        QString msgType = model->data(model->index(i, TX_MESSAGE_TABLE_VIEW_MESSAGE_TYPE), Qt::DisplayRole).toString();

        if (MESSAGE_TYPE_CYCLIC == msgType) {
            model->setData(index, checked, Qt::UserRole);
        }

        auto msg = model->getMessage(index);
        enableSendMessage(msg->getId(), checked);
    }
}

#if SEND_MESSAGE_TABLE_VIEW_FILTER_ENABLED
void SendMessageWidget::filterTextChanged(QString text)
{
    proxyModel->setFilterRegExp(QRegExp(text, Qt::CaseInsensitive, QRegExp::Wildcard));
}

void SendMessageWidget::filterColumnChanged(int index)
{
    if (FILTER_INDEX_MESSAGE_ID == index) {
        proxyModel->setFilterKeyColumn(TX_MESSAGE_TABLE_VIEW_COLUMN_INDEX_ID);
    } else if (FILTER_INDEX_MESSAGE_NAME == index) {
        proxyModel->setFilterKeyColumn(TX_MESSAGE_TABLE_VIEW_COLUMN_INDEX_NAME);
    }
}
#endif

void SendMessageWidget::initTitle()
{
    title->setText(tr("发送报文"));
}

void SendMessageWidget::initModel()
{
    model = new TxMessageTableModel(NULL, this);

#if SEND_MESSAGE_TABLE_VIEW_FILTER_ENABLED
    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    tableView->setModel(proxyModel);
#else
    tableView->setModel(model);
#endif

    tableView->resizeColumnsToContents();

    // 选择模型
    QItemSelectionModel *selectionModel = tableView->selectionModel();
    connect(selectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)), this,
            SLOT(currentChanged(QModelIndex,QModelIndex)));
}

void SendMessageWidget::initTableView()
{
    // 设置选择模式
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);

    checkedItemDelegate = new CheckedButtonDelegate(this);
    checkedItemDelegate->setCheckedButotnColumn(TX_MESSAGE_TABLE_VIEW_TX_EN);
    connect(checkedItemDelegate, SIGNAL(checkedButtonClicked(int,int)), this, SLOT(sendButtonClicked(int,int)));
    tableView->setItemDelegate(checkedItemDelegate);
}

void SendMessageWidget::initCan()
{
    if (UsbCanUtil::GetInstance().Open(USBCAN1)) {
        //QTimerMessagePeriodTxMgr::GetInstance().Start();
    }
}

void SendMessageWidget::initToolButtons()
{
    QVBoxLayout *mainLayout = static_cast<QVBoxLayout*>(layout());
    QHBoxLayout *toolButtonsLayout = new QHBoxLayout;
    mainLayout->insertLayout(1, toolButtonsLayout);

    // 使能/失能按钮
    enableAllCyclicButton = new QPushButton(tr("Enable/Disable All"), this);
    enableAllCyclicButton->setCheckable(true);
    enableAllCyclicButton->setChecked(false);
    connect(enableAllCyclicButton, SIGNAL(clicked(bool)), this, SLOT(onToggleAllCyclicButton(bool)));

    toolButtonsLayout->addWidget(enableAllCyclicButton);

    //resize columns按钮
    resizeColumnsButton = new QPushButton(tr("Resize Columns"), this);
    connect(resizeColumnsButton, SIGNAL(clicked(bool)), tableView, SLOT(resizeColumnsToContents()));
    toolButtonsLayout->addWidget(resizeColumnsButton);

#if SEND_MESSAGE_TABLE_VIEW_FILTER_ENABLED
    initFilter(toolButtonsLayout);
#endif

    QSpacerItem *spacerItem = new QSpacerItem(0, 0, QSizePolicy::Expanding);
    toolButtonsLayout->addSpacerItem(spacerItem);
}

#if SEND_MESSAGE_TABLE_VIEW_FILTER_ENABLED
void SendMessageWidget::initFilter(QHBoxLayout *toolButtonsLayout)
{
    // 创建UI
    QLabel *columnLabel = new QLabel(tr("Filter Column:"), this);
    filterColumnCombo = new QComboBox(this);
    connect(filterColumnCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(filterColumnChanged(int)));
    filterColumnCombo->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    filterColumnCombo->insertItem(FILTER_INDEX_MESSAGE_NAME, FILTER_TEXT_MESSAGE_NAME);
    filterColumnCombo->insertItem(FILTER_INDEX_MESSAGE_ID, FILTER_TEXT_MESSAGE_ID);
    filterColumnCombo->setCurrentIndex(FILTER_INDEX_MESSAGE_NAME);

    toolButtonsLayout->addWidget(columnLabel);
    toolButtonsLayout->addWidget(filterColumnCombo);
    toolButtonsLayout->addSpacing(5);

    QLabel *patternLabel = new QLabel(tr("Filter Pattern:"), this);
    filterTextEdit = new QLineEdit(this);
    filterTextEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(filterTextEdit, SIGNAL(textChanged(QString)), this, SLOT(filterTextChanged(QString)));
    toolButtonsLayout->addWidget(patternLabel);
    toolButtonsLayout->addWidget(filterTextEdit);
}
#endif

void SendMessageWidget::enableSendMessage(int msgId, bool enabled)
{
    auto &txMgr = MessagePeriodTxMgr::GetInstance();
    if (enabled) {
        txMgr.Start(msgId);
    } else {
        txMgr.Stop(msgId);
    }
    qDebug() << tr("Object: ID: 0x%1").arg(msgId, 0, 16).toUpper();
}
