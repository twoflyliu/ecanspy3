#ifndef PROJECTFILE_H
#define PROJECTFILE_H

#include "database/dbctablemodel.h"
#include "receive/filtertablemodel.h"

#include <QObject>
#include <QtXml>

#include <dbc4cpp/parser.h>

#define DBC_MODEL_NAME_TRANSMIT     "transmit"
#define DBC_MODEL_NAME_RECEIVE      "receive"
#define DBC_MODEL_NAME_DATABASE     "database"

class ProjectFile : public QObject
{
    Q_OBJECT
public:

    enum ErrorCode {
        ERROR_CODE_SUCCESS = 0,
        ERROR_CODE_FILE_NOT_EXIST,
        ERROR_CODE_FILE_CREATE_FAILED,
        ERROR_CODE_PARSE_ERROR
    };

    explicit ProjectFile(QObject *parent = nullptr);

    /*!
     * \brief save 保存当前项目数据
     *
     * 保存数据时候，需要设置号相应的Model
     *
     * \param projectFile 要加的文件文件所在路径
     * \retval 返回错误码, 如果返回ERROR_CODE_SUCCESS，则说明保存成功
     */
    ErrorCode save(const QString &projectFile);

    /*!
     * \brief load  加载当前项目数据
     * \param projectFile 要加的文件文件所在路径
     * \retval 返回错误码, 如果返回ERROR_CODE_SUCCESS，则说明保存成功
     */
    ErrorCode load(const QString &projectFile);

    void setDBCFile(QString dbcFile)
    {
        this->dbcFile = dbcFile;
    }

    QString getDBCFile() const
    {
        return this->dbcFile;
    }

    void setTransmitTableModel(DBCTableModel *transmitTableModel)
    {
        this->transmitTableModel = transmitTableModel;
    }

    void setReceiveTableModel(DBCTableModel *receiveTableModel)
    {
        this->receiveTableModel = receiveTableModel;
    }

    void setDatabaseTableModel(DBCTableModel *databaseTableModel)
    {
        this->databaseTableModel = databaseTableModel;
    }

    void setFilterTableModel(FilterTableModel *filterTableModel)
    {
        this->filterTableModel = filterTableModel;
    }

    void setCurrDatabasePageIndex(int currDatabasePageIndex)
    {
        this->currDatabasePageIndex = currDatabasePageIndex;
    }

    int getCurrDatabasePageIndex()
    {
        return this->currDatabasePageIndex;
    }

    QList<int> getTransmitMsgIdList()
    {
        return transmitMsgIdList;
    }

    QList<int> getReceiveMsgIdList()
    {
        return receiveMsgIdList;
    }

    QList<int> getDatabaseMsgIdList()
    {
        return databaseMsgIdList;
    }

    QList<FilterTableModel::Entity> getFilterEntList()
    {
        return this->filterEntList;
    }

    QString getBaurate() const
    {
        return this->baurate;
    }

    void setBaurate(QString baurate)
    {
        this->baurate = baurate;
    }

signals:

public slots:

private:
    QDomElement createDatabaseElement(QDomDocument &doc);
    QDomElement createTextElement(QDomDocument &doc, const QString &tagName, const QString &innerText);
    QDomElement createDBCTableModel(QDomDocument &doc, const QString &name, DBCTableModel *model);

    QDomElement createFilterElement(QDomDocument &doc);
    QDomElement createFilterTableModelElement(QDomDocument &doc, FilterTableModel *model);

    QDomElement createCanElement(QDomDocument &doc);
    QDomElement createCanBaurateElement(QDomDocument &doc);

    ErrorCode doParseDatabase(QDomElement databaseElem);
    ErrorCode doParseFilter(QDomElement filterElem);
    ErrorCode doParseCan(QDomElement canElem);

private:
    // Database相关配置
    QString dbcFile;                            //!< 当前DBC文件名称

    DBCTableModel *transmitTableModel = nullptr;          //!< 发送Database模型
    DBCTableModel *receiveTableModel = nullptr;           //!< 接收Database模型
    DBCTableModel *databaseTableModel = nullptr;          //!< Database模型
    FilterTableModel *filterTableModel = nullptr;         //!< 过滤TableModel

    QList<int> transmitMsgIdList;
    QList<int> receiveMsgIdList;
    QList<int> databaseMsgIdList;
    QList<FilterTableModel::Entity> filterEntList;

    int currDatabasePageIndex = 2;                     //!< Database DockWidget当前页面索引
    QString baurate;
};

#endif // PROJECTFILE_H
