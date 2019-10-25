#include <QCoreApplication>
#include <QTextCodec>
#include <QDebug>
#include <QRegExp>

#include <dbc4cpp/parser.h>

static void logAllAttributeDefaultValues(Document *doc, Document::ObjectType objectType, const QString &desc);
static void logAllAttributePrototypes(Document *doc, Document::ObjectType objectType, const QString &desc);

static void logAdditionalAttributeTabel(const AdditionalAttributeTable &tbl);

static void logAllNodeAttributeValues(Document *doc);
static void logAllMsgAttributeValues(Document *doc);
static void logAllSigAttributeValues(Document *doc);
static void logAllEvAttributeValues(Document *doc);

static void logAllSigValueDesc(Document *doc);
static void logAllEvValueDesc(Document *doc);

static void logValueDescTable(const ValueDescription &valueDesc);

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));
    QCoreApplication a(argc, argv);

    DBCParser parser;
    DocumentBuilder builder;

    //const QString dbcFile = QObject::tr("F:/bdstar/项目/FE-4KB/5.CAN/B.通信矩阵/FE_4KB-20190124.dbc");
    //const QString dbcFile = QObject::tr("F:/projects/cpp_projects/HostComputerProjects/CommonTool/com_code_gen/dbc/FE-3ZA_IHU.dbc");
    const QString dbcFile = QObject::tr("F:/bdstar/项目/T500/ICU/5.CAN/B.通信矩阵/A12国六 TNN4G15T 6AT MsgList_V1.1_ICU_20180621.dbc");

    if (parser.parse(dbcFile, builder)) {
        Document *doc = builder.getDocument();
        if (doc) {
            qDebug() << "Version:" << doc->getVersion();        //获取版本号
            qDebug() << "NS:" << doc->getNS();                  //获取所有新标签

            // 获取所有位定时参数
            quint32 baurate, btr1, btr2;
            doc->getBS(baurate, btr1, btr2);
            qDebug() << "BS:" << baurate << "," << btr1 << "," << btr2;

            // 获取
            qDebug() << "BU:" << doc->getBU();

            // 获取所有VALUE TABLE
            auto &valueTableMap = doc->getValueTableMap();
            qDebug() << "ValueTable: Total Size = " << valueTableMap.size();
            for (auto iter = valueTableMap.begin(); iter != valueTableMap.end(); ++iter) {
                qDebug() << "ValueTable:" << iter.key();
                logValueDescTable(*iter.value());
            }

            // 所有报文
            auto &nodeMap = doc->getNodeMap();
            for (auto iter = nodeMap.begin(); iter != nodeMap.end(); ++iter) {
                qDebug() << "Node:" << iter.key();
                Node* node = iter.value();
                auto &msgMap = node->getTxMsgMap();
                for (auto iter = msgMap.begin(); iter != msgMap.end(); ++iter) {
                    Message *msg = iter.value();
                    qDebug() << "-->ID:" << msg->getId() << ", Name:" << msg->getName()
                             << ", Len:" << msg->getByteLen() << ", Transmiter:"
                             << msg->getTransmitterSet();
                    auto &signalTable = msg->getSignalList();
                    for (auto iter = signalTable.begin(); iter != signalTable.end(); ++iter) {
                        auto signal = iter.value();
                        qDebug() << "---->" << "Name:" << signal->getName() << ", StartBit:"
                                 << signal->getStartBit() << ", BitLen:" << signal->getBitLen()
                                 << ", ByteOrder:" << signal->getByteOrder()
                                 << ", ValueType:" << signal->getValueType()
                                 << ", factor:" << signal->getFactor()
                                 << ", offset:" << signal->getOffset()
                                 << ", minimum:" << signal->getMinimum()
                                 << ", maximum:" << signal->getMaximum()
                                 << ", unit:" << signal->getUnit()
                                 << ", receivers:" << signal->getReceiverSet();
                    }
                    qDebug();
                }
            }

            // 获取所有属性定义
            logAllAttributePrototypes(doc, Document::OBJECT_TYPE_GL, "Global");
            logAllAttributePrototypes(doc, Document::OBJECT_TYPE_BU, "Node");
            logAllAttributePrototypes(doc, Document::OBJECT_TYPE_BO, "Message");
            logAllAttributePrototypes(doc, Document::OBJECT_TYPE_SG, "Signal");
            logAllAttributePrototypes(doc, Document::OBJECT_TYPE_EV, "EnvVar");


            // 获取所有属性默认值
            logAllAttributeDefaultValues(doc, Document::OBJECT_TYPE_GL, "Global");
            logAllAttributeDefaultValues(doc, Document::OBJECT_TYPE_BU, "Node");
            logAllAttributeDefaultValues(doc, Document::OBJECT_TYPE_BO, "Message");
            logAllAttributeDefaultValues(doc, Document::OBJECT_TYPE_SG, "Signal");
            logAllAttributeDefaultValues(doc, Document::OBJECT_TYPE_EV, "EnvVar");

            // 获取所有属性值
            logAllNodeAttributeValues(doc);
            logAllMsgAttributeValues(doc);
            logAllSigAttributeValues(doc);
            logAllEvAttributeValues(doc);

            // 获取所有值描述
            logAllSigValueDesc(doc);
            logAllEvValueDesc(doc);
        }

    } else {
        qDebug() << "Parse Error:" << parser.getErrMsg();
    }

    return a.exec();
}

static void logAllAttributePrototypes(Document *doc, Document::ObjectType objectType, const QString &desc)
{
    auto &tbl = doc->getAllObjectAttrDefaultValues()[objectType];
    auto &attrMap = tbl.getAdditionalAttributeMap();
    qint32 minimum = 0, maximum = 0;
    double minDbl = 0.0, maxDbl = 0.0;
    qDebug() << desc << " attribute prototype:";
    for (auto iter = attrMap.begin(); iter != attrMap.end(); ++iter) {
        auto &val = iter.value();
        switch (val.getValueType()) {
        case AdditionalAttribute::VALUE_TYPE_INT:
            val.getIntRange(minimum, maximum);
            qDebug() << "-->Name:" << iter.key() << ",type: Int, minimum:" << minimum
                     << ", maximum:" << maximum;
            break;
        case AdditionalAttribute::VALUE_TYPE_HEX:
            val.getHexRange(minimum, maximum);
            qDebug() << "-->Name:" << iter.key() << ",type: Hex, minimum:" << minimum
                     << ", maximum:" << maximum;
            break;
        case AdditionalAttribute::VALUE_TYPE_FLOAT:
            val.getFloatRange(minDbl, maxDbl);
            qDebug() << "-->Name:" << iter.key() << ",type: Float, minimum:" << minDbl
                     << ", maximum:" << maxDbl;
            break;
        case AdditionalAttribute::VALUE_TYPE_STRING:
            qDebug() << "-->Name:" << iter.key() << ",type: String, minimum:";
            break;
        case AdditionalAttribute::VALUE_TYPE_ENUM:
            qDebug() << "-->Name:" << iter.key() << ",type: Enum, values:" << val.getEnumValues();
            break;
        default:
            qDebug() << "-->Name:" << iter.key() << "-> Unknown Types";
            break;
        }
    }
    qDebug();
}

static void logAdditionalAttributeTabel(const AdditionalAttributeTable &tbl)
{
    auto &attrMap = tbl.getAdditionalAttributeMap();
    for (auto iter = attrMap.begin(); iter != attrMap.end(); ++iter) {
        auto &val = iter.value();
        switch (val.getValueType()) {
        case AdditionalAttribute::VALUE_TYPE_INT:
            qDebug() << "-->" << iter.key() << "=" << val.getIntValue();
            break;
        case AdditionalAttribute::VALUE_TYPE_HEX:
            qDebug() << "-->" << iter.key() << "=" << val.getHexValue();
            break;
        case AdditionalAttribute::VALUE_TYPE_FLOAT:
            qDebug() << "-->" << iter.key() << "=" << val.getFloatValue();
            break;
        case AdditionalAttribute::VALUE_TYPE_STRING:
            qDebug() << "-->" << iter.key() << "=" << val.getStringValue();
            break;
        case AdditionalAttribute::VALUE_TYPE_ENUM:
            qDebug() << "-->" << iter.key() << "=" << val.getEnumValue();
            break;
        }
    }
}

static void logAllAttributeDefaultValues(Document *doc, Document::ObjectType objectType, const QString &desc)
{
    auto &tbl = doc->getAllObjectAttrDefaultValues()[objectType];
    qDebug() << desc << " attribute default values:";
    logAdditionalAttributeTabel(tbl);
    qDebug();
}

static void logAllNodeAttributeValues(Document *doc)
{
    auto &tblMap = doc->getAllNodeAttrValues();

    qDebug() << "node attribute values:total=" << tblMap.size();
    for (auto iter = tblMap.begin(); iter != tblMap.end(); ++iter) {
        qDebug() << "Node (" << iter.key() << ") attribute values:";
        logAdditionalAttributeTabel(iter.value());
        qDebug();
    }
    qDebug();
}

static void logAllMsgAttributeValues(Document *doc)
{
    auto &tblMap = doc->getAllMsgAttrValues();

    qDebug() << "msg attribute values:total=" << tblMap.size();
    for (auto iter = tblMap.begin(); iter != tblMap.end(); ++iter) {
        qDebug() << "Message (0x" << QByteArray::number(iter.key(), 16) << ") attribute values:";
        logAdditionalAttributeTabel(iter.value());
        qDebug();
    }
    qDebug();
}


static void logAllSigAttributeValues(Document *doc)
{
    auto &tblMap = doc->getAllSigAttrValues();

    qDebug() << "sig attribute values:total=" << tblMap.size();
    for (auto iter = tblMap.begin(); iter != tblMap.end(); ++iter) {
        qDebug() << "Signal (" << iter.key()->getName() << ") attribute values:";
        logAdditionalAttributeTabel(iter.value());
        qDebug();
    }
    qDebug();
}


static void logAllEvAttributeValues(Document *doc)
{
    auto &tblMap = doc->getAllEvAttrValues();

    qDebug() << "ev attribute values:total=" << tblMap.size();
    for (auto iter = tblMap.begin(); iter != tblMap.end(); ++iter) {
        qDebug() << "EV (" << iter.key() << ") attribute values:";
        logAdditionalAttributeTabel(iter.value());
        qDebug();
    }
    qDebug();
}

static void logValueDescTable(const ValueDescription &valueDesc)
{
    auto &tbl = valueDesc.getValueDescTable();
    for (auto iter = tbl.begin(); iter != tbl.end(); ++iter) {
        qDebug() << "-->" << iter.key() << "=" << iter.value();
    }
}

static void logAllSigValueDesc(Document *doc)
{
    auto &nodeMap = doc->getNodeMap();
    for (auto iter = nodeMap.begin(); iter != nodeMap.end(); ++iter) {
        Node *node = iter.value();
        auto &txMsgMap = node->getTxMsgMap();
        for (auto iter = txMsgMap.begin(); iter != txMsgMap.end(); ++iter) {
            auto &sigTbl = iter.value()->getSignalList();
            for (auto iter = sigTbl.begin(); iter != sigTbl.end(); ++iter) {
                auto signal = (*iter);
                qDebug() << "Signal (" << signal->getName() << ") Desc:";
                logValueDescTable((*signal));
                qDebug();
            }
        }
    }
}

static void logAllEvValueDesc(Document *doc)
{
    auto &evMap = doc->getEVMap();
    for (auto iter = evMap.begin(); iter != evMap.end(); ++iter) {
        qDebug() << "EvVar(" << iter.key() << ")";
        logValueDescTable(*iter.value());
        qDebug();
    }
}
