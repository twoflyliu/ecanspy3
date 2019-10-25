#ifndef BUILDER_H
#define BUILDER_H

#include "dbc4cpp_global.h"

#include <QString>
#include <QList>

#include "entities.h"
#include "context.h"

BEGIN_DBC4CPP_NAMESPACE

struct Builder {
    struct ValueTableEntry
    {
        qint32 value;
        QString desc;
    };

    virtual ~Builder() {}

    virtual void onVersion(const Context &context, const QString &version) = 0;
    virtual void onNewSymbol(const Context &context, const QString &newSymbol) = 0;
    virtual void onBS(const Context &context, quint32 baurate, quint32 btr1, quint32 btr2) = 0;
    virtual void onBU(const Context &context, const QString &nodeName) = 0;
    virtual void onValTable(const Context &context, const QString &valTableName, const QList<ValueTableEntry> &list) = 0;
    virtual void onBO(const Context &context, qint32 msgId, const QString &name, qint32 size, const QString transmitter) = 0;

    virtual void onSG(const Context &context, const QString &signalName,
                      Signal::MultiplexerIndType mpIndicatorType, quint32 multiplexerSwithValue,
                      qint32 startBit, qint32 signalSize, Signal::ByteOrderType byteOrder,
                      Signal::ValueType valueType, double factor, double offset,
                      double minimum, double maximum, const QString &unit, const QList<QString> receivers) = 0;

    virtual void onBoTxBu(const Context &context, qint32 msgId, const QList<QString> transmitters) = 0;
    virtual void onSigVal(const Context &context, qint32 msgId, const QString &signalName, const QList<ValueTableEntry> &list) = 0;
    virtual void onEnvVal(const Context &context, const QString &envName, const QList<ValueTableEntry> &list) = 0;
    virtual void onEv(const Context &context, const QString &envVarName, EV::EnvVarType envVarType,
                      double minimum, double maximum, const QString &unit,
                      double initValue, quint32 evId, EV::AccessType accessType,
                      const QList<QString> &accessNodeList) = 0;
    virtual void onEvData(const Context &context, const QString envVarName, quint32 dataSize) = 0;

    //CM_
    virtual void onCm(const Context &context, const QString &globalComment) = 0;
    virtual void onBUCM(const Context &context, const QString &nodeName, const QString &comment) = 0;
    virtual void onBOCM(const Context &context, qint32 msgId, const QString &comment) = 0;
    virtual void onSGCM(const Context &context, qint32 msgId, const QString &sigName, const QString &comment) = 0;
    virtual void onEVCM(const Context &context, const QString &envVarName, const QString &comment) = 0;

    //BA_DEF_
    virtual void onIntAttrDef(const Context &context, Document::ObjectType objectType, const QString &attrName,
                              qint32 minimum, qint32 maximum) = 0;
    virtual void onHexAttrDef(const Context &context, Document::ObjectType objectType, const QString &attrName,
                              qint32 minimum, qint32 maximum) = 0;
    virtual void onFloatAttrDef(const Context &context, Document::ObjectType objectType, const QString &attrName,
                                double minimum, double maximum) = 0;
    virtual void onStringAttrDef(const Context &context, Document::ObjectType objectType, const QString &attrName) = 0;
    virtual void onEnumAttrDef(const Context &context, Document::ObjectType objectType, const QString &attrName,
                                 QList<QString> attrValues) = 0;

    //BA_DEF_DEF_
    virtual void onIntAttrDefaultDef(const Context &context, const QString &attrName, qint32 value) = 0;
    virtual void onHexAttrDefaultDef(const Context &context, const QString &attrName, qint32 value) = 0;
    virtual void onFloatAttrDefaultDef(const Context &context, const QString &attrName, double value) = 0;
    virtual void onStringAttrDefaultDef(const Context &context, const QString &attrName, const QString &value) = 0;
    virtual void onEnumAttrDefaultDef(const Context &context, const QString &attrName, const QString &value) = 0;

    //BA_
    virtual void onBUAttributeValue(const Context &context, const QString &nodeName, const QString &attrName, qint32 value) = 0;
    virtual void onBUAttributeValue(const Context &context, const QString &nodeName, const QString &attrName, double value) = 0;
    virtual void onBUAttributeValue(const Context &context, const QString &nodeName, const QString &attrName, const QString &value) = 0;

    virtual void onBOAttributeValue(const Context &context, qint32 msgId, const QString &attrName, qint32 value) = 0;
    virtual void onBOAttributeValue(const Context &context, qint32 msgId, const QString &attrName, double value) = 0;
    virtual void onBOAttributeValue(const Context &context, qint32 msgId, const QString &attrName, const QString &value) = 0;

    virtual void onSGAttributeValue(const Context &context, qint32 msgId, const QString &sigName, const QString &attrName, qint32 value) = 0;
    virtual void onSGAttributeValue(const Context &context, qint32 msgId, const QString &sigName, const QString &attrName, double value) = 0;
    virtual void onSGAttributeValue(const Context &context, qint32 msgId, const QString &sigName, const QString &attrName, const QString &value) = 0;

    virtual void onEvAttributeValue(const Context &context, const QString &varName, const QString &attrName, qint32 value) = 0;
    virtual void onEvAttributeValue(const Context &context, const QString &varName, const QString &attrName, double value) = 0;
    virtual void onEvAttributeValue(const Context &context, const QString &varName, const QString &attrName, const QString &value) = 0;

    virtual void onGblAttributeValue(const Context &context, const QString &attrName, qint32 value) = 0;
    virtual void onGblAttributeValue(const Context &context, const QString &attrName, double value) = 0;
    virtual void onGblAttributeValue(const Context &context, const QString &attrName, const QString &value) = 0;

    virtual void onDone(const Context &context) = 0;
};

class DocumentBuilder : public Builder
{
    // Builder interface
public:
    void onVersion(const Context &context, const QString &version);
    void onNewSymbol(const Context &context, const QString &newSymbol);
    void onBS(const Context &context, quint32 baurate, quint32 btr1, quint32 btr2);
    void onBU(const Context &context, const QString &nodeName);
    void onValTable(const Context &context, const QString &valTableName, const QList<ValueTableEntry> &list);
    void onBO(const Context &context, qint32 msgId, const QString &name, qint32 size, const QString transmitter);
    void onSG(const Context &context, const QString &signalName, Signal::MultiplexerIndType mpIndicatorType, quint32 multiplexerSwithValue, qint32 startBit, qint32 signalSize, Signal::ByteOrderType byteOrder, Signal::ValueType valueType, double factor, double offset, double minimum, double maximum, const QString &unit, const QList<QString> receivers);
    void onBoTxBu(const Context &context, qint32 msgId, const QList<QString> transmitters);
    void onSigVal(const Context &context, qint32 msgId, const QString &signalName, const QList<ValueTableEntry> &list);

    void onEnvVal(const Context &context, const QString &envName, const QList<ValueTableEntry> &list);
    void onEv(const Context &context, const QString &envVarName, EV::EnvVarType envVarType, double minimum, double maximum, const QString &unit, double initValue, quint32 evId, EV::AccessType accessType, const QList<QString> &accessNodeList);
    void onEvData(const Context &context, const QString envVarName, quint32 dataSize);

    void onCm(const Context &context, const QString &globalComment);
    void onBUCM(const Context &context, const QString &nodeName, const QString &comment);
    void onBOCM(const Context &context, qint32 msgId, const QString &comment);
    void onSGCM(const Context &context, qint32 msgId, const QString &sigName, const QString &comment);
    void onEVCM(const Context &context, const QString &envVarName, const QString &comment);

    void onIntAttrDef(const Context &context, Document::ObjectType objectType, const QString &attrName, qint32 minimum, qint32 maximum);
    void onHexAttrDef(const Context &context, Document::ObjectType objectType, const QString &attrName, qint32 minimum, qint32 maximum);
    void onFloatAttrDef(const Context &context, Document::ObjectType objectType, const QString &attrName, double minimum, double maximum);
    void onStringAttrDef(const Context &context, Document::ObjectType objectType, const QString &attrName);
    void onEnumAttrDef(const Context &context, Document::ObjectType objectType, const QString &attrName, QList<QString> attrValues);

    void onIntAttrDefaultDef(const Context &context, const QString &attrName, qint32 value);
    void onHexAttrDefaultDef(const Context &context, const QString &attrName, qint32 value);
    void onFloatAttrDefaultDef(const Context &context, const QString &attrName, double value);
    void onStringAttrDefaultDef(const Context &context, const QString &attrName, const QString &value);
    void onEnumAttrDefaultDef(const Context &context, const QString &attrName, const QString &value);

    void onBUAttributeValue(const Context &context, const QString &nodeName, const QString &attrName, qint32 value);
    void onBUAttributeValue(const Context &context, const QString &nodeName, const QString &attrName, double value);
    void onBUAttributeValue(const Context &context, const QString &nodeName, const QString &attrName, const QString &value);

    void onBOAttributeValue(const Context &context, qint32 msgId, const QString &attrName, qint32 value);
    void onBOAttributeValue(const Context &context, qint32 msgId, const QString &attrName, double value);
    void onBOAttributeValue(const Context &context, qint32 msgId, const QString &attrName, const QString &value);

    void onSGAttributeValue(const Context &context, qint32 msgId, const QString &sigName, const QString &attrName, qint32 value);
    void onSGAttributeValue(const Context &context, qint32 msgId, const QString &sigName, const QString &attrName, double value);
    void onSGAttributeValue(const Context &context, qint32 msgId, const QString &sigName, const QString &attrName, const QString &value);

    void onEvAttributeValue(const Context &context, const QString &varName, const QString &attrName, qint32 value);
    void onEvAttributeValue(const Context &context, const QString &varName, const QString &attrName, double value);
    void onEvAttributeValue(const Context &context, const QString &varName, const QString &attrName, const QString &value);

    void onGblAttributeValue(const Context &context, const QString &attrName, qint32 value);
    void onGblAttributeValue(const Context &context, const QString &attrName, double value);
    void onGblAttributeValue(const Context &context, const QString &attrName, const QString &value);

    void onDone(const Context &context);
public:
    DocumentBuilder();
    Document *getDocument();

protected:
    Document *doc;
    Message *curMsg;
};

END_DBC4CPP_NAMESPACE

#endif // BUILDER_H
