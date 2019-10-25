#ifndef RECEIVEMESSAGEWIDGET_H
#define RECEIVEMESSAGEWIDGET_H

#include "base/titletreeviewwidget.h"
#include "receive/receivemessagetreemodel.h"
#include "receive/receivemessagefilterproxymodel.h"
#include "utils/scrollingfile.h"

#include <QTimer>
#include <QPushButton>

#define RECEIVE_FILTER_PROXY_ENABLED (0)

QT_BEGIN_NAMESPACE
class QTextStream;
QT_END_NAMESPACE

class ReceiveMessageWidget : public TitleTreeViewWidget
{
    Q_OBJECT
public:
    enum {
        RECEIVE_INTERVAL_MS = 10,
        RECEIVE_BUFFER_SIZE = 100,
    };

    ReceiveMessageWidget(QWidget *parent = nullptr);
    ~ReceiveMessageWidget();

    void setFilterList(QList<FilterTableModel::Entity> *filterList);
    void setCanObjFilter(CanObjFilter *filter);

    QList<int> getColumnWidthList() const;
    void setColumnWidthList(QList<int> widthList);

protected:
    void initTitle();
    void initModel();
    void initTimer();
    void initToolButtons();

    void initColumnsWidth();
    int getTextWidth(QChar ch);

    void doRealTimeSave(const ReceiveMessageTreeModel::Entity &ent);
    void doSave(QTextStream &stream, const ReceiveMessageTreeModel::Entity &ent);

public slots:
    void refreshDocument(Document *doc);
    void updateFilter();
    void onTimer();

    void onStartReceive(bool checked);
    void onSave();
    void onRealTimeSave(bool checked);
    void onResizeColumns();

private:
    ReceiveMessageTreeModel *model = nullptr;

#if RECEIVE_FILTER_PROXY_ENABLED
    ReceiveMessageFilterProxyModel *proxyModel = nullptr;
#endif

    QTimer *timer = nullptr;

    QPushButton *stopShowButton = nullptr;
    QPushButton *clearButton = nullptr;
    QPushButton *scrollButton = nullptr;
    QPushButton *saveButton = nullptr;
    QPushButton *realTimeSaveButton = nullptr;
    QPushButton *resizeColumns = nullptr;

    ScrollingFile *scrollingFile = nullptr;
};

#endif // RECEIVEMESSAGEWIDGET_H
