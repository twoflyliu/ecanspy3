#include "entities.h"

Version::Version(const QString &_version)
    : version(_version)
{

}

const QString &Version::get() const
{
    return version;
}

void Version::set(const QString &version)
{
    this->version = version;
}

NS::NS(const QSet<QString> &set)
    : newSymboSet(set)
{

}

void NS::addNewSymbol(const QString &newSymbol)
{
    this->newSymboSet.insert(newSymbol);
}

void NS::reset()
{
    this->newSymboSet.clear();
}

const QSet<QString> &NS::getNewSymbolSet() const
{
    return this->newSymboSet;
}


BS::BS(quint32 _baurate, quint32 _btr1, quint32 _btr2)
    : baurate(_baurate), btr1(_btr1), btr2(_btr2)
{

}

quint32 BS::getBaurate() const
{
    return this->baurate;
}

void BS::setBaurate(quint32 baurate)
{
    this->baurate = baurate;
}

quint32 BS::getBTR1() const
{
    return this->btr1;
}

void BS::setBTR1(quint32 btr1)
{
    this->btr1 = btr1;
}

quint32 BS::getBTR2() const
{
    return this->btr2;
}

void BS::setBTR2(quint32 btr2)
{
    this->btr2 = btr2;
}

Document::Document()
{
    // 新建默认的节点
    const QString defaultNodeName("Vector__XXX");
    Node *node = new Node;
    if (node) {
        node->setName(defaultNodeName);
        addNode(node);
    }
}

Document::~Document()
{
    removeAllNode();
    removeAllValueTable();
    removeAllEV();
    removeAllEVData();
}

void Document::foreachAllNewSymbols(Document::ForEachNewSymbolFn fn, void *arg)
{
    if (fn != NULL) {
        const QSet<QString> &newSymbolSet = ns.getNewSymbolSet();
        for (auto iter = newSymbolSet.begin(); iter != newSymbolSet.end(); ++iter) {
            if (!fn(arg, *iter)) {
                break;
            }
        }
    }
}

const QSet<QString> &Document::getNS() const
{
    return ns.getNewSymbolSet();
}

void Document::setBS(quint32 baurate, quint32 btr1, quint32 btr2)
{
    bs.setBaurate(baurate);
    bs.setBTR1(btr1);
    bs.setBTR2(btr2);
}

void Document::getBS(quint32 &baurate, quint32 &btr1, quint32 &btr2)
{
    baurate = bs.getBaurate();
    btr1 = bs.getBTR1();
    btr2 = bs.getBTR2();
}

void Document::addNode(Node *node)
{
    if (node != NULL) {
        nodeMap.insert(node->getName(), node);
    }
}

void Document::removeNode(const QString &nodeName)
{
    auto iter = nodeMap.find(nodeName);
    if (iter != nodeMap.end()) {
        delete iter.value();
        nodeMap.erase(iter);
    }
}

void Document::removeNode(Node *node)
{
    if (node != NULL) {
        removeNode(node->getName());
    }
}

void Document::removeAllNode()
{
    // 释放掉nodeList
    for (auto iter = nodeMap.begin(); iter != nodeMap.end(); ++iter) {
        delete iter.value();
        iter.value() = NULL;
    }
    nodeMap.clear();
}

Node *Document::getNode(const QString &nodeName)
{
    auto iter = nodeMap.find(nodeName);
    if (iter != nodeMap.end()) {
        return iter.value();
    }

    return NULL;
}

QList<QString> Document::getBU()
{
    return nodeMap.keys();
}

void Document::addValueTable(ValTable *valTable)
{
    if (valTable != NULL) {
        valueTableMap.insert(valTable->getName(), valTable);
    }
}

void Document::removeValueTable(const QString &name)
{
    auto iter = valueTableMap.find(name);
    if (iter != valueTableMap.end()) {
        delete iter.value();
        iter.value() = NULL;
        valueTableMap.erase(iter);
    }
}

void Document::removeAllValueTable()
{
    for (auto iter = valueTableMap.begin(); iter != valueTableMap.end(); ++iter) {
        delete iter.value();
        iter.value() = NULL;
    }
    valueTableMap.clear();
}

ValTable *Document::getValueTable(const QString &name)
{
    auto iter = valueTableMap.find(name);
    if (iter != valueTableMap.end()) {
        return iter.value();
    }
    return NULL;
}

Message *Document::getMessage(qint32 msgId)
{
    Message *msg = NULL;
    for (auto iter = nodeMap.begin(); iter != nodeMap.end(); ++iter) {
        Node *node = iter.value();
        msg = node->getTxMsg(msgId);
        if (msg != NULL) {
            break;
        }
    }
    return msg;
}

Signal *Document::getSignal(qint32 msgId, const QString &sigName)
{
    Message *msg = getMessage(msgId);
    if (msg != NULL) {
        return msg->getSignal(sigName);
    }
    return NULL;
}

const AdditionalAttribute* Document::getMsgAttribute(quint32 msgId, const QString &attrName) const
{
    // 先从报文表中获取属性
    auto iter = msgAttrTblMap.find(msgId);
    if (iter != msgAttrTblMap.end()) {
        auto &tbl = iter.value();
        return tbl.getAdditionalAttribute(attrName);
    } else { //找不到从默认属性中找
        auto msgIter = defaultAttrTblMap.find(OBJECT_TYPE_BO);
        if (msgIter != defaultAttrTblMap.end()) {
            return msgIter.value().getAdditionalAttribute(attrName);
        }
    }
    return NULL;
}

void Document::setDefaultAttributeIntValue(const QString &attrName, qint32 value)
{
    for (auto iter = defaultAttrTblMap.begin(); iter != defaultAttrTblMap.end(); ++iter) {
        AdditionalAttribute *attr = iter.value().getAdditionalAttribute(attrName);
        if (attr != NULL) {
            attr->setIntValue(value);
            break;
        }
    }
}

void Document::setDefaultAttributeHexValue(const QString &attrName, qint32 value)
{
    for (auto iter = defaultAttrTblMap.begin(); iter != defaultAttrTblMap.end(); ++iter) {
        AdditionalAttribute *attr = iter.value().getAdditionalAttribute(attrName);
        if (attr != NULL) {
            attr->setHexValue(value);
            break;
        }
    }
}

void Document::setDefaultAttributeFloatValue(const QString &attrName, double value)
{
    for (auto iter = defaultAttrTblMap.begin(); iter != defaultAttrTblMap.end(); ++iter) {
        AdditionalAttribute *attr = iter.value().getAdditionalAttribute(attrName);
        if (attr != NULL) {
            attr->setFloatValue(value);
            break;
        }
    }
}

void Document::setDefaultAttributeStringValue(const QString &attrName, const QString &value)
{
    for (auto iter = defaultAttrTblMap.begin(); iter != defaultAttrTblMap.end(); ++iter) {
        AdditionalAttribute *attr = iter.value().getAdditionalAttribute(attrName);
        if (attr != NULL) {
            attr->setStringValue(value);
            break;
        }
    }
}

void Document::setDefaultAttributeEnumValue(const QString &attrName, const QString &value)
{
    for (auto iter = defaultAttrTblMap.begin(); iter != defaultAttrTblMap.end(); ++iter) {
        AdditionalAttribute *attr = iter.value().getAdditionalAttribute(attrName);
        if (attr != NULL) {
            attr->setEnumValue(value);
            break;
        }
    }
}

void Document::onParseDone()
{
    msgList.clear();
    for (auto iter = nodeMap.begin(); iter != nodeMap.end(); ++iter) {
        Node *node = iter.value();
        auto &txMsgMap = node->getTxMsgMap();
        for (auto iter = txMsgMap.begin(); iter != txMsgMap.end(); ++iter) {
            msgList.push_back(iter.value());
        }
    }
}
