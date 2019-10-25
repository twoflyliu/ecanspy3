#include "builder.h"

void DocumentBuilder::onVersion(const Context &context, const QString &version)
{
    (void)context;
    if (NULL == doc) {
        doc = new Document();
        curMsg = NULL;
    }

    doc->setVersion(version);
}

void DocumentBuilder::onNewSymbol(const Context &context, const QString &newSymbol)
{
    (void)context;
    doc->addNewSymbol(newSymbol);
}

void DocumentBuilder::onBS(const Context &context, quint32 baurate, quint32 btr1, quint32 btr2)
{
    (void)context;
    doc->setBS(baurate, btr1, btr2);
}

void DocumentBuilder::onBU(const Context &context, const QString &nodeName)
{
    (void)context;
    Node *node = new Node;
    if (node != NULL) {
        node->setName(nodeName);
        doc->addNode(node);
    }
}

void DocumentBuilder::onValTable(const Context &context, const QString &valTableName,
                                 const QList<Builder::ValueTableEntry> &list)
{
    (void)context;
    ValTable *valTable = new ValTable;
    if (valTable != NULL) {
        valTable->setName(valTableName);
        for (auto iter = list.begin(); iter != list.end(); ++iter) {
            valTable->addValueDesc(iter->value, iter->desc);
        }
        doc->addValueTable(valTable);
    }
}

void DocumentBuilder::onBO(const Context &context, qint32 msgId, const QString &name,
                           qint32 size, const QString transmitter)
{
    (void)context;
    Message *msg = new Message;
    if (msg != NULL) {
        msg->setId(msgId);
        msg->setName(name);
        msg->setByteLen(size);
        msg->addTransmitter(transmitter);

        Node *node = doc->getNode(transmitter);
        if (node != NULL) {
            node->addTxMsg(msg);
        }
    }
    curMsg = msg;
}

void DocumentBuilder::onSG(const Context &context, const QString &signalName, Signal::MultiplexerIndType mpIndicatorType,
                           quint32 multiplexerSwithValue, qint32 startBit,
                           qint32 signalSize, Signal::ByteOrderType byteOrder,
                           Signal::ValueType valueType, double factor, double offset,
                           double minimum, double maximum, const QString &unit, const QList<QString> receivers)
{
    (void)context;
    Signal *signal = new Signal;
    if (signal != NULL) {
        if (curMsg != NULL) {
            signal->setName(signalName);
            signal->setMultiplexerIndicatorType(mpIndicatorType);
            signal->setMultiplexerIndicatorValue(multiplexerSwithValue);
            signal->setStartBit(startBit);
            signal->setBitLen(signalSize);
            signal->setByteOrder(byteOrder);
            signal->setValueType(valueType);
            signal->setFactor(factor);
            signal->setOffset(offset);
            signal->setMinimum(minimum);
            signal->setMaximum(maximum);
            signal->setUnit(unit);

            for (auto iter = receivers.begin(); iter != receivers.end(); ++iter) {
                signal->addReceiver(*iter);
            }
            curMsg->addSignal(signal);
        } else {
            delete signal;
        }
    }
}

void DocumentBuilder::onBoTxBu(const Context &context, qint32 msgId, const QList<QString> transmitters)
{
    (void)context;
    Message *msg = doc->getMessage(msgId);
    if (msg != NULL) {
        for (auto iter = transmitters.begin(); iter != transmitters.end(); ++iter) {
            msg->addTransmitter(*iter);
        }
    }
}

void DocumentBuilder::onSigVal(const Context &context, qint32 msgId, const QString &signalName,
                               const QList<Builder::ValueTableEntry> &list)
{
    (void)context;
    Signal *signal = doc->getSignal(msgId, signalName);
    if (signal != NULL) {
        for (auto iter = list.begin(); iter != list.end(); ++iter) {
            signal->addValueDesc(iter->value, iter->desc);
        }
    }
}

void DocumentBuilder::onEnvVal(const Context &context, const QString &envName,
                               const QList<Builder::ValueTableEntry> &list)
{
    (void)context;
    EV* ev = doc->getEV(envName);
    if (ev != NULL) {
        for (auto iter = list.begin(); iter != list.end(); ++iter) {
            ev->addValueDesc(iter->value, iter->desc);
        }
    }
}

void DocumentBuilder::onEv(const Context &context, const QString &envVarName,
                           EV::EnvVarType envVarType, double minimum, double maximum,
                           const QString &unit, double initValue, quint32 evId, EV::AccessType accessType,
                           const QList<QString> &accessNodeList)
{
    (void)context;
    EV *ev = new EV;
    if (ev != NULL) {
        ev->setName(envVarName);
        ev->setEnvVarType(envVarType);
        ev->setMinimum(minimum);
        ev->setMaximum(maximum);
        ev->setUnit(unit);
        ev->setInitValue(initValue);
        ev->setEvId(evId);
        ev->setAccessType(accessType);
        for (auto iter = accessNodeList.begin(); iter != accessNodeList.end(); ++iter) {
            ev->addAccessNode(*iter);
        }
        doc->addEV(ev);
    }
}

void DocumentBuilder::onEvData(const Context &context, const QString envVarName, quint32 dataSize)
{
    (void)context;
    EnvVarData *evData = new EnvVarData;
    if (evData != NULL) {
        evData->setName(envVarName);
        evData->setDataSize(dataSize);
        doc->addEVData(evData);
    }
}

void DocumentBuilder::onCm(const Context &context, const QString &globalComment)
{
    // 忽略全局注释
    (void)context;
    (void)globalComment;
}

void DocumentBuilder::onBUCM(const Context &context, const QString &nodeName, const QString &comment)
{
    (void)context;
    Node *node = doc->getNode(nodeName);
    if (node != NULL) {
        node->setComment(comment);
    }
}

void DocumentBuilder::onBOCM(const Context &context, qint32 msgId, const QString &comment)
{
    (void)context;
    Message *msg = doc->getMessage(msgId);
    if (msg != NULL) {
        msg->setComment(comment);
    }
}

void DocumentBuilder::onSGCM(const Context &context, qint32 msgId, const QString &sigName, const QString &comment)
{
    (void)context;
    Signal *signal = doc->getSignal(msgId, sigName);
    if (signal != NULL) {
        signal->setComment(comment);
    }
}

void DocumentBuilder::onEVCM(const Context &context, const QString &envVarName, const QString &comment)
{
    (void)context;
    EV *ev = doc->getEV(envVarName);
    if (ev != NULL) {
        ev->setComment(comment);
    }
}

void DocumentBuilder::onIntAttrDef(const Context &context, Document::ObjectType objectType, const QString &attrName,
                                   qint32 minimum, qint32 maximum)
{
    (void)context;
    AdditionalAttribute attr;
    attr.setIntRange(minimum, maximum);
    doc->addObjectAttribute(objectType, attrName, attr);
}

void DocumentBuilder::onHexAttrDef(const Context &context, Document::ObjectType objectType, const QString &attrName,
                                   qint32 minimum, qint32 maximum)
{
   (void)context;
   AdditionalAttribute attr;
   attr.setHexRange(minimum, maximum);
   doc->addObjectAttribute(objectType, attrName, attr);
}

void DocumentBuilder::onFloatAttrDef(const Context &context, Document::ObjectType objectType, const QString &attrName,
                                     double minimum, double maximum)
{
    (void)context;
    AdditionalAttribute attr;
    attr.setFloatRange(minimum, maximum);
    doc->addObjectAttribute(objectType, attrName, attr);
}

void DocumentBuilder::onStringAttrDef(const Context &context, Document::ObjectType objectType, const QString &attrName)
{
    (void)context;
    AdditionalAttribute attr;
    attr.setStringType();
    doc->addObjectAttribute(objectType, attrName, attr);
}

void DocumentBuilder::onEnumAttrDef(const Context &context, Document::ObjectType objectType, const QString &attrName,
                                    QList<QString> attrValues)
{
    (void)context;
    AdditionalAttribute attr;
    attr.setEnumType();
    for (auto iter = attrValues.begin(); iter != attrValues.end(); ++iter) {
        attr.addEnumValue(*iter);
    }
    doc->addObjectAttribute(objectType, attrName, attr);
}

void DocumentBuilder::onIntAttrDefaultDef(const Context &context, const QString &attrName, qint32 value)
{
    (void)context;
    doc->setDefaultAttributeIntValue(attrName, value);
}

void DocumentBuilder::onHexAttrDefaultDef(const Context &context, const QString &attrName, qint32 value)
{
    (void)context;
    doc->setDefaultAttributeHexValue(attrName, value);
}

void DocumentBuilder::onFloatAttrDefaultDef(const Context &context, const QString &attrName, double value)
{
    (void)context;
    doc->setDefaultAttributeFloatValue(attrName, value);
}

void DocumentBuilder::onStringAttrDefaultDef(const Context &context, const QString &attrName, const QString &value)
{
    (void)context;
    doc->setDefaultAttributeStringValue(attrName, value);
}

void DocumentBuilder::onEnumAttrDefaultDef(const Context &context, const QString &attrName, const QString &value)
{
    (void)context;
    doc->setDefaultAttributeEnumValue(attrName, value);
}

void DocumentBuilder::onBUAttributeValue(const Context &context, const QString &nodeName, const QString &attrName, qint32 value)
{
    (void)context;
    doc->setNodeAttributeValue(nodeName, attrName, value);
}

void DocumentBuilder::onBUAttributeValue(const Context &context, const QString &nodeName, const QString &attrName, double value)
{
    (void)context;
    doc->setNodeAttributeValue(nodeName, attrName, value);
}

void DocumentBuilder::onBUAttributeValue(const Context &context, const QString &nodeName, const QString &attrName, const QString &value)
{
    (void)context;
    doc->setNodeAttributeValue(nodeName, attrName, value);
}

void DocumentBuilder::onBOAttributeValue(const Context &context, qint32 msgId, const QString &attrName, qint32 value)
{
    (void)context;
    doc->setMsgAttributeValue(msgId, attrName, value);
}

void DocumentBuilder::onBOAttributeValue(const Context &context, qint32 msgId, const QString &attrName, double value)
{
    (void)context;
    doc->setMsgAttributeValue(msgId, attrName, value);
}

void DocumentBuilder::onBOAttributeValue(const Context &context, qint32 msgId, const QString &attrName, const QString &value)
{
    (void)context;
    doc->setMsgAttributeValue(msgId, attrName, value);
}

void DocumentBuilder::onSGAttributeValue(const Context &context, qint32 msgId, const QString &sigName, const QString &attrName, qint32 value)
{
    (void)context;
    doc->setSigAttributeValue(msgId, sigName, attrName, value);
}

void DocumentBuilder::onSGAttributeValue(const Context &context, qint32 msgId, const QString &sigName, const QString &attrName, double value)
{
    (void)context;
    doc->setSigAttributeValue(msgId, sigName, attrName, value);
}

void DocumentBuilder::onSGAttributeValue(const Context &context, qint32 msgId, const QString &sigName, const QString &attrName, const QString &value)
{
    (void)context;
    doc->setSigAttributeValue(msgId, sigName, attrName, value);
}

void DocumentBuilder::onEvAttributeValue(const Context &context, const QString &varName, const QString &attrName, qint32 value)
{
    (void)context;
    doc->setEvAttributeValue(varName, attrName, value);
}

void DocumentBuilder::onEvAttributeValue(const Context &context, const QString &varName, const QString &attrName, double value)
{
    (void)context;
    doc->setEvAttributeValue(varName, attrName, value);
}

void DocumentBuilder::onEvAttributeValue(const Context &context, const QString &varName, const QString &attrName, const QString &value)
{
    (void)context;
    doc->setEvAttributeValue(varName, attrName, value);
}

void DocumentBuilder::onGblAttributeValue(const Context &context, const QString &attrName, qint32 value)
{
    (void)context;
    doc->setGblAttributeValue(attrName, value);
}

void DocumentBuilder::onGblAttributeValue(const Context &context, const QString &attrName, double value)
{
    (void)context;
    doc->setGblAttributeValue(attrName, value);
}

void DocumentBuilder::onGblAttributeValue(const Context &context, const QString &attrName, const QString &value)
{
    (void)context;
    doc->setGblAttributeValue(attrName, value);
}

void DocumentBuilder::onDone(const Context &context)
{
    (void)context;
    doc->onParseDone();
}

DocumentBuilder::DocumentBuilder()
    : doc(NULL), curMsg(NULL)
{

}

Document *DocumentBuilder::getDocument()
{
    Document *ret = doc;
    if (ret != NULL) {
        doc = NULL;
    }
    return ret;
}
