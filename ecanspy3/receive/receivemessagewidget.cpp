#include "receivemessagewidget.h"
#include <QDebug>
#include <QPainter>
#include <QFileDialog>
#include <QMessageBox>

template <typename T>
static inline void safeDelete(T *&ptr)
{
    delete ptr;
    ptr = nullptr;
}

ReceiveMessageWidget::ReceiveMessageWidget(QWidget *parent)
    : TitleTreeViewWidget(parent)
{
    initTitle();
    initModel();
    initTimer();
    initToolButtons();
    initColumnsWidth();
}

ReceiveMessageWidget::~ReceiveMessageWidget()
{
    safeDelete(scrollingFile);
}

void ReceiveMessageWidget::initTitle()
{
    title->setText(tr("报文"));
}

void ReceiveMessageWidget::initModel()
{
    model = new ReceiveMessageTreeModel(this);
    model->setTreeView(treeView);

#if RECEIVE_FILTER_PROXY_ENABLED
    proxyModel = new ReceiveMessageFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    proxyModel->setDynamicSortFilter(true);

    treeView->setModel(proxyModel);
#else
    treeView->setModel(model);
#endif
    treeView->setUniformRowHeights(true);

    treeView->setColumnWidth(0, 125);
    treeView->setColumnWidth(5, 200);
}

void ReceiveMessageWidget::initTimer()
{
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start(RECEIVE_INTERVAL_MS);
}

void ReceiveMessageWidget::initToolButtons()
{
    QHBoxLayout *hBoxLayout = new QHBoxLayout;

    // 显示/停止显示按钮
    stopShowButton = new QPushButton(tr("Show/Stop"), this);
    stopShowButton->setCheckable(true);
    stopShowButton->setChecked(true);
    connect(stopShowButton, SIGNAL(clicked(bool)), this, SLOT(onStartReceive(bool)));

    // 初始化清空按钮
    clearButton = new QPushButton(tr("Clear"), this);
    connect(clearButton, SIGNAL(clicked(bool)), model, SLOT(clear()));

    // 重新调整所有栏目
    resizeColumns = new QPushButton(tr("Resize Columns"), this);
    connect(resizeColumns, SIGNAL(clicked(bool)), this, SLOT(onResizeColumns()));

    // 滚动模式/统计模式
    scrollButton = new QPushButton(tr("Scroll"), this);
    scrollButton->setCheckable(true);
    scrollButton->setChecked(model->isScrollUpdate());
    connect(scrollButton, SIGNAL(clicked(bool)), model, SLOT(setScroll(bool)));

    // 保存按钮
    saveButton = new QPushButton(tr("Save"), this);
    connect(saveButton, SIGNAL(clicked(bool)), this, SLOT(onSave()));

    // 实时保存按钮
    realTimeSaveButton = new QPushButton(tr("RealTime Save"), this);
    realTimeSaveButton->setCheckable(true);
    connect(realTimeSaveButton, SIGNAL(clicked(bool)), this, SLOT(onRealTimeSave(bool)));


    hBoxLayout->addWidget(stopShowButton);
    hBoxLayout->addSpacing(5);
    hBoxLayout->addWidget(clearButton);
    hBoxLayout->addSpacing(5);
    hBoxLayout->addWidget(resizeColumns);
    hBoxLayout->addSpacing(5);
    hBoxLayout->addWidget(scrollButton);
    hBoxLayout->addSpacing(5);
    hBoxLayout->addWidget(saveButton);
    hBoxLayout->addSpacing(5);
    hBoxLayout->addWidget(realTimeSaveButton);

    QSpacerItem *spacerItem = new QSpacerItem(0, 0, QSizePolicy::Expanding);
    hBoxLayout->addSpacerItem(spacerItem);

    QVBoxLayout *mainLayout = static_cast<QVBoxLayout*>(layout());
    if (mainLayout) {
        mainLayout->insertLayout(1, hBoxLayout);
    }
}

void ReceiveMessageWidget::initColumnsWidth()
{
    //enum { COLUMN_SERIAL_NUM=0, COLUMN_PERIOD_MS, COLUMN_INDEX_ID, COLUMN_INDEX_NAME, COLUMN_INDEX_LEN,
    //       COLUMN_INDEX_DATA, COLUMN_FRAME_TYPE, COLUMN_FRAME_FMT, COLUMN_INDEX_ABS_TIME};
    int maxColumnsWidth[ReceiveMessageTreeModel::COLUMN_COUNT] = {16, 12, 5, 30, 5, 25, 13, 12, 10};
    int charWidth = getTextWidth(QChar('X'));
    for (int i = 0; i < ReceiveMessageTreeModel::COLUMN_COUNT; i++) {
        treeView->setColumnWidth(i, charWidth * maxColumnsWidth[i]);
    }
}

int ReceiveMessageWidget::getTextWidth(QChar ch)
{
    QPainter p(this);
    QFontMetrics fm = p.fontMetrics();
    return fm.width(ch);
}

void ReceiveMessageWidget::doRealTimeSave(const ReceiveMessageTreeModel::Entity &ent)
{
    if (!scrollingFile) return;
    QByteArray buf;
    QTextStream stream(&buf);
    doSave(stream, ent);
    scrollingFile->write(buf);
}

void ReceiveMessageWidget::doSave(QTextStream &stream, const ReceiveMessageTreeModel::Entity &ent)
{
    int columnCount = model->columnCount(QModelIndex());
    for (int i = 0; i < columnCount; i++) {
        stream << model->messageColumnData(ent, i, Qt::DisplayRole).toString();
        if (i != columnCount-1) {
            stream << ",";
        } else {
            stream << "\r\n";
        }
    }
    stream.flush();
}

void ReceiveMessageWidget::setFilterList(QList<FilterTableModel::Entity> *filterList)
{
#if RECEIVE_FILTER_PROXY_ENABLED
    if (this->proxyModel) {
        proxyModel->setFilterList(filterList);
    }
#else
    Q_UNUSED(filterList);
#endif
}

void ReceiveMessageWidget::setCanObjFilter(CanObjFilter *filter)
{
    if (model) {
        model->setCanObjFilter(filter);
    }
}

QList<int> ReceiveMessageWidget::getColumnWidthList() const
{
    QList<int> columnWidthList;
    for (int i = 0; i < ReceiveMessageTreeModel::COLUMN_COUNT; ++i) {
        columnWidthList.append(treeView->columnWidth(i));
    }
    return columnWidthList;
}

void ReceiveMessageWidget::setColumnWidthList(QList<int> widthList)
{
    Q_ASSERT(treeView);
    int columnCount = qMin(widthList.size(), (int)ReceiveMessageTreeModel::COLUMN_COUNT-1);
    for (int i = 0; i < columnCount; i++) {
        treeView->setColumnWidth(i, widthList.at(i));
    }
}

void ReceiveMessageWidget::refreshDocument(Document *doc)
{
    model->setDocument(doc);
    qDebug() << "ReceiveMessageWidget::refreshDocument";
}

void ReceiveMessageWidget::updateFilter()
{
#if RECEIVE_FILTER_PROXY_ENABLED
    if (proxyModel) {
        proxyModel->refilter();
    }
#endif
}

void ReceiveMessageWidget::onTimer()
{
    auto &usbCan = UsbCanUtil::GetInstance();
    CAN_OBJ objBuffer[RECEIVE_BUFFER_SIZE];

    DWORD len = usbCan.Receive(objBuffer, RECEIVE_BUFFER_SIZE);
    for (int i = 0; i < (int)len; i++) {
        auto ent = model->addCanObj(objBuffer[i]);
        if (ent) {
            doRealTimeSave(*ent);
        }
    }
}

void ReceiveMessageWidget::onStartReceive(bool checked)
{
    if (checked) {
        model->startReceive();
    } else {
        model->stopReceive();
    }
}

void ReceiveMessageWidget::onSave()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("保存日志"), tr(""), tr("日志文件(*.csv)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
            QMessageBox::critical(this, tr("错误"), tr("打开%1失败").arg(fileName));
            return;
        }

        // 保存头
        QTextStream stream(&file);
        int columnCnt = model->columnCount(QModelIndex());
        for (int i = 0; i < columnCnt; ++i) {
            stream << model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
            if (i != columnCnt -1) {
                stream << ",";
            } else {
                stream << "\r\n";
            }
        }

        // 保存内容
        auto &objList = model->getObjList();
        for (auto iter = objList.begin(); iter != objList.end(); ++iter) {
            doSave(stream, *iter);
        }
    }
}

void ReceiveMessageWidget::onRealTimeSave(bool checked)
{
    if (checked) {
        QString fileName = QFileDialog::getSaveFileName(this,
                                                        tr("保存日志"), tr(""), tr("日志文件(*.csv)"));
        if (!fileName.isEmpty()) {
            scrollingFile = new ScrollingFile(fileName);
        }
    } else {
        scrollingFile->flush();
        safeDelete(scrollingFile);
    }
}

void ReceiveMessageWidget::onResizeColumns()
{
    int columns = model->columnCount(QModelIndex());
    for (int i = 0; i < columns; i++) {
        treeView->resizeColumnToContents(i);
    }
}
