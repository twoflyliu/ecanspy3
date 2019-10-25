#include "projectfile.h"

#include <QFile>

#define XML_VERSION     "1.0"
#define XML_ENCODING    "utf-8"

#define XML_ECANSPY3_ELEM_TAG_NAME "ECanSpy3"
#define XML_DATABASE_ELEM_TAG_NAME "Database"

#define XML_DATABASE_CURR_DBC_FILE_TAG_NAME "CurrDBCFile"
#define XML_DATABASE_CURR_PAGE_INDEX_TAG_NAME "CurrPageIndex"

#define XML_DATABASE_DBC_TABLE_MODEL "DBCTableModel"
#define XML_DATABASE_DBC_TABLE_MSG_ID_TAG_NAME "MsgId"

#define XML_DATABASE_DBC_TABLE_NAME_ATTR_NAME "name"
#define XML_DATABASE_DBC_TABLE_NAME_ATTR_VALUE_TRANSMIT "transmit"
#define XML_DATABASE_DBC_TABLE_NAME_ATTR_VALUE_RECEIVE "receive"
#define XML_DATABASE_DBC_TABLE_NAME_ATTR_VALUE_DATABASE "database"

#define XML_FILTER_TAG_NAME "Filter"
#define XML_FILTER_TABLE_MODEL_TAG_NAME "FilterTableModel"
#define XML_FILTER_ITEM_TAG_NAME "FilterItem"

#define XML_FILTER_START_ID_ATTR_NAME "startId"
#define XML_FILTER_END_ID_ATTR_NAME "endId"
#define XML_FILTER_EN_ATTR_NAME "en"

#define XML_CAN_TAG_NAME "Can"
#define XML_CAN_BAURATE_TAG_NAME "Baurate"

#define XML_INDENT_WIDTH 4

ProjectFile::ProjectFile(QObject *parent) : QObject(parent)
{

}

/**
 *  形成的最终效果是：
 *  <?xml version="1.0" encoding="utf-8" ?>
 *  <ECanSpy3>
 *      <Can>
 *          <Baurate>500KBPS</Baurate>
 *      </Can>
 *      <Database>
 *          <CurrDBCFile>XXX.dbc</CurrDBCFile>
 *          <CurrPageIndex>0</CurrPageIndex>
 *          <DBCTableModel name="transmit">
 *              <MsgId></MsgId>
 *              <MsgId></MsgId>
 *              <MsgId></MsgId>
 *              <MsgId></MsgId>
 *              <MsgId></MsgId>
 *              <MsgId></MsgId>
 *          </DBCTableModel>
 *          <DBCTableModel name="receive">
 *              <MsgId></MsgId>
 *              <MsgId></MsgId>
 *              <MsgId></MsgId>
 *              <MsgId></MsgId>
 *              <MsgId></MsgId>
 *              <MsgId></MsgId>
 *          </DBCTableModel>
 *          <DBCTableModel name="database">
 *              <MsgId></MsgId>
 *              <MsgId></MsgId>
 *              <MsgId></MsgId>
 *              <MsgId></MsgId>
 *              <MsgId></MsgId>
 *              <MsgId></MsgId>
 *          </DBCTableModel>
 *      </Database>
 *      <Filter>
 *          <FilterTableModel>
 *              <FilterItem startId="" endId="" en=""/>
 *              <FilterItem startId="" endId="" en=""/>
 *              <FilterItem startId="" endId="" en=""/>
 *              <FilterItem startId="" endId="" en=""/>
 *              <FilterItem startId="" endId="" en=""/>
 *              <FilterItem startId="" endId="" en=""/>
 *          </FilterTableModel>
 *      </Filter>
 *  </ECanSpy3>
 */
ProjectFile::ErrorCode ProjectFile::save(const QString &projectFile)
{
    QDomDocument doc;

    QFile file(projectFile);
    if (!file.open(QIODevice::WriteOnly|QIODevice::Truncate)) {
        return ERROR_CODE_FILE_CREATE_FAILED;
    }


    // 添加xml指定说明
    QDomProcessingInstruction instruction;
    instruction = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(instruction);

    // 创建根节点
    QDomElement rootElem = doc.createElement(XML_ECANSPY3_ELEM_TAG_NAME);
    doc.appendChild(rootElem);

    // 创建Database节点
    rootElem.appendChild(createDatabaseElement(doc));

    // 创建Filter节点
    rootElem.appendChild(createFilterElement(doc));

    // 创建Can节点
    rootElem.appendChild(createCanElement(doc));

    QTextStream out(&file);
    doc.save(out, XML_INDENT_WIDTH);

    return ERROR_CODE_SUCCESS;
}

ProjectFile::ErrorCode ProjectFile::load(const QString &projectFile)
{
    QFile file(projectFile);
    if (!file.open(QIODevice::ReadOnly)) {
        return ERROR_CODE_FILE_NOT_EXIST;
    }

    QDomDocument doc;
    if (!doc.setContent(&file)) {
        file.close();
        return ERROR_CODE_PARSE_ERROR;
    }
    file.close();

    QDomElement root = doc.documentElement();
    if (root.tagName() != XML_ECANSPY3_ELEM_TAG_NAME) {
        return ERROR_CODE_PARSE_ERROR;
    }

    QDomNodeList elems = root.elementsByTagName(XML_DATABASE_ELEM_TAG_NAME);
    if (elems.size() > 0) {
        auto ret = doParseDatabase(elems.at(0).toElement());
        if (ret != ERROR_CODE_SUCCESS) {
            return ret;
        }
    }

    elems = root.elementsByTagName(XML_FILTER_TAG_NAME);
    if (elems.size() > 0) {
        auto ret = doParseFilter(elems.at(0).toElement());
        if (ERROR_CODE_SUCCESS != ret) {
            return ret;
        }
    }

    elems = root.elementsByTagName(XML_CAN_TAG_NAME);
    if (elems.size() > 0) {
        return doParseCan(elems.at(0).toElement());
    }

    return ERROR_CODE_PARSE_ERROR;
}

QDomElement ProjectFile::createDatabaseElement(QDomDocument &doc)
{
    QDomElement databaseElem = doc.createElement(XML_DATABASE_ELEM_TAG_NAME);
    doc.appendChild(databaseElem);

    databaseElem.appendChild(createTextElement(doc, XML_DATABASE_CURR_DBC_FILE_TAG_NAME, dbcFile));
    databaseElem.appendChild(createTextElement(doc, XML_DATABASE_CURR_PAGE_INDEX_TAG_NAME,
                                               QString("%1").arg(currDatabasePageIndex)));

    databaseElem.appendChild(createDBCTableModel(doc, XML_DATABASE_DBC_TABLE_NAME_ATTR_VALUE_TRANSMIT,
                                                 transmitTableModel));
    databaseElem.appendChild(createDBCTableModel(doc, XML_DATABASE_DBC_TABLE_NAME_ATTR_VALUE_RECEIVE,
                                                 receiveTableModel));
    databaseElem.appendChild(createDBCTableModel(doc, XML_DATABASE_DBC_TABLE_NAME_ATTR_VALUE_DATABASE,
                                                 databaseTableModel));
    return databaseElem;
}

QDomElement ProjectFile::createTextElement(QDomDocument &doc, const QString &tagName,
                                           const QString &innerText)
{
    QDomElement elem = doc.createElement(tagName);
    QDomText text = doc.createTextNode(innerText);
    elem.appendChild(text);
    return elem;
}

QDomElement ProjectFile::createDBCTableModel(QDomDocument &doc, const QString &name, DBCTableModel *model)
{
    QDomElement dbcTableModelElem = doc.createElement(XML_DATABASE_DBC_TABLE_MODEL);
    dbcTableModelElem.setAttribute(XML_DATABASE_DBC_TABLE_NAME_ATTR_NAME, name);

    foreach (const Message *msg, model->getMsgList()) {
        QDomElement msgIdElem = createTextElement(doc, XML_DATABASE_DBC_TABLE_MSG_ID_TAG_NAME,
                                                  QString("%1").arg(msg->getId()));
        dbcTableModelElem.appendChild(msgIdElem);
    }
    return dbcTableModelElem;
}

QDomElement ProjectFile::createFilterElement(QDomDocument &doc)
{
    QDomElement filterElem = doc.createElement(XML_FILTER_TAG_NAME);
    filterElem.appendChild(createFilterTableModelElement(doc, filterTableModel));
    return filterElem;
}

QDomElement ProjectFile::createFilterTableModelElement(QDomDocument &doc, FilterTableModel *model)
{
    QDomElement filterTableModel = doc.createElement(XML_FILTER_TABLE_MODEL_TAG_NAME);

    if (model) {
        auto entities = model->getFilterList();
        for (int i = 0; i < entities->size(); i++) {
            auto &entity = entities->at(i);
            QDomElement filterItemElem = doc.createElement(XML_FILTER_ITEM_TAG_NAME);
            filterItemElem.setAttribute(XML_FILTER_START_ID_ATTR_NAME, QString("%1").arg(entity.idStart));
            filterItemElem.setAttribute(XML_FILTER_END_ID_ATTR_NAME, QString("%1").arg(entity.idEnd));
            filterItemElem.setAttribute(XML_FILTER_EN_ATTR_NAME, QString("%1").arg(entity.enabled));
            filterTableModel.appendChild(filterItemElem);
        }
    }

    return filterTableModel;
}

QDomElement ProjectFile::createCanElement(QDomDocument &doc)
{
    QDomElement elem = doc.createElement(XML_CAN_TAG_NAME);
    elem.appendChild(createCanBaurateElement(doc));
    return elem;
}

QDomElement ProjectFile::createCanBaurateElement(QDomDocument &doc)
{
    return createTextElement(doc, XML_CAN_BAURATE_TAG_NAME, getBaurate());
}

ProjectFile::ErrorCode ProjectFile::doParseDatabase(QDomElement databaseElem)
{
    // 解析DBC文件名称
    auto elems = databaseElem.elementsByTagName(XML_DATABASE_CURR_DBC_FILE_TAG_NAME);
    if (elems.size() > 0) {
        dbcFile = elems.at(0).toElement().text();
    } else {
        return ERROR_CODE_PARSE_ERROR;
    }

    // 解析当前页面索引
    elems = databaseElem.elementsByTagName(XML_DATABASE_CURR_PAGE_INDEX_TAG_NAME);
    if (elems.size() > 0) {
        currDatabasePageIndex = elems.at(0).toElement().text().toInt();
    } else {
        return ERROR_CODE_PARSE_ERROR;
    }

    // 解析DBCTableModel
    elems = databaseElem.elementsByTagName(XML_DATABASE_DBC_TABLE_MODEL);

    for (int i = 0; i < elems.size(); i++) {
        QDomNode node = elems.at(i);
        QDomElement elem = node.toElement();
        QString attrName = elem.attribute(XML_DATABASE_DBC_TABLE_NAME_ATTR_NAME);
        QList<int> *msgList = nullptr;

        if (XML_DATABASE_DBC_TABLE_NAME_ATTR_VALUE_TRANSMIT == attrName) {
            msgList = &transmitMsgIdList;
        } else if (XML_DATABASE_DBC_TABLE_NAME_ATTR_VALUE_RECEIVE == attrName) {
            msgList = &receiveMsgIdList;
        } else if (XML_DATABASE_DBC_TABLE_NAME_ATTR_VALUE_DATABASE == attrName) {
            msgList = &databaseMsgIdList;
        } else {
            continue;
        }

        msgList->clear();
        auto msgIdElems = elem.elementsByTagName(XML_DATABASE_DBC_TABLE_MSG_ID_TAG_NAME);
        for (int j = 0; j < msgIdElems.size(); j++) {
            QDomElement msgIdElem = msgIdElems.at(j).toElement();
            msgList->append(msgIdElem.text().toUInt());
        }
    }

    return ERROR_CODE_SUCCESS;
}

ProjectFile::ErrorCode ProjectFile::doParseFilter(QDomElement filterElem)
{
    //解析FilterTableModel
    filterEntList.clear();
    auto nodes = filterElem.elementsByTagName(XML_FILTER_TABLE_MODEL_TAG_NAME);
    if (nodes.size() > 0) {
        nodes = nodes.at(0).toElement().elementsByTagName(XML_FILTER_ITEM_TAG_NAME);
        FilterTableModel::Entity entity;
        for (int i = 0; i < nodes.size(); i++) {
            auto elem = nodes.at(i).toElement();
            entity.idStart = elem.attribute(XML_FILTER_START_ID_ATTR_NAME).toUInt();
            entity.idEnd = elem.attribute(XML_FILTER_END_ID_ATTR_NAME).toUInt();
            entity.enabled = elem.attribute(XML_FILTER_EN_ATTR_NAME).toUInt();
            filterEntList.append(entity);
        }
    }
    return ERROR_CODE_SUCCESS;
}

ProjectFile::ErrorCode ProjectFile::doParseCan(QDomElement canElem)
{
    auto nodes = canElem.elementsByTagName(XML_CAN_BAURATE_TAG_NAME);
    if (nodes.size() > 0) {
        this->baurate = nodes.at(0).toElement().text();
    }
    return ERROR_CODE_SUCCESS;
}

