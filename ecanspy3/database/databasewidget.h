#ifndef DATABASEWIDGET_H
#define DATABASEWIDGET_H

#include <QWidget>
#include <QTableView>
#include <QAction>
#include <QMenu>
#include <QPushButton>
#include <QVBoxLayout>
#include <QButtonGroup>

#include <dbc4cpp/parser.h>

#include "dbctablemodel.h"

class DatabaseWidget : public QWidget
{
    Q_OBJECT
public:
    enum {
        PAGE_INDEX_TRANSMIT = 0,
        PAGE_INDEX_RECEIVE,
        PAGE_INDEX_DATABASE
    };

    explicit DatabaseWidget(QWidget *parent = nullptr);
    ~DatabaseWidget();

    void initTableView(QVBoxLayout *layout);
    void intiContextMenu();
    void initAllButtons(QVBoxLayout *layout);

    void resetSizePolicy();

    DBCTableModel *getTransmitTableModel()
    {
        return this->transmitTableModel;
    }

    DBCTableModel *getReceiveTableModel()
    {
        return this->receiveTableModel;
    }

    DBCTableModel *getDatabaseTableModel()
    {
        return this->databaseTableModel;
    }

    int getCurrPageIndex()
    {
        return this->currPageIndex;
    }

    void setCurrPageIndex(int currPageIndex);

    QString getCurrDBCFileName()
    {
        return currDBCFileName;
    }

    void clear();

signals:

    // 刷新发送报文
    void refreshSendMessage(Document *doc, const QList<Message*> &);
    void refreshDocument(Document *doc);

public slots:
    void copyToTransmit();
    void copyToReceive();
    void copyTo(DBCTableModel *model);

    void deleteCurr();

    void switchToTransmit();
    void switchToReceive();
    void switchToDatabase();
    void buttonGroupClicked(QAbstractButton *button);

    void loadDBC();
    void loadDBC(const QString &dbcFile);
    void loadDocument(Document *doc);


    void loadTransmitMsgIdList(QList<int> transmitMsgIdList);
    void loadReceiveMsgIdList(QList<int> receiveMsgIdList);
    void loadDatabaseMsgIdList(QList<int> databaseMsgIdList);

    void loadMsgIdList(QList<int> msgIdList, DBCTableModel &model);
    void loadCurrentPageIndex(int currPageIndex);

    void onContextMenu(const QPoint &pos);

    void onOpenCan();
    void onCloseCan();

    void onContextMenuAboutToShow();

private:
    QTableView *tableView;

    Document *doc;

    DBCTableModel *transmitTableModel;
    DBCTableModel *receiveTableModel;
    DBCTableModel *databaseTableModel;

    QMenu  *contextMenu;
    QAction *copyToTransmitAction;
    QAction *copyToReceiveAction;
    QAction *deleteCurrAction;

    QPushButton *transmitButton;
    QPushButton *receiveButton;
    QPushButton *databaseButton;
    QButtonGroup *buttonGroup;

    QPushButton *loadDBCButton;

    QSizePolicy oldSizePolicy;

    int currPageIndex = -1;
    QString currDBCFileName;
};

#endif // DATABASEWIDGET_H
