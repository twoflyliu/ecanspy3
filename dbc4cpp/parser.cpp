#include "parser.h"

#include <QFile>


//=======================定义节点关键字=============================
#define SECTOR_VERSION                  "VERSION"
#define SECTOR_NEW_SYMBOL               "NS_"
#define SECTOR_BIT_SET                  "BS_"

#define SECTOR_NODE                     "BU_"
#define SECTOR_DEFAULT_NODE             "Vector__XXX"

#define SECTOR_VAL_TABLE                "VAL_TABLE_"
#define SECTOR_MESSAGE                  "BO_"
#define SECTOR_SIGNAL                   "SG_"

#define SECTOR_ATTR_DEFINE              "BA_DEF_"
#define SECTOR_ATTR_DEFAULT_DEFINE      "BA_DEF_DEF_"
#define SECTOR_ATTR_VAL                 "BA_"

#define SECTOR_VAL                      "VAL_"
#define SECTOR_BO_TX_BU                 "BO_TX_BU_"

#define SECTOR_EV                       "EV_"

#define SECTOR_COMMENT                  "CM_"

//=========================默认值==================================
#define DEFAULT_BAURATE                 (0)
#define DEFAULT_BTR1                    (0)
#define DEFAULT_BTR2                    (0)

//=========================特定值==================================
#define ATTR_VALUE_TYPE_INT             ("INT")
#define ATTR_VALUE_TYPE_HEX             ("HEX")
#define ATTR_VALUE_TYPE_FLOAT           ("FLOAT")
#define ATTR_VALUE_TYPE_STRING          ("STRING")
#define ATTR_VALUE_TYPE_ENUM            ("ENUM")

/*!
 * \brief endingErrMsg 检测错误是否是行尾错误
 * \param errMsg
 * \retval 错误是否是行尾错误
 */
static inline bool endingErrMsg(const Context &context)
{
    auto &errMsg = context.getErrMsg();
    return (-1 != errMsg.indexOf("eol")) || (-1 != errMsg.indexOf("eof"));
}

ParserBase::ParserBase()
{

}

const QString &ParserBase::getErrMsg() const
{
    if (!context) {
        return context.getErrMsg();
    }
    return EMPTY_STRING;
}

bool ParserBase::parse(const QString &filePath, Builder &builder)
{
    context.setDBCFileName(filePath);
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        context.setErrMsg(file.errorString());
        return false;
    }

    QTextStream in(&file);
    in.setCodec(getCodec().toAscii().data());

    context.setCurLine("");
    context.setCurLineNo(0);
    context.setCurColNo(0);
    context.clrErrMsg();

    content = in.readAll();
    cursor = 0;

    return onParse(builder);
}

QChar ParserBase::getChar()
{
    QChar ch;
    if (!isEnd()) {
        ch = content.at(cursor);
        ++cursor;

        if ('\n' == ch) {
            context.moveNextLine();
        } else {
            context.moveNextCol();
        }
    }
    return ch;
}

void ParserBase::putChar()
{
    --cursor;

    if (cursor > 0) {
        if ('\n' == content.at(cursor)) {
            context.movePrevLineLastPos();
        }
    }
}

bool ParserBase::isEnd()
{
    return cursor >= content.size();
}


bool DBCParser::onParse(Builder &builder)
{
    QString identifier;
    while (getNextIdentifier(identifier, false)) {
        auto iter = parseMethodMap.find(identifier);
        if (iter != parseMethodMap.end()) {
            ParseMethod method = iter.value();
            if (!(this->*method)(builder)) {
                return false;
            }
        } else {
            context.setErrMsg(QString("Sector [%1] in (Line:%2,Col:%3) is not supported!")
                              .arg(identifier).arg(context.getCurLineNo()).arg(context.getCurColNo()));
            return false;
        }
    }

    bool ret = static_cast<bool>(context);
    if (ret) {
        builder.onDone(context);
    }

    return ret;
}

DBCParser::DBCParser() : ParserBase()
{
    parseMethodMap.insert(SECTOR_VERSION, &DBCParser::parseVersion);
    parseMethodMap.insert(SECTOR_NEW_SYMBOL, &DBCParser::parseNewSymbol);
    parseMethodMap.insert(SECTOR_BIT_SET, &DBCParser::parseBitSetting);
    parseMethodMap.insert(SECTOR_NODE, &DBCParser::parseNode);
    parseMethodMap.insert(SECTOR_VAL_TABLE, &DBCParser::parseValueTable);
    parseMethodMap.insert(SECTOR_MESSAGE, &DBCParser::parseMessage);
    parseMethodMap.insert(SECTOR_SIGNAL, &DBCParser::parseSignal);
    parseMethodMap.insert(SECTOR_VAL, &DBCParser::parseValue);
    parseMethodMap.insert(SECTOR_BO_TX_BU, &DBCParser::parseBoTxBu);

    parseMethodMap.insert(SECTOR_ATTR_DEFINE, &DBCParser::parseAttrDefine);
    parseMethodMap.insert(SECTOR_ATTR_DEFAULT_DEFINE, &DBCParser::parseAttrDefaultDefine);
    parseMethodMap.insert(SECTOR_ATTR_VAL, &DBCParser::parseAttrValue);
    parseMethodMap.insert(SECTOR_COMMENT, &DBCParser::parseComment);

    //EV_, EV_DATA_未解析
    //SIGTYPE_, SIG_GROUP_未解析
}

bool DBCParser::parseVersion(Builder &builder)
{
    QString version;
    if (getNextString(version)) {
        builder.onVersion(context, version);
        return true;
    }
    return false;
}

bool DBCParser::parseNewSymbol(Builder &builder)
{
    // 获取分号
    if (!getNextColon()) {
        return false;
    }

    // 获取Symbol
    QString newSymbol;

    if (getNextNewSymbol(newSymbol)) {
        builder.onNewSymbol(context, newSymbol);
        while (getNextNewSymbol(newSymbol)) {
            builder.onNewSymbol(context, newSymbol);
        }
        return static_cast<bool>(context);
    } else {
        context.setErrMsg(QString("NS_ sector missing new symbol"));
        return false;
    }
}

bool DBCParser::parseBitSetting(Builder &builder)
{
    //获取分号
    if (!getNextColon()) {
        return false;
    }

    //解析参数
    quint32 baurate = DEFAULT_BAURATE;
    quint32 btr1 = DEFAULT_BTR1;
    quint32 btr2 = DEFAULT_BTR2;

    if (getNextUnsignedInt(baurate)) {
        if (getNextColon()
                && getNextUnsignedInt(btr1)
                && getNextComma()
                && getNextUnsignedInt(btr2)) {

        } else {
            return false;
        }
    } else { //证明BS_为空
        context.clrErrMsg();
    }

    builder.onBS(context, baurate, btr1, btr2);
    return true;
}

bool DBCParser::parseNode(Builder &builder)
{
    //获取分号
    if (!getNextColon()) {
        return false;
    }

    //获取节点列表
    QString node;
    if (getNextIdentifier(node)) {
        builder.onBU(context, node);
        while (getNextIdentifier(node)) {
            builder.onBU(context, node);
        }

        auto &msg = context.getErrMsg();
        if (-1 != msg.indexOf("NL") || -1 != msg.indexOf("eol")) { //行尾，或者新行都不算是错误
            context.clrErrMsg();
            return true;
        } else {
            return false;
        }

    } else {
        return false;
    }
}

bool DBCParser::parseMessage(Builder &builder)
{
    quint32 id;
    if (!getNextUnsignedInt(id)) {          //获取报文id
        return false;
    }

    QString name;
    if (!getNextIdentifier(name)) {         //获取报文名称
        return false;
    }

    if (!getNextColon()) {                  //获取冒号
        return false;
    }

    quint32 len;
    if (!getNextUnsignedInt(len)) {         //获取报文长度
        return false;
    }

    QString transmitter;
    if (!getNextIdentifier(transmitter)) {  //获取发送者
        return false;
    }

    builder.onBO(context, id, name, len, transmitter);
    return true;
}

bool DBCParser::parseSignal(Builder &builder)
{
    QString name;
    if (!getNextIdentifier(name)) {         //解析信号名称
        return false;
    }

    if (!getNextColon()) {                  //获取:
        return false;
    }

    // 解析多路复用指示
    Signal::MultiplexerIndType type = Signal::MultiplexerIndType::MULTIPLEXER_IND_TYPE_NONE;
    quint32 value = 0;

    if (getNextChar(QChar('M'), "M")) {     //获取M
        type = Signal::MultiplexerIndType::MULTIPLEXER_IND_TYPE_SWITCH;
    } else {
        putChar();
    }

    if (getNextChar(QChar('m'), "m")) {     //获取m
        type = Signal::MultiplexerIndType::MULTIPLEXER_IND_TYPE_VALUE;
        if (!getNextUnsignedInt(value)) {
            return false;
        }
    } else {
        putChar();
    }

    quint32 startBit = 0;
    if (!getNextUnsignedInt(startBit)) {    //获取起始位
        return false;
    }

    if (!getNextChar(QChar('|'), "'|'")) { //获取|
        return false;
    }

    quint32 bitLen = 0;
    if (!getNextUnsignedInt(bitLen)) {      //获取信号长度
        return false;
    }

    if (!getNextChar(QChar('@'), "@")) {    //获取@
        return false;
    }

    Signal::ByteOrderType byteOrder = Signal::ByteOrderType::BYTE_ORDER_TYPE_MOTOROLA;;
    QChar ch;

    // 获取字节序
    if (!getNextChar(QString("01"), QString("Byte Order"), ch)) {
        return false;
    } else {
        if (QChar('0') == ch) {
            byteOrder = Signal::ByteOrderType::BYTE_ORDER_TYPE_MOTOROLA;
        } else if (QChar('1') == ch) {
            byteOrder = Signal::ByteOrderType::BYTE_ORDER_TYPE_INTEL;
        }
    }

    // 获取符号类型
    Signal::ValueType valueType = Signal::ValueType::VALUE_TYPE_UNSIGNED;
    if (!getNextChar(QString("+-"), QString("Sign"), ch)) {
        return false;
    } else {
        if (QChar('+') == ch) {
            valueType = Signal::ValueType::VALUE_TYPE_UNSIGNED;
        } else if (QChar('-') == ch) {
            valueType = Signal::ValueType::VALUE_TYPE_SIGNED;
        }
    }

    if (!getNextChar(QChar('('), "'('")) {  // 获取左括号
        return false;
    }

    double factor = 1.0;
    double offset = 0.0;
    if (!getNextDouble(factor)) {           // 获取精度
        return false;
    }

    if (!getNextComma()) {                  // 获取逗号
        return false;
    }

    if (!getNextDouble(offset)) {           // 获取偏移值
        return false;
    }

    if (!getNextChar(')', "')'")) {         // 获取右括号
        return false;
    }

    double minimum = 0.0;
    double maximum = 0.0;

    if (!getNextChar(QChar('['), "'['")) {
        return false;
    }

    if (!getNextDouble(minimum)) {          // 获取最小值
        return false;
    }

    if (!getNextChar(QChar('|'), "'|'")) {  // 获取|
        return false;
    }

    if (!getNextDouble(maximum)) {          // 获取最大值
        return false;
    }

    if (!getNextChar(QChar(']'), "']'")) {
        return false;
    }

    QString unit;
    if (!getNextString(unit)) {             // 获取单位
        return false;
    }

    QList<QString> receivers;
    QString receiver;
    if (!getNextIdentifier(receiver)) {
        return false;
    } else {
        receivers.push_back(receiver);
        while (getNextChar(QChar(','), "','")) {
            if (!getNextIdentifier(receiver)) {
                return false;
            }
            receivers.push_back(receiver);
        }
        context.clrErrMsg();
    }

    builder.onSG(context, name, type, value, startBit, bitLen, byteOrder, valueType,
                 factor, offset, minimum, maximum, unit, receivers);
    return true;
}

bool DBCParser::parseValueTable(Builder &builder)
{
    QString name;

    if (!getNextIdentifier(name)) { //获取值表名称
        return false;
    }

    QList<Builder::ValueTableEntry> entryList;
    if (!doParseValueDesc(entryList)) {
        return false;
    }

    builder.onValTable(context, name, entryList);
    return true;
}

bool DBCParser::parseAttrDefine(Builder &builder)
{
    QString objectTypeStr;
    QString attrName;

    Document::ObjectType objectType = Document::ObjectType::OBJECT_TYPE_GL;
    if (!getNextIdentifier(objectTypeStr)) { //全局属性
        putChar();
    } else {
        if (SECTOR_NODE == objectTypeStr) {
            objectType = Document::ObjectType::OBJECT_TYPE_BU;
        } else if (SECTOR_MESSAGE == objectTypeStr) {
            objectType = Document::ObjectType::OBJECT_TYPE_BO;
        } else if (SECTOR_SIGNAL == objectTypeStr) {
            objectType = Document::ObjectType::OBJECT_TYPE_SG;
        } else if (SECTOR_EV == objectTypeStr) {
            objectType = Document::ObjectType::OBJECT_TYPE_EV;
        } else {
           context.setErrMsg(QString("[BA_]: unknown object type (%1) in (Line:%2,Col:%3)")
                             .arg(objectTypeStr).arg(context.getCurLineNo()).arg(context.getCurLineNo()));
           return false;
        }
    }

    if (!getNextString(attrName)) {
        return false;
    }

    QString valType;
    if (!getNextIdentifier(valType)) {
        return false;
    }

    if (valType == ATTR_VALUE_TYPE_INT || valType == ATTR_VALUE_TYPE_HEX) {// INT类型, HEX类型
        qint32 minimum = 0;
        qint32 maximum = 0;
        if (!getNextInt(minimum)) {
            return false;
        }
        if (!getNextInt(maximum)) {
            return false;
        }
        if (!getNextSemicolon()) {
            return false;
        }

        if (valType == ATTR_VALUE_TYPE_INT) {
            builder.onIntAttrDef(context, objectType, attrName, minimum, maximum);
            return true;
        } else {
            builder.onHexAttrDef(context, objectType, attrName, minimum, maximum);
            return true;
        }
    } else if (valType == ATTR_VALUE_TYPE_FLOAT) { // FLOAT类型
        double minimum = 0.0;
        double maximum = 0.0;
        if (!getNextDouble(minimum)) {
            return false;
        }
        if (!getNextDouble(maximum)) {
            return false;
        }
        if (!getNextSemicolon()) {
            return false;
        }
        builder.onFloatAttrDef(context, objectType, attrName, minimum, maximum);
        return true;
    } else if (valType == ATTR_VALUE_TYPE_STRING) { // 字符串类型
        if (!getNextSemicolon()) {
            return false;
        }
        builder.onStringAttrDef(context, objectType, attrName); //属性不需要值
        return true;
    } else if (valType == ATTR_VALUE_TYPE_ENUM) { // ENUMl类型
        QList<QString> valList;
        QString val;
        if (!getNextString(val)) {
            return false;
        }
        valList.push_back(val);
        val.clear();

        while (getNextComma()) {
            if (!getNextString(val)) {
                return false;
            }
            valList.push_back(val);
            val.clear();
        }

        if (endingErrMsg(context)) {
            return false;
        }
        putChar();

        if (!getNextSemicolon()) {
            return false;
        }
        builder.onEnumAttrDef(context, objectType, attrName, valList);
        return true;
    } else {
        context.setErrMsg(QString("[BA_]: Invalid value type -> %1").arg(valType));
        return false;
    }
}

bool DBCParser::parseAttrDefaultDefine(Builder &builder)
{
    QString attrName;

    if (!getNextString(attrName)) {
        return false;
    }

    bool iret, dret, sret;
    qint32 ival;
    double dval;
    QString sval;
    if (!getNextValue(iret, ival, dret, dval, sret, sval)) {
        return false;
    }
    if (!getNextSemicolon()) {
        return false;
    }

    if (iret) {
        builder.onIntAttrDefaultDef(context, attrName, ival);
    } else if (dret) {
        builder.onFloatAttrDefaultDef(context, attrName, dval);
    } else if (sret) {
        builder.onStringAttrDefaultDef(context, attrName, sval);
    } else {
        context.setErrMsg(QString("[BA_DEF_DEF_]: invalid value"));
        return false;
    }
    return true;
}

bool DBCParser::parseAttrValue(Builder &builder)
{
    QString attrName;
    if (!getNextString(attrName)) {
        return false;
    }

    QString objectTypeStr;
    if (!getNextIdentifier(objectTypeStr)) {
        context.clrErrMsg();
        putChar(); //全局的属性
    }

    bool iret, dret, sret;
    qint32 ival;
    double dval;
    QString sval;
    if (SECTOR_NODE == objectTypeStr) {
        QString nodeName;
        if (!getNextIdentifier(nodeName)) {
            return false;
        }
        if (!getNextValue(iret, ival, dret, dval, sret, sval)) {
            return false;
        }
        if (!getNextSemicolon()) {
            return false;
        }
        if (iret) {
            builder.onBUAttributeValue(context, nodeName, attrName, ival);
        } else if (dret) {
            builder.onBUAttributeValue(context, nodeName, attrName, dval);
        } else if (sret) {
            builder.onBUAttributeValue(context, nodeName, attrName, sval);
        }
        return true;
    } else if (SECTOR_MESSAGE == objectTypeStr) {
        quint32 id;
        if (!getNextUnsignedInt(id)) {
            return false;
        }
        if (!getNextValue(iret, ival, dret, dval, sret, sval)) {
            return false;
        }
        if (!getNextSemicolon()) {
            return false;
        }
        if (iret) {
            builder.onBOAttributeValue(context, id, attrName, ival);
        } else if (dret) {
            builder.onBOAttributeValue(context, id, attrName, dval);
        } else if (sret) {
             builder.onBOAttributeValue(context, id, attrName, sval);
        }
        return true;
    } else if (SECTOR_SIGNAL == objectTypeStr) {
        quint32 id;
        QString sigName;
        if (!getNextUnsignedInt(id)) {
            return false;
        }
        if (!getNextIdentifier(sigName)) {
            return false;
        }
        if (!getNextValue(iret, ival, dret, dval, sret, sval)) {
            return false;
        }
        if (!getNextSemicolon()) {
            return false;
        }
        if (iret) {
            builder.onSGAttributeValue(context, id, sigName, attrName, ival);
        } else if (dret) {
            builder.onSGAttributeValue(context, id, sigName, attrName, dval);
        } else if (sret) {
            builder.onSGAttributeValue(context, id, sigName, attrName, sval);
        }
        return true;
    } else if (SECTOR_EV == objectTypeStr) {
        QString varName;
        if (!getNextIdentifier(varName)) {
            return false;
        }
        if (!getNextValue(iret, ival, dret, dval, sret, sval)) {
            return false;
        }
        if (!getNextSemicolon()) {
            return false;
        }
        if (iret) {
            builder.onEvAttributeValue(context, varName, attrName, ival);
        } else if (dret) {
            builder.onEvAttributeValue(context, varName, attrName, dval);
        } else if (sret) {
            builder.onEvAttributeValue(context, varName, attrName, sval);
        }
        return true;
    } else { //全局属性
        if (!getNextValue(iret, ival, dret, dval, sret, sval)) {
            return false;
        }
        if (!getNextSemicolon()) {
            return false;
        }
        if (iret) {
            builder.onGblAttributeValue(context, attrName, ival);
        } else if (dret) {
            builder.onGblAttributeValue(context, attrName, dval);
        } else if (sret) {
            builder.onGblAttributeValue(context, attrName, sval);
        }
        return true;
    }
}

bool DBCParser::parseValue(Builder &builder)
{
    QString envName;
    QList<Builder::ValueTableEntry> valueList;
    if (getNextString(envName)) { //值描述是环境变量值描述
        if (!doParseValueDesc(valueList)) {
            return false;
        }
        builder.onEnvVal(context, envName, valueList);
    } else {
        putChar();
        quint32 id;
        QString sigName;
        if (!getNextUnsignedInt(id)) {
            return false;
        }
        if (!getNextIdentifier(sigName)) {
            return false;
        }
        if (!doParseValueDesc(valueList)) {
            return false;
        }
        builder.onSigVal(context, id, sigName, valueList);
    }
    return true;
}

bool DBCParser::parseBoTxBu(Builder &builder)
{
    quint32 id;
    if (!getNextUnsignedInt(id)) {
        return false;
    }
    if (!getNextColon()) {
        return false;
    }

    QList<QString> transmitters;
    QString transmitter;

    if (!getNextIdentifier(transmitter)) {
        return false;
    }
    transmitters.push_back(transmitter);

    while (getNextComma()) {
        if (!getNextIdentifier(transmitter)) {
            return false;
        }
        transmitters.push_back(transmitter);
    }
    putChar();

    if (!getNextSemicolon()) {
        return false;
    }

    builder.onBoTxBu(context, id, transmitters);
    return true;
}

bool DBCParser::parseComment(Builder &builder)
{
    QString objectType;
    QString comment;

    if (!getNextIdentifier(objectType)) {
        putChar();
        if (!getNextString(comment)) {
            return false;
        }
        if (!getNextSemicolon()) {
            return false;
        }
        builder.onCm(context, comment);
        return true;
    } else if (objectType == SECTOR_NODE) {
        QString nodeName;
        if (!getNextIdentifier(nodeName)) {
            return false;
        }
        if (!getNextString(comment)) {
            return false;
        }
        if (!getNextSemicolon()) {
            return false;
        }
        builder.onBUCM(context, nodeName, comment);
        return true;
    } else if (objectType == SECTOR_MESSAGE) {
        quint32 id;
        if (!getNextUnsignedInt(id)) {
            return false;
        }
        if (!getNextString(comment)) {
            return false;
        }
        if (!getNextSemicolon()) {
            return false;
        }
        builder.onBOCM(context, id, comment);
        return true;
    } else if (objectType == SECTOR_SIGNAL) {
        quint32 id;
        if (!getNextUnsignedInt(id)) {
            return false;
        }
        QString sigName;
        if (!getNextIdentifier(sigName)) {
            return false;
        }
        if (!getNextString(comment)) {
            return false;
        }
        if (!getNextSemicolon()) {
            return false;
        }
        builder.onSGCM(context, id, sigName, comment);
        return true;
    } else if (objectType == SECTOR_EV) {
        QString evName;
        if (!getNextIdentifier(evName)) {
            return false;
        }
        if (!getNextString(comment)) {
            return false;
        }
        if (!getNextSemicolon()) {
            return false;
        }
        builder.onEVCM(context, evName, comment);
        return true;
    } else {
        context.setErrMsg(QString("[CM_]: Unknown object type(%1)").arg(objectType));
        return false;
    }
}

bool DBCParser::doParseValueDesc(QList<Builder::ValueTableEntry> &entryList)
{
    Builder::ValueTableEntry entry;
    quint32 val;
    QString desc;

    // 必须有一个值-描述
    if (!getNextUnsignedInt(val)) {
        return false;
    } else {
        if (!getNextString(desc)) {
            return false;
        }
        entry.value = val;
        entry.desc = desc;
        entryList.push_back(entry);
    }

    // 解析剩余的值描述
    while (getNextUnsignedInt(val)) {
        if (!getNextString(desc)) {
            return false;
        }
        entry.value = val;
        entry.desc = desc;
        entryList.push_back(entry);
    }

    // 值描述结尾有';'
    putChar();
    context.clrErrMsg();
    if (!getNextSemicolon()) {
        return false;
    }
    return true;
}

bool DBCParser::getNextIdentifier(QString &nextIdentifier, bool eol)
{
    enum {
        IDENTIFIER_STATE_SPACE = 0,
        IDENTIFIER_STATE_UND_ALNUM,
    } state = IDENTIFIER_STATE_SPACE;

    QChar ch;
    QChar newLine('\n');
    nextIdentifier.clear();

    while (!isEnd()) {
        ch = getChar();
        switch (state) {
        case IDENTIFIER_STATE_SPACE:
            if (ch.isLetter() || (QChar('_') == ch)) {
                nextIdentifier.append(ch);
                state = IDENTIFIER_STATE_UND_ALNUM;
            } else if (ch.isNumber()) {
                context.setErrMsg(QString("[getNextIdentifier]: Identifier in (Line:%1,Col:%2) cannot start with number")
                                  .arg(context.getCurLineNo()).arg(context.getCurColNo()));
                return false;
            } else if (!ch.isSpace()) {
                context.setErrMsg(QString("[getNextIdentifier]: expect get alpha or underline, but got '%1' in (Line:%2, Col:%3)")
                                  .arg(ch).arg(context.getCurLineNo()).arg(context.getCurColNo()));
                return false;
            } else if (eol && (newLine == ch)) {
                context.setErrMsg(QString("[getNextIdentifier]: Expect get alpha or underline identifier heading, but got new Line(NL) in (Line:%1, Col:%2)")
                                  .arg(context.getCurLineNo()).arg(context.getCurColNo()));
                return false;
            }
            break;
        case IDENTIFIER_STATE_UND_ALNUM:
            if (ch.isLetterOrNumber() || (QChar('_') == ch)) {
                nextIdentifier.append(ch);
            } else {
                putChar(); //把标识符后面的空白字符put回去
                return true;
            }
            break;
        }
    }

    //context.setErrMsg(QString("[getNextIdentifier]: Expect to get identifier, but got eof"));
    return false;
}

bool DBCParser::getNextString(QString &nextString)
{
    enum {
        STRING_STATE_LEFT_DBL_QUOTE = 0,
        STRING_STATE_CONTENT
    } state = STRING_STATE_LEFT_DBL_QUOTE;

    const QChar quote('\"');

    QChar ch;
    nextString.clear();

    while (!isEnd()) {
        ch = getChar();
        switch (state) {
        case STRING_STATE_LEFT_DBL_QUOTE:
            if (quote == ch) {
                state = STRING_STATE_CONTENT;
            } else if (!ch.isSpace()) {
                context.setErrMsg(QString("Expect \", but got %1 in (Line:%2,Col:%3)")
                                  .arg(ch).arg(context.getCurLineNo()).arg(context.getCurColNo()));
                return false;
            }
            break;
        case STRING_STATE_CONTENT:
            if (quote != ch) {
                nextString.append(ch);
            } else {
                return true;
            }
            break;
        default:
            break;
        }
    }

    context.setErrMsg(QString("expect to got \", but got eol"));
    return false;
}

bool DBCParser::getNextSemicolon()
{
    return getNextChar(QChar(';'), "Semicolon");
}

bool DBCParser::getNextColon()
{
    return getNextChar(QChar(':'), "Colon");
}

bool DBCParser::getNextComma()
{
    return getNextChar(QChar(','), "Comma");
}

bool DBCParser::getNextInt(qint32 &intVal, bool eol)
{
    enum {
        INT_STATE_SPACE = 0,
        INT_STATE_NUM
    } state = INT_STATE_SPACE;

    QChar ch;
    QString str;
    const QChar newLine('\n');

    while (!isEnd()) {
        ch = getChar();
        switch (state) {
        case INT_STATE_SPACE:
            if (ch.isNumber() || (QChar('-') == ch) || (QChar('+') == ch)) {
                str.append(ch);
                state = INT_STATE_NUM;
            } else if (!ch.isSpace()) {
                context.setErrMsg(QString("Unsigned integer cannot contains non number(%1, which in [Line:%2:Col:%3]) char.")
                                  .arg(ch).arg(context.getCurLineNo()).arg(context.getCurColNo()));
                return false;
            } else if (eol && (newLine == ch)) {
                context.setErrMsg(QString("Expect got int, but got eol"));
                return false;
            }
            break;
        case INT_STATE_NUM:
            if (ch.isNumber()) {
                str.append(ch);
            } else {
                intVal = str.toInt();
                putChar();
                return true;
            }
            break;
        }
    }

    context.setErrMsg(QString("Expect got unsigned integer, but got eof"));
    return false;
}

bool DBCParser::getNextUnsignedInt(quint32 &uintVal, bool eol)
{
    enum {
        UNSIGNED_INT_STATE_SPACE = 0,
        UNSIGNED_INT_STATE_NUM
    } state = UNSIGNED_INT_STATE_SPACE;

    QChar ch;
    QString str;

    while (!isEnd()) {
        ch = getChar();
        switch (state) {
        case UNSIGNED_INT_STATE_SPACE:
            if (ch.isNumber()) {
                str.append(ch);
                state = UNSIGNED_INT_STATE_NUM;
            } else if (!ch.isSpace()) {
                context.setErrMsg(QString("Unsigned integer cannot contains non number(%1, which in [Line:%2:Col:%3]) char.")
                                  .arg(ch).arg(context.getCurLineNo()).arg(context.getCurColNo()));
                return false;
            } else if (eol && (ch == QChar('\n'))) {
                context.setErrMsg(QString("Expect got int, but got eol"));
                return false;
            }
            break;
        case UNSIGNED_INT_STATE_NUM:
            if (ch.isNumber()) {
                str.append(ch);
            } else {
                uintVal = str.toUInt();
                putChar();
                return true;
            }
            break;
        }
    }

    context.setErrMsg(QString("Expect got unsigned integer, but got eof"));
    return false;
}

static inline bool isDoubleChar(QChar ch)
{
    return (ch.isNumber() || QChar('.') == ch \
            || QChar('-') == ch || QChar('+') == ch);
}

static inline bool isNumAlphChar(QChar ch)
{
    return (ch.isNumber() || QChar('-') == ch\
            || QChar('+') == ch);
}

bool DBCParser::getNextDouble(double &doubleVal, bool eol)
{
    enum {
        NEXT_DOUBLE_STATE_SPACE = 0,
        NEXT_DOUBLE_STATE_ALNUM
    } state = NEXT_DOUBLE_STATE_SPACE;

    QChar ch;
    QString str;
    const QChar newLine('\n');
    while (!isEnd()) {
        ch = getChar();
        switch (state) {
        case NEXT_DOUBLE_STATE_SPACE:
            if (isDoubleChar(ch)) {
                str.append(ch);
                state = NEXT_DOUBLE_STATE_ALNUM;
            } else if (!ch.isSpace()) {
                context.setErrMsg(QString("Expect got double value, but got '%1' char.")
                                  .arg(ch));
                return false;
            } else if (eol && (newLine == ch)) {
                context.setErrMsg(QString("Expect got double value, but got eol."));
                return false;
            }
            break;
        case NEXT_DOUBLE_STATE_ALNUM:
            if (isDoubleChar(ch)) {
                str.append(ch);
            } else {
                putChar();
                doubleVal = str.toDouble();
                return true;
            }
            break;
        }
    }

    context.setErrMsg(QString("Expect got double value, but got eol."));
    return false;
}

bool DBCParser::getNextNewSymbol(QString &newSymbol)
{
    enum {
        NEW_SYMBOL_STATE_SPACE = 0,
        NEW_SYMBOL_STATE_UND_ALNUM,
    } state = NEW_SYMBOL_STATE_SPACE;

    QChar ch;
    QChar newLine('\n');
    int newLineCnt = 0;
    newSymbol.clear();

    while (!isEnd()) {
        ch = getChar();
        switch (state) {
        case NEW_SYMBOL_STATE_SPACE:
            if (ch.isLetter() || (QChar('_') == ch)) {
                newSymbol.append(ch);
                state = NEW_SYMBOL_STATE_UND_ALNUM;
            } else if (ch.isNumber()) {
                context.setErrMsg(QString("Identifier in (Line:%1,Col:%2) cannot start with number")
                                  .arg(context.getCurLineNo()).arg(context.getCurColNo()));
                return false;
            } else if (ch == newLine){
                if (++newLineCnt >= 2) {  //检测到空行，则表示结束
                    return false;
                }
            }
            break;
        case NEW_SYMBOL_STATE_UND_ALNUM:
            if (ch.isLetterOrNumber() || (QChar('_') == ch)) {
                newSymbol.append(ch);
            } else {
                putChar();
                return true;
            }
            break;
        }
    }

    context.setErrMsg(QString("[NS_]: got eof"));
    return false;
}

bool DBCParser::getNextChar(QChar mch, QString desc, bool eol)
{
    QChar ch;
    const QChar newLine('\n');

    while (!isEnd()) {
        ch = getChar();
        if (ch == mch) {
            return true;
        } else if (!ch.isSpace()) {
            context.setErrMsg(QString("Expect got %1, but got %2 in (Line:%3,Col:%4)")
                              .arg(desc).arg(ch).arg(context.getCurLineNo()).arg(context.getCurColNo()));
            return false;
        } else if (newLine == ch){
            if (eol) {
                context.setErrMsg(QString("Expect got %1, but got eol in (Line:%2, Col:%3)")
                                  .arg(desc).arg(context.getCurLineNo()).arg(context.getCurColNo()));
                return false;
            }
        }
    }

    context.setErrMsg(QString("Expect got %1, but got eof").arg(desc));
    return false;
}

bool DBCParser::getNextChar(const QString set, const QString &desc, QChar &out, bool eol)
{
    QChar ch;
    const QChar newLine('\n');

    while (!isEnd()) {
        ch = getChar();
        if (set.indexOf(ch) != -1) {
            out = ch;
            return true;
        } else if (!ch.isSpace()) {
            context.setErrMsg(QString("Expect got %1, but got %2 in (Line:%3,Col:%4)")
                              .arg(desc).arg(ch).arg(context.getCurLineNo()).arg(context.getCurColNo()));
            return false;
        } else if (newLine == ch){
            if (eol) {
                context.setErrMsg(QString("Expect got %1, but got eol in (Line:%2, Col:%3)")
                                  .arg(desc).arg(context.getCurLineNo()).arg(context.getCurColNo()));
                return false;
            }
        }
    }

    context.setErrMsg(QString("Expect got %1, but got eof").arg(desc));
    return false;
}

bool DBCParser::getNextValue(bool &iret, qint32 &ival, bool &dret, double &dval, bool &sret, QString &sval)
{
    iret = false;
    dret = false;
    sret = false;

    // 尝试按照字符串解析
    if (getNextString(sval)) {
        sret = true;
        return true;
    }
    putChar();

    // 解析整形数据和double类型数据
    enum {
        NEXT_VALUE_STATE_SPACE = 0,
        NEXT_VALUE_STATE_ALNUM
    } state = NEXT_VALUE_STATE_SPACE;
    QChar ch;
    QString str;
    bool isInt = true;

    while (!isEnd()) {
        ch = getChar();
        switch (state) {
        case NEXT_VALUE_STATE_SPACE:
            if (isDoubleChar(ch)) {
                if (ch == QChar('.')) {
                    isInt = false;
                }
                str.append(ch);
                state = NEXT_VALUE_STATE_ALNUM;
            } else if (ch.isPrint()) {
                context.setErrMsg(QString("Expect got double or int value, but got '%1' char.")
                                  .arg(ch));
                return false;
            }
            break;
        case NEXT_VALUE_STATE_ALNUM:
            if (isDoubleChar(ch)) {
                if (ch == QChar('.')) {
                    isInt = false;
                }
                str.append(ch);
            } else {
                putChar();
                if (isInt) {
                    ival = str.toInt();
                    iret = true;
                } else {
                    dval = str.toDouble();
                    dret = true;
                }
                return true;
            }
            break;
        }
    }

    context.setErrMsg(QString("Expect got double, int or string value, but got eol."));
    return false;
}
