#ifndef ENTITIES_H
#define ENTITIES_H

#include "dbc4cpp_global.h"

#include <QSet>
#include <QMap>
#include <QStringList>

BEGIN_DBC4CPP_NAMESPACE

static QString EMPTY_STRING;

/*!
 * \brief Version类，代表DBC文件中的版本号
 */
class Version
{
public:
    Version(const QString &version="");

    /*!
     * \brief get 获取版本号
     * \retval 版本号
     */
    const QString &get() const;

    /*!
     * \brief set 设置版本号
     * \param version 要设置的版本号
     */
    void set(const QString &version);

private:
    QString version;                 //!< 保存当前DBC文件中解析到的版本号
};

/*!
 * \brief NS类，代表DBC文件中的NS（New Symbol)
 */
class NS
{
public:
    NS(const QSet<QString> &set=QSet<QString>());

    /*!
     * \brief addNewSymbol 添加新的Symbol到NewSymbol集合中
     * \param newSymbol 新的NewSymbol
     */
    void addNewSymbol(const QString &newSymbol);

    /*!
     * \brief reset 清空NewSymbol集合中的所有元素
     */
    void reset();

    /*!
     * \brief getNewSymbolSet 获取当前的NewSymbol集合
     * \return 当前的NewSymbol集合
     */
    const QSet<QString> &getNewSymbolSet() const;

private:
    QSet<QString> newSymboSet;          //!< NewSymbol集合
};

/*!
 * \brief BS类表示DBC文件中位时间参数信息
 */
class BS
{
public:
    BS(quint32 baurate=0, quint32 btr1=0, quint32 btr2=0);

    /*!
     * \brief getBaurate 获取总线波特率值
     * \retval 总线波特率值
     */
    quint32 getBaurate() const;

    /*!
     * \brief setBaurate 设置总线波特率
     * \param baurate 要设置的总线波特率
     */
    void setBaurate(quint32 baurate);

    /*!
     * \brief getBTR1 获取BTR1寄存器的值
     * \retval BTR1寄存器的值
     */
    quint32 getBTR1() const;

    /*!
     * \brief setBTR1 设置BTR1寄存器的值
     * \param btr1 要设置BTR1寄存器的值
     */
    void setBTR1(quint32 btr1);

    /*!
     * \brief getBTR2 获取BTR2寄存器的值
     * \retval BTR2寄存器的值
     */
    quint32 getBTR2() const;

    /*!
     * \brief setBTR2 设置BTR2寄存器的值
     * \param btr2 要设置的BTR2寄存器的值
     */
    void setBTR2(quint32 btr2);

private:
    quint32 baurate;     //!< 保存总线波特率
    quint32 btr1;        //!< 保存BTR1寄存器的值
    quint32 btr2;        //!< 保存BTR2寄存器的值
};

/*!
 * \brief Commenter类代表DBC文件中的CM_节点
 */
class Commenter
{
public:
    /*!
     * \brief getComment 获取注释
     * \retval 注释
     */
    const QString &getComment() const
    {
        return this->comment;
    }

    /*!
     * \brief setComment 设置注释
     * \param comment 要设置的注释
     */
    void setComment(const QString &comment)
    {
        this->comment = comment;
    }

protected:
    QString comment;                            //!< 注释
};

/*!
 * \brief AdditionalAttribute类表示附加属性
 */
class AdditionalAttribute
{
public:

    enum ValueType {
        VALUE_TYPE_INT=0,       //!< 整形类型
        VALUE_TYPE_HEX,         //!< HEX类型
        VALUE_TYPE_FLOAT,       //!< 浮点类型
        VALUE_TYPE_STRING,      //!< 字符串类型
        VALUE_TYPE_ENUM         //!< 枚举类型
    };

    ValueType getValueType() const
    {
        return this->valueType;
    }

    void setIntRange(qint32 minimum, qint32 maximum)
    {
        this->value.intVal.minimum = minimum;
        this->value.intVal.maximum = maximum;
        this->valueType = VALUE_TYPE_INT;
    }

    bool getIntRange(qint32 &minimum, qint32 &maximum) const
    {
        if (VALUE_TYPE_INT == this->valueType) {
            minimum = this->value.intVal.minimum;
            maximum = this->value.intVal.maximum;
            return true;
        }
        return false;
    }

    void setIntValue(qint32 value)
    {
        if (VALUE_TYPE_INT == this->valueType) {
            this->value.intVal.setValue(value);
        }
    }

    qint32 getIntValue() const
    {
        if (VALUE_TYPE_INT == this->valueType) {
            return this->value.intVal.getValue();
        }
        return -1;
    }

    void setHexRange(qint32 minimum, qint32 maximum)
    {
        this->valueType = VALUE_TYPE_HEX;
        this->value.hexVal.minimum = minimum;
        this->value.hexVal.maximum = maximum;
    }

    bool getHexRange(qint32 &minimum, qint32 &maximum) const
    {
        if (VALUE_TYPE_HEX == this->valueType) {
            minimum = this->value.hexVal.minimum;
            maximum = this->value.hexVal.maximum;
            return true;
        }
        return false;
    }

    void setHexValue(qint32 value)
    {
        if (VALUE_TYPE_HEX == this->valueType) {
            this->value.hexVal.setValue(value);
        }
    }

    qint32 getHexValue() const
    {
        if (VALUE_TYPE_HEX == this->valueType) {
            return this->value.hexVal.getValue();
        }
        return -1;
    }

    QString getHexString() const
    {
        if (VALUE_TYPE_HEX == this->valueType) {
            return this->value.hexVal.getHexValue();
        }
        return "";
    }

    void setFloatRange(double minimum, double maximum)
    {
        this->valueType = VALUE_TYPE_FLOAT;
        this->value.floatVal.minimum = minimum;
        this->value.floatVal.maximum = maximum;
    }

    bool getFloatRange(double &minimum, double &maximum) const
    {
        if (VALUE_TYPE_FLOAT == this->valueType) {
            minimum = this->value.floatVal.minimum;
            maximum = this->value.floatVal.maximum;
            return true;
        }
        return false;
    }

    void setFloatValue(double value)
    {
        if (VALUE_TYPE_FLOAT == this->valueType) {
            this->value.floatVal.setValue(value);
        }
    }

    double getFloatValue() const
    {
        if (VALUE_TYPE_FLOAT == this->valueType) {
            return this->value.floatVal.getValue();
        }
        return -1;
    }

    void setStringType()
    {
        this->valueType = VALUE_TYPE_STRING;
    }

    const QString &getStringValue() const
    {
        if (VALUE_TYPE_STRING == this->valueType) {
            return this->strVal.getValue();
        }
        return EMPTY_STRING;
    }

    void setStringValue(const QString &str)
    {
        if (VALUE_TYPE_STRING == this->valueType) {
            this->strVal.setValue(str);
        }
    }

    void setEnumType()
    {
        this->valueType = VALUE_TYPE_ENUM;
    }

    void addEnumValue(const QString &value)
    {
        if (VALUE_TYPE_ENUM == this->valueType) {
            this->enumVal.addEnumValue(value);
        }
    }

    const QString &getEnumValue(qint32 index) const
    {
        if (VALUE_TYPE_ENUM == this->valueType) {
            return this->enumVal.getEnumValue(index);
        }
        return EMPTY_STRING;
    }

    qint32 getEnumIndex(const QString &value) const
    {
        if (VALUE_TYPE_ENUM == this->valueType) {
            return this->enumVal.getEnumIndex(value);
        }
        return -1;
    }

    void setEnumValue(qint32 index)
    {
        if (VALUE_TYPE_ENUM == this->valueType) {
            this->enumVal.setValue(index);
        }
    }


    void setEnumValue(const QString &index)
    {
        if (VALUE_TYPE_ENUM == this->valueType) {
            this->enumVal.setValue(index);
        }
    }

    qint32 getEnumValue() const
    {
        if (VALUE_TYPE_ENUM == this->valueType) {
            return this->enumVal.getValue();
        }
        return -1;
    }

    const QString &getEnumStringValue() const
    {
        if (VALUE_TYPE_ENUM == this->valueType) {
            return this->enumVal.getEnumValue(getEnumValue());
        }
        return EMPTY_STRING;
    }

    QStringList getEnumValues() const
    {
        if (VALUE_TYPE_ENUM == this->valueType) {
            return this->enumVal.getEnumValues();
        }
        return QStringList();
    }

    void setValue(qint32 value)
    {
        switch (this->valueType) {
        case VALUE_TYPE_ENUM:
            this->enumVal.setValue(value);
            break;
        case VALUE_TYPE_HEX:
            this->value.hexVal.setValue(value);
            break;
        case VALUE_TYPE_INT:
            this->value.intVal.setValue(value);
            break;
        default:
            break;
        }
    }

    void setValue(const QString &value)
    {
        switch (this->valueType) {
        case VALUE_TYPE_ENUM:
            this->enumVal.setValue(value);
            break;
        case VALUE_TYPE_STRING:
            this->strVal.setValue(value);
            break;
        default:
            break;
        }
    }

    void setValue(double value)
    {
        switch (this->valueType) {
        case VALUE_TYPE_FLOAT:
            this->value.floatVal.setValue(value);
            break;
        default:
            break;
        }
    }

private:

    template <typename T>
    struct RangeValue {
        T minimum;
        T maximum;
        T value;

        void setValue(T value)
        {
            if (value >= minimum && value <= maximum) {
                this->value = value;
            }
        }

        T getValue() const
        {
            return this->value;
        }
    };

    // 执行数值
    struct IntValue : public RangeValue<qint32> {};

    // HEX数据
    struct HexValue : public IntValue {
        QString getHexValue() const
        {
            return QString("%1").arg(this->value, 0, 16);
        }
    };

    // Float数值
    struct FloatValue : public RangeValue<double> {};

    // 字符串值
    struct StringValue {
        QString value;

        void setValue(const QString &value)
        {
            this->value = value;
        }

        const QString &getValue() const
        {
            return this->value;
        }
    };

    // 枚举类型值
    struct EnumValue {
        typedef QStringList EnumValueTable;

        EnumValue() : valueIndex(-1) {}

        /*!
         * \brief addEnumValue 添加枚举类型值
         * \param value 要添加的枚举类型值
         */
        void addEnumValue(const QString &value)
        {
            this->valueTable.push_back(value);
        }

        /*!
         * \brief getEnumValue 获取指定索引位置上的枚举值
         * \param index 指定索引位置
         * \retval 指定索引位置上的枚举值
         */
        const QString &getEnumValue(qint32 index) const
        {
            if (index < valueTable.size()) {
                return valueTable.at(index);
            }
            return EMPTY_STRING;
        }

        /*!
         * \brief setValue 设置枚举值
         * \param value 要设置的枚举值
         */
        void setValue(const QString &value)
        {
            valueIndex = getEnumIndex(value);
        }

        /*!
         * \brief setValue 设置枚举值
         * \param index 要设置的索引
         */
        void setValue(qint32 index)
        {
            if (index < valueTable.size()) {
                valueIndex = index;
            }
        }

        /*!
         * \brief getValue 获取枚举值
         * \retval 枚举值
         */
        qint32 getValue() const
        {
            return valueIndex;
        }

        /*!
         * \brief getEnumIndex 获取指定枚举值对应的索引
         * \param value 指定枚举值
         * \retval 指定枚举值对应的索引
         */
        qint32 getEnumIndex(const QString &value) const
        {
            return valueTable.indexOf(value);
        }

        /*!
         * \brief getEnumValues 获取enum内的所有值
         * \retval enum内的所有值
         */
        const EnumValueTable& getEnumValues() const
        {
            return this->valueTable;
        }

        EnumValueTable valueTable;
        int valueIndex;
    };

    ValueType valueType;            //!< 存储值类型
    union {
        IntValue intVal;
        HexValue hexVal;
        FloatValue floatVal;
    } value;                        //!< 存储对应的值
    StringValue strVal;
    EnumValue enumVal;
};

class AdditionalAttributeTable
{
public:
    typedef QMap<QString, AdditionalAttribute>  AdditionalAttributeMap;

    void addAdditionalAttribute(const QString &name, const AdditionalAttribute &attr)
    {
        table.insert(name, attr);
    }

    void removeAdditionalAttribute(const QString &name)
    {
        table.remove(name);
    }

    AdditionalAttribute* getAdditionalAttribute(const QString &name)
    {
        auto iter = table.find(name);
        if (iter != table.end()) {
            return &iter.value();
        }
        return NULL;
    }

    const AdditionalAttribute* getAdditionalAttribute(const QString &name) const
    {
        auto iter = table.find(name);
        if (iter != table.end()) {
            return &iter.value();
        }
        return NULL;
    }

    const AdditionalAttributeMap& getAdditionalAttributeMap() const
    {
        return table;
    }

protected:
    AdditionalAttributeMap table;
};

/*!
 * \brief ValueDescription 作为Val和ValTable的基类存在，主要复位维护数据
 */
class ValueDescription
{
public:
    typedef QMap<qint32, QString> ValueDescTable;  //!< 表示对应值描述对应的表类型

    /*!
     * \brief addValueDesc 添加值描述
     * \param value 要添加的值
     * \param desc 对应的值描述
     */
    void addValueDesc(qint32 value, const QString &desc)
    {
        valueDescTable.insert(value, desc);
    }

    /*!
     * \brief removeValueDesc 移除指定的值描述
     * \param value 要移除的值
     */
    void removeValueDesc(qint32 value)
    {
        valueDescTable.remove(value);
    }

    /*!
     * \brief getValueDescTable 获取值描述表
     * \retval 描述表
     */
    const ValueDescTable & getValueDescTable() const
    {
        return valueDescTable;
    }

protected:
    ValueDescTable valueDescTable;
};

/*!
 * \brief ValTable类对应DBC文件中的VAL_TABLE_节点
 */
class ValTable : public ValueDescription
{
public:

    /*!
     * \brief setName 设置值表名
     * \param name 要设置的值表名
     */
    void setName(const QString &name)
    {
        this->name = name;
    }

    /*!
     * \brief getName 返回值表名
     * \retval 值表名
     */
    const QString &getName() const
    {
        return this->name;
    }

private:
    QString name; //!< 用来保存值表名
};

/*!
 * \brief Signal类，定义DBC文件中的SG_节点和VAL_节点
 */
class Signal : public Commenter, public ValueDescription
{
public:
    enum MultiplexerIndType
    {
        MULTIPLEXER_IND_TYPE_SWITCH=0,          //!< 多路复用指示开关类型
        MULTIPLEXER_IND_TYPE_VALUE,             //!< 多路复用指示值类型
        MULTIPLEXER_IND_TYPE_NONE               //!< 多路复用指示空类型
    };

    enum ByteOrderType
    {
        BYTE_ORDER_TYPE_MOTOROLA=0,             //!< 摩托罗拉字节序（大端）
        BYTE_ORDER_TYPE_INTEL                   //!< 英特尔字节序（小端）
    };

    enum ValueType
    {
        VALUE_TYPE_UNSIGNED='+',                //!< 无符号类型
        VALUE_TYPE_SIGNED='-'                   //!< 有符号类型
    };

    enum ExternalValueType
    {
        EXTERNAL_VALUE_TYPE_SIGNED_UNSIGNED=0,  //!< 有符号或者无符号类型
        EXTERNAL_VALUE_TYPE_32_BIT_IEEE_FLOAT,  //!< 32位浮点数
        EXTERNAL_VALUE_TYPE_64_BIT_IEEE_DOUBLE  //!< 64位双精度
    };

    /*!
     * \brief getName 获取信号名
     * \retval 信号名
     */
    const QString &getName() const
    {
        return sigName;
    }

    /*!
     * \brief setName 设置信号名
     * \param sigName 要设置的信号名
     */
    void setName(const QString &sigName)
    {
        this->sigName = sigName;
    }

    /*!
     * \brief getMultiplexerIndicatorType 获取多路复用指示类型
     * \retval 多路复用指示类型
     */
    MultiplexerIndType getMultiplexerIndicatorType() const
    {
        return this->multiplexerIndType;
    }

    /*!
     * \brief setMultiplexerIndicatorType 设置多路复用指示类型
     * \param multiplexerIndType 要设置的多路复用指示类型
     */
    void setMultiplexerIndicatorType(MultiplexerIndType multiplexerIndType)
    {
        this->multiplexerIndType = multiplexerIndType;
    }

    /*!
     * \brief getMultiplexerIndicatorValue 获取多路复用指示值
     * \retval 多路复用指示值
     */
    qint32 getMultiplexerIndicatorValue() const
    {
        return this->multiplexerIndValue;
    }

    /*!
     * \brief setMultiplexerIndicatorValue 设置多路复用指示值
     * \param value 要设置的多路复用指示值
     */
    void setMultiplexerIndicatorValue(quint32 value)
    {
        this->multiplexerIndValue = value;
    }

    /*!
     * \brief getStartBit 获取起始位
     * \retval 起始位
     */
    qint32 getStartBit() const
    {
        return this->startBit;
    }

    /*!
     * \brief setStartBit
     * \param startBit
     */
    void setStartBit(qint32 startBit)
    {
        this->startBit = startBit;
    }

    /*!
     * \brief getBitLen 获取信号位长度
     * \retval 信号位长度
     */
    qint32 getBitLen() const
    {
        return this->bitLen;
    }

    /*!
     * \brief setBitLen 设置信号位长度
     * \param bitLen 要设置的信号位长度
     */
    void setBitLen(qint32 bitLen)
    {
        this->bitLen = bitLen;
    }

    /*!
     * \brief getByteOrder 获取字节序
     * \retval 字节序
     */
    ByteOrderType getByteOrder() const
    {
        return this->byteOrder;
    }

    /*!
     * \brief setByteOrder 设置字节序
     * \param byteOrder 要设置的字节序
     */
    void setByteOrder(ByteOrderType byteOrder)
    {
        this->byteOrder = byteOrder;
    }

    /*!
     * \brief getValueType 获取值类型
     * \retval 值类型
     */
    ValueType getValueType() const
    {
        return this->valueType;
    }

    /*!
     * \brief setValueType 设置值类型
     * \param valueType 要设置的值类型
     */
    void setValueType(ValueType valueType)
    {
        this->valueType = valueType;
    }

    /*!
     * \brief getExternalValueType 获取外部值类型
     * \retval 外部值类型
     */
    ExternalValueType getExternalValueType() const
    {
        return this->externValueType;
    }

    /*!
     * \brief setExternalValueType 设置外部值类型
     * \param externValueType 要设置的外部值类型
     */
    void setExternalValueType(ExternalValueType externValueType)
    {
        this->externValueType = externValueType;
    }

    /*!
     * \brief getFactor 获取精度
     * \retval 精度
     */
    double getFactor() const
    {
        return factor;
    }

    /*!
     * \brief setFactor 设置精度
     * \param factor 要设置的精度
     */
    void setFactor(double factor)
    {
        this->factor = factor;
    }

    /*!
     * \brief getOffset 获取偏移值
     * \retval 偏移值
     */
    double getOffset() const
    {
        return offset;
    }

    /*!
     * \brief setOffset 设置偏移值
     * \param offset 要设置的偏移值
     */
    void setOffset(double offset)
    {
        this->offset = offset;
    }

    /*!
     * \brief getMinimum 获取最小值（物理值）
     * \retval 最小值（物理值）
     */
    double getMinimum() const
    {
        return this->minimum;
    }

    /*!
     * \brief setMinimum 设置最小值（物理值）
     * \param minimum 要设置的最小值（物理值）
     */
    void setMinimum(double minimum)
    {
        this->minimum = minimum;
    }

    /*!
     * \brief getMaximum 获取最大值（物理值）
     * \retval 最大值（物理值）
     */
    double getMaximum() const
    {
        return this->maximum;
    }

    /*!
     * \brief setMaximum 设置最大值（物理值）
     * \param maximum 要设置的最大值（物理值）
     */
    void setMaximum(double maximum)
    {
        this->maximum = maximum;
    }

    /*!
     * \brief getUnit 获取单位
     * \retval 单位
     */
    const QString &getUnit() const
    {
        return this->unit;
    }

    /*!
     * \brief setUnit 设置单位
     * \param unit 要设置的单位
     */
    void setUnit(const QString &unit)
    {
        this->unit = unit;
    }

    /*!
     * \brief addReceiver 添加接收者
     * \param receiver 要添加的接收者
     */
    void addReceiver(const QString &receiver)
    {
        receiverSet.insert(receiver);
    }

    /*!
     * \brief resetReceiverSet 清空所有接收者
     */
    void resetReceiverSet()
    {
        receiverSet.clear();
    }

    /*!
     * \brief getReceiverSet 获取接收者集合
     * \retval 接收者集合
     */
    QSet<QString> getReceiverSet() const
    {
        return receiverSet;
    }

private:
    QString sigName;                            //!< 保存信号名称
    MultiplexerIndType multiplexerIndType;      //!< 多路复用指示类型
    quint32 multiplexerIndValue;                //!< 多路复用指示值
    qint32  startBit;                           //!< 起始位
    qint32  bitLen;                             //!< 信号的位长度
    ByteOrderType byteOrder;                    //!< 字节序
    ValueType valueType;                        //!< 值类型
    ExternalValueType externValueType;          //!< 外部值类型
    double factor;                              //!< 精度
    double offset;                              //!< 偏移值
    double minimum;                             //!< 最小值
    double maximum;                             //!< 最大值
    QString unit;                               //!< 单位
    QSet<QString> receiverSet;                  //!< 集合
};

class Message : public Commenter
{
public:
    typedef QMap<QString, Signal*> SignalTable;

    ~Message()
    {
        resetSignalList();
    }

    /*!
     * \brief getName 获取报文名称
     * \retval 报文名称
     */
    const QString &getName() const
    {
        return this->name;
    }

    /*!
     * \brief setName 设置报文名称
     * \param name 要设置的报文名称
     */
    void setName(const QString &name)
    {
        this->name = name;
    }

    /*!
     * \brief getId 获取报文ID
     * \retval 报文ID
     */
    quint32 getId() const
    {
        return this->id;
    }

    /*!
     * \brief setId 设置报文ID
     * \param id 要设置的报文ID
     */
    void setId(quint32 id)
    {
        this->id = id;
    }

    /*!
     * \brief addTransmitter 添加发送者
     * \param transmitter 要添加的发送者
     */
    void addTransmitter(const QString &transmitter)
    {
        transmitterSet.insert(transmitter);
    }

    /*!
     * \brief resetTransmitterSet 清空发送者集合
     */
    void resetTransmitterSet()
    {
        transmitterSet.clear();
    }

    /*!
     * \brief getTransmitterSet 返回发送者集合
     * \retval 发送者集合
     */
    const QSet<QString>& getTransmitterSet() const
    {
        return transmitterSet;
    }

    /*!
     * \brief getTransmitter 获取发送者名称
     * \retval 发送者名称
     */
    QString getTransmitter()
    {
        auto iter = transmitterSet.begin();
        if (iter != transmitterSet.end()) {
            return *iter;
        }
        return QString("");
    }

    /*!
     * \brief setByteLen 设置报文数据长度
     * \param byteLen 要设置的报文数据长度
     */
    void setByteLen(qint32 byteLen)
    {
        this->byteLen = byteLen;
    }

    /*!
     * \brief getByteLen 获取报文字节长度
     * \retval 报文字节长度
     */
    qint32 getByteLen() const
    {
        return this->byteLen;
    }

    /*!
     * \brief addSignal 添加信号到列表中
     * \param signal 要添加的信号
     */
    void addSignal(Signal *signal)
    {
        if (signal != NULL) {
            signalList.insert(signal->getName(), signal);
        }
    }

    /*!
     * \brief removeSignal 移除指定名称的信号
     * \param name 要移除信号的名称
     */
    void removeSignal(const QString &name)
    {
        auto iter = signalList.find(name);
        if (iter != signalList.end()) {
            delete iter.value();
            iter.value() = NULL;
            signalList.erase(iter);
        }
    }

    /*!
     * \brief getSignal 获取指定名称的信号
     * \param name 指定的信号名称
     * \retval 指定名称的信号
     */
    Signal* getSignal(const QString &name)
    {
        auto iter = signalList.find(name);
        if (iter != signalList.end()) {
            return iter.value();
        }
        return NULL;
    }

    /*!
     * \brief resetSignalList 清空信号列表
     */
    void resetSignalList()
    {
        for (auto iter = signalList.begin(); iter != signalList.end(); ++iter) {
            delete iter.value();
            iter.value() = NULL;
        }
        signalList.clear();
    }

    /*!
     * \brief getSignalList 获取信号列表
     * \retval 信号列表
     */
    const SignalTable &getSignalList() const
    {
        return this->signalList;
    }

private:
    QString name;                            //!< 报文名称
    quint32 id;                              //!< 报文id
    QSet<QString> transmitterSet;            //!< 发送者集合
    qint32  byteLen;                         //!< 报文数据长度
    SignalTable signalList;                //!< 信号列表
};

/*!
 * \brief The Node class 网络节点，对应DBC文件中的BU_
 */
class Node : public Commenter
{
public:

    ~Node()
    {
        clearMsgMap(this->txMsgMap);
    }

    /*!
     * \brief clearMsgMap 清除指定的MsgMap
     * \param msgMap 要清除的msgMap
     */
    void clearMsgMap(QMap<qint32, Message*> &msgMap)
    {
        for (auto iter = msgMap.begin(); iter != msgMap.end(); ++iter) {
            delete iter.value();
        }
        msgMap.clear();
    }


    /*!
     * \brief setName 设置节点名称
     * \param name 节点名称
     */
    void setName(const QString &name)
    {
        this->name = name;
    }

    /*!
     * \brief getName 获取节点名称
     * \return 节点名称
     */
    const QString &getName() const
    {
        return this->name;
    }

    /*!
     * \brief addTxMsg 添加发送报文
     * \param msg 要添加的发送报文
     */
    void addTxMsg(Message *msg)
    {
        Message *oldMsg = NULL;
        auto iter = this->txMsgMap.find(msg->getId());
        if (iter != this->txMsgMap.end()) {
            oldMsg = iter.value();
        }
        if (oldMsg != msg) {
            delete oldMsg;
            this->txMsgMap.insert(msg->getId(), msg);
        }
    }

    /*!
     * \brief removeTxMsg 移除发送报文
     * \param id 要移除的报文ID
     */
    void removeTxMsg(qint32 id)
    {
        auto iter = this->txMsgMap.find(id);
        if (iter != this->txMsgMap.end()) {
            delete iter.value();
            this->txMsgMap.remove(id);
        }
    }

    /*!
     * \brief getTxMsgMap 获取发送报文Map
     * \retval 发送报文Map
     */
    const QMap<qint32, Message*> &getTxMsgMap() const
    {
        return this->txMsgMap;
    }

    /*!
     * \brief getTxMsg 报文报文id来查找此报文的发送报文
     * \param id 报文id
     * \return 指定id的发送报文
     */
    Message* getTxMsg(qint32 id)
    {
        auto iter = txMsgMap.find(id);
        if (iter != txMsgMap.end()) {
            return iter.value();
        }
        return NULL;
    }

private:
    QString name;                            //!< 节点名称
    QMap<qint32, Message*> txMsgMap;         //!< 发送报文Map
};

/*!
 * \brief EVBase类是所有环境变量类的基类类型
 */
class EVBase
{
public:
    enum ClassType {
        CLASS_TYPE_BASE_TYPE,
        CLASS_TYPE_DATA_TYPE
    };

    virtual ~EVBase() {}

    /*!
     * \brief getClassType 由子类实现，获取类类型
     * \retval 类类型
     */
    virtual ClassType getClassType() const = 0;

    /*!
     * \brief setName 设置环境变量名称
     * \param name 环境变量名称
     */
    void setName(const QString &name)
    {
        this->name = name;
    }

    /*!
     * \brief getName 获取环境变量名称
     * \retval 环境变量名称
     */
    const QString &getName() const
    {
        return this->name;
    }

private:
    QString name;                       //!< 环境变量名称
};

/*!
 * \brief EV 类对应dbc文件中的EV_节点，它只表示基本类型的环境变量
 */
class EV : public EVBase, public Commenter
        , public ValueDescription
{
public:
    enum EnvVarType {
        ENV_VAR_TYPE_INTEGER=0,         //!< 表示变量值是整形
        ENV_VAR_TYPE_FLOAT,             //!< 表示变量值是浮点类型
        ENV_VAR_TYPE_STRING             //!< 表示变量值是字符串类型
    };

    enum AccessType {
        ACCESS_TYPE_UNRESTRICTED=0,     //!< 没有限制
        ACCESS_TYPE_READ,               //!< 读
        ACCESS_TYPE_WRITE,              //!< 写
        ACCESS_TYPE_READ_WRITE          //!< 读写
    };

    ~EV() {}

    ClassType getClassType() const override
    {
        return CLASS_TYPE_BASE_TYPE;
    }

    /*!
     * \brief getEnvVarType 获取环境变量值类型
     * \retval 环境变量值类型
     */
    EnvVarType getEnvVarType() const
    {
        return envVarType;
    }

    /*!
     * \brief setEnvVarType 设置环境变量类型
     * \param envVarType 要设置的环境变量类型
     */
    void setEnvVarType(EnvVarType envVarType)
    {
        this->envVarType = envVarType;
    }

    /*!
     * \brief getMinimum 获取最小值
     * \retval 最小值
     */
    double getMinimum() const
    {
        return minimum;
    }

    /*!
     * \brief setMinimum 设置最小值
     * \param minimum 要设置的最小值
     */
    void setMinimum(double minimum)
    {
        this->minimum = minimum;
    }

    /*!
     * \brief getMaximum 获取最大值
     * \retval 最大值
     */
    double getMaximum() const
    {
        return this->maximum;
    }

    /*!
     * \brief setMaximum 设置最大值
     * \param maximum 要设置的最大值
     */
    void setMaximum(double maximum)
    {
        this->maximum = maximum;
    }

    /*!
     * \brief getUnit 获取单位
     * \retval 单位
     */
    const QString &getUnit() const
    {
        return this->unit;
    }

    /*!
     * \brief setUnit 设置单位
     * \param unit 要设置的单位
     */
    void setUnit(const QString &unit)
    {
        this->unit = unit;
    }

    /*!
     * \brief getInitValue 获取初始值
     * \return 初始值
     */
    double getInitValue() const
    {
        return this->initValue;
    }

    /*!
     * \brief setInitValue 设置初始值
     * \param initValue 要设置的初始值
     */
    void setInitValue(double initValue)
    {
        this->initValue = initValue;
    }

    /*!
     * \brief getEvId 获取要设置的环境id
     * \retval  要设置的环境id
     */
    quint32 getEvId() const
    {
        return this->evId;
    }

    /*!
     * \brief setEvId 设置环境id
     * \param evId 要设置的环境id
     */
    void setEvId(quint32 evId)
    {
        this->evId = evId;
    }

    /*!
     * \brief getAccessType 返回访问类型
     * \retval  访问类型
     */
    AccessType getAccessType() const
    {
        return this->accessType;
    }

    /*!
     * \brief setAccessType 设置访问类型
     * \param accessType 要设置的访问类型
     */
    void setAccessType(AccessType accessType)
    {
        this->accessType = accessType;
    }

    /*!
     * \brief addAccessNode 添加访问节点名称
     * \param name 访问节点名称
     */
    void addAccessNode(const QString &name)
    {
        accessNodeList.push_back(name);
    }

    /*!
     * \brief removeAccessNode 移除指定名称的访问节点
     * \param name 要移除的访问节点名称
     */
    void removeAccessNode(const QString &name)
    {
        auto index = accessNodeList.indexOf(name);
        if (index != -1) {
            accessNodeList.removeAt(index);
        }
    }

    /*!
     * \brief getAllAccessNode 获取所有访问节点
     * \retval 所有访问节点
     */
    const QStringList &getAllAccessNode() const
    {
        return accessNodeList;
    }

    /*!
     * \brief getIntValue 获取整形值
     * \retval 整形值
     */
    qint32 getIntValue() const
    {
        if (ENV_VAR_TYPE_INTEGER == envVarType) {
            return 0;
        }
        return value.intVal;
    }

    /*!
     * \brief setIntValue 设置整型值
     * \param intVal 要设置的整型值
     */
    void setIntValue(qint32 intVal)
    {
        this->envVarType = ENV_VAR_TYPE_INTEGER;
        this->value.intVal = intVal;
    }

    /*!
     * \brief getFloatVal 或是浮点类型值
     * \retval 浮点类型值
     */
    float getFloatVal() const
    {
        if (ENV_VAR_TYPE_FLOAT == envVarType) {
            return 0.0;
        }
        return this->value.floatVal;
    }

    /*!
     * \brief setFloatVal 设置浮点类型值
     * \param floatVal 浮点类型值
     */
    void setFloatVal(float floatVal)
    {
        this->envVarType = ENV_VAR_TYPE_FLOAT;
        this->value.floatVal = floatVal;
    }

    /*!
     * \brief getStrVal 获取字符串类型值
     * \retval 字符串类型值
     */
    QString getStrVal() const
    {
        if (ENV_VAR_TYPE_STRING == envVarType) {
            return "";
        }
        return this->strVal;
    }

    /*!
     * \brief setStrVal 设置字符串类型值
     * \param strVal 要设置的字符串值
     */
    void setStrVal(const QString strVal)
    {
        this->envVarType = ENV_VAR_TYPE_STRING;
        this->strVal = strVal;
    }

private:
    EnvVarType envVarType;              //!< 表示基本的变量值类型
    double minimum;                     //!< 最小值
    double maximum;                     //!< 最大值
    QString unit;                       //!< 单位
    double initValue;                   //!< 初始值
    quint32 evId;                       //!< 环境变量id
    AccessType accessType;              //!< 访问类型
    QStringList accessNodeList;         //!< 访问节点列表
    union {
        qint32 intVal;
        float floatVal;
    } value;                            //!< 存储对应的值
    QString strVal;
};

/*!
 * \brief EnvVarData类表示dbc文件中的ENVAR_DATA_节点，这个类数据以二进制格式存储
 */
class EnvVarData : public EVBase
{
public:
    typedef QList<quint8> ValueType;

    ClassType getClassType() const override
    {
        return CLASS_TYPE_DATA_TYPE;
    }

    /*!
     * \brief getDataSize 获取数据尺寸
     * \retval 数据尺寸
     */
    qint32 getDataSize() const
    {
        return value.size();
    }

    /*!
     * \brief setDataSize 设置数据尺寸
     * \param newDataSize 要设置的数据尺寸
     */
    void setDataSize(qint32 newDataSize)
    {
        qint32 oldDataSize = getDataSize();
        if (oldDataSize < newDataSize) {        //要添加
            for (; oldDataSize < newDataSize; ++oldDataSize) {
                value.push_back(0);
            }
        } else if (newDataSize < oldDataSize) { //要减少
            for (; newDataSize < oldDataSize; ++newDataSize) {
                value.pop_back();
            }
        }
    }

    /*!
     * \brief getValue 或是值，可以直接获取或者修改这个值
     * \retval 值
     */
    ValueType &getValue()
    {
        return value;
    }

private:
    ValueType value;        //!< 值类型
};

/*!
 * \brief Document类代表整个DBC文件
 */
class Document
{
public:
    /*!
     * \brief ForEachNewSymbolFn 是foreachAllNewSymbols方法的回调函数类型
     * \param arg 是附加参数，此参数由foreachAllNewSymbols方法的第二个参数决定
     * \param newSymbol 当前的新标签(New Symbol)的名称
     */
    typedef bool (*ForEachNewSymbolFn)(void* arg, const QString &newSymbol);

    typedef QMap<QString, Node*> NodeMap;   //!< （网络）节点列表
    typedef QMap<QString, ValTable*> ValueTableMap;
    typedef QMap<QString, EV*> EVMap;
    typedef QMap<QString, EnvVarData*> EVDataMap;

    typedef QMap<qint32, AdditionalAttributeTable> DefaultAttrTblMap;
    typedef QMap<QString, AdditionalAttributeTable> NodeAttrTblMap;
    typedef QMap<QString, AdditionalAttributeTable> EVAttrTblMap;
    typedef QMap<qint32, AdditionalAttributeTable>  MsgAttrTblMap;
    typedef QMap<Signal*, AdditionalAttributeTable> SigAttrTablMap;

    enum ObjectType
    {
        OBJECT_TYPE_BU = 0,
        OBJECT_TYPE_BO,
        OBJECT_TYPE_SG,
        OBJECT_TYPE_EV,
        OBJECT_TYPE_GL  //!< 全局
    };

    Document();
    ~Document();

    /*!
     * \brief getVersion 获取版本号
     * \retval 版本号
     */
    const QString &getVersion() const
    {
        return version.get();
    }

    /*!
     * \brief setVersion 设置版本号
     * \param version 要设置的版本号
     */
    void setVersion(const QString &version)
    {
        this->version.set(version);
    }

    /*!
     * \brief addNewSymbol 添加新标签（NewSymbol）
     * \param newSymbol 要新建的新标签（NewSymbol）
     */
    void addNewSymbol(const QString &newSymbol)
    {
        ns.addNewSymbol(newSymbol);
    }

    /*!
     * \brief foreachAllNewSymbols 遍历所有的新标签（NewSymbol）
     * \param fn 回调函数
     */
    void foreachAllNewSymbols(ForEachNewSymbolFn fn, void *arg);

    /*!
     * \brief getNS 获取所有New Symbol
     * \retval 所有New Symbol
     */
    const QSet<QString> &getNS() const;

    /*!
     * \brief setBS     设置位定时参数
     * \param baurate   波特率
     * \param btr1      BTR1寄存器的值
     * \param btr2      BTR2寄存器的值
     */
    void setBS(quint32 baurate, quint32 btr1, quint32 btr2);

    /*!
     * \brief getBS         获取位定时参数
     * \param [out] baurate 接收波特率值
     * \param [out] btr1    接收BTR1寄存器的值
     * \param [out] btr2    接收BTR2寄存器的值
     */
    void getBS(quint32 &baurate, quint32 &btr1, quint32 &btr2);

    /*!
     * \brief addNode 添加（网络）节点
     * \param node 要添加的（网络）节点
     */
    void addNode(Node *node);

    /*!
     * \brief removeNode 移除节点
     * \param nodeName 要移除的节点名称
     */
    void removeNode(const QString &nodeName);

    /*!
     * \brief removeNode 移除节点，要移除的节点
     * \param node 要移除的节点
     */
    void removeNode(Node *node);

    /*!
     * \brief removeAllNode 移除所有节点
     */
    void removeAllNode();

    /*!
     * \brief getNode 获取指定名称节点
     * \param nodeName 要获取的节点名称
     * \retval 执行名称的节点
     */
    Node* getNode(const QString &nodeName);

    /*!
     * \brief getNodeMap 获取所有节点
     * \retval 所有节点
     */
    const NodeMap &getNodeMap() const
    {
        return this->nodeMap;
    }

    /*!
     * \brief getBU 获取所有节点名称
     * \retval  所有节点名称
     */
    QList<QString> getBU();

    /*!
     * \brief getAllNodeNames 获取所有节点名称，同能同getBU
     * \retval 所有节点名称
     */
    QList<QString> getAllNodeNames()
    {
        return getBU();
    }

    /*!
     * \brief addValueTable 添加ValueTable
     * \param valTable 要添加的ValueTable
     */
    void addValueTable(ValTable *valTable);

    /*!
     * \brief removeValueTable 移除ValueTable
     * \param name 要移除的ValueTable名称
     */
    void removeValueTable(const QString &name);

    /*!
     * \brief removeAllValueTable 移除所有的ValueTable
     */
    void removeAllValueTable();

    /*!
     * \brief getValueTable 获取指定名称的ValueTable
     * \param name 指定的名称
     * \retval 指定名称的ValueTable
     */
    ValTable *getValueTable(const QString &name);

    /*!
     * \brief getValueTableMap 获取所有的ValueTable
     * \retval 所有的ValueTable
     */
    const ValueTableMap &getValueTableMap() const
    {
        return this->valueTableMap;
    }

    /*!
     * \brief getMessage 根据报文id来获取指定指定报文
     * \param msgId 报文id
     * \retval 指定id的报文
     */
    Message *getMessage(qint32 msgId);

    /*!
     * \brief getSignal 获取指定报文中的指定信号名的信号
     * \param msgId     报文id
     * \param sigName   信号名
     * \retval 指定报文中的指定信号名的信号
     */
    Signal *getSignal(qint32 msgId, const QString &sigName);

    /*!
     * \brief addEV 添加环境变量
     * \param ev 要添加环境变量
     */
    void addEV(EV * ev)
    {
        if (ev != NULL) {
            evMap.insert(ev->getName(), ev);
        }
    }

    /*!
     * \brief removeEV 移除指定名称的环境变量
     * \param name 要移除的环境变量的名称
     */
    void removeEV(const QString &name)
    {
        auto iter = evMap.find(name);
        if (iter != evMap.end()) {
            delete iter.value();
            iter.value() = NULL;
            evMap.erase(iter);
        }
    }

    /*!
     * \brief removeAllEV 移除所有基本类型环境变量
     */
    void removeAllEV()
    {
        for (auto iter = evMap.begin(); iter != evMap.end(); ++iter) {
            delete iter.value();
            iter.value() = NULL;
        }
        evMap.clear();
    }

    /*!
     * \brief getEV 获取指定名称的环境变量
     * \param evName 指定的环境变量名称
     * \return 取指定名称的环境变量
     */
    EV *getEV(const QString &evName)
    {
        auto iter = evMap.find(evName);
        if (iter != evMap.end()) {
            return iter.value();
        }
        return NULL;
    }

    /*!
     * \brief getEVMap 获取所有环境变量
     * \retval 所有环境变量
     */
    const EVMap& getEVMap() const
    {
        return evMap;
    }

    /*!
     * \brief addEVData 添加二进制值类型的环境变量
     * \param evData 要添加的二进制值类型的环境变量
     */
    void addEVData(EnvVarData *evData)
    {
        if (evData != NULL) {
            evDataMap.insert(evData->getName(), evData);
        }
    }

    /*!
     * \brief removeEVData 移除指定名称的二进制值环境变量
     * \param name 要移除的变量的名称
     */
    void removeEVData(const QString &name)
    {
        auto iter = evDataMap.find(name);
        if (iter != evDataMap.end()) {
            delete iter.value();
            iter.value() = NULL;
            evDataMap.erase(iter);
        }
    }

    /*!
     * \brief removeAllEVData 移除所有的二进制环境变量
     */
    void removeAllEVData()
    {
        for (auto iter = evDataMap.begin(); iter != evDataMap.end(); ++iter) {
            delete iter.value();
            iter.value() = NULL;
        }
        evDataMap.clear();
    }

    /*!
     * \brief getEVData 获取指定名称的二进制值环境变量
     * \param name 变量名称
     * \retval 指定名称的二进制值环境变量
     */
    EnvVarData* getEVData(const QString &name)
    {
        auto iter = evDataMap.find(name);
        if (iter != evDataMap.end()) {
            return iter.value();
        }
        return NULL;
    }

    /*!
     * \brief addObjectAttribute 添加默认属性定义
     * \param objectType 对象类型, 实际上值为：Document::ObjectType枚举类型中的值
     * \param attrName 属性名称
     * \param attr 属性值
     */
    void addObjectAttribute(int objectType, const QString &attrName, const AdditionalAttribute &attr)
    {
        auto iter = defaultAttrTblMap.find(objectType);
        if (iter != defaultAttrTblMap.end()) {
            iter.value().addAdditionalAttribute(attrName, attr);
        } else {
            AdditionalAttributeTable tbl;
            tbl.addAdditionalAttribute(attrName, attr);
            defaultAttrTblMap.insert(objectType, tbl);
        }
    }

    /*!
     * \brief getAllObjectAttrPrototypes 获取所有对象属性原型
     * \retval 所有对象属性原型
     */
    const DefaultAttrTblMap &getAllObjectAttrPrototypes() const
    {
        return defaultAttrTblMap;
    }

    /*!
     * \brief getAllObjectAttrDefaultValues 获取所有对象属性默认值
     * \retval  所有对象属性默认值
     */
    const DefaultAttrTblMap &getAllObjectAttrDefaultValues() const
    {
        return defaultAttrTblMap;
    }

    /*!
     * \brief getAllNodeAttrValues 获取所有节点属性值
     * \retval  所有节点属性值
     */
    const NodeAttrTblMap &getAllNodeAttrValues() const
    {
        return this->nodeAttrTblMap;
    }

    /*!
     * \brief getAllMsgAttrValues 获取所有报文属性值
     * \retval 所有报文属性值
     */
    const MsgAttrTblMap &getAllMsgAttrValues() const
    {
        return this->msgAttrTblMap;
    }

    /*!
     * \brief getMsgAttribute   获取报文属性
     * \param attrName          属性名称
     * \param msgId             报文id
     * \retval                  报文属性
     */
    const AdditionalAttribute* getMsgAttribute(quint32 msgId, const QString &attrName) const;

    /*!
     * \brief getAllSigAttrValues 获取所有信号属性值
     * \retval 所有信号属性值
     */
    const SigAttrTablMap &getAllSigAttrValues() const
    {
        return this->sigAttrTblMap;
    }

    /*!
     * \brief getAllEvAttrValues 获取所有环境变量属性值
     * \retval 所有环境变量属性值
     */
    const EVAttrTblMap &getAllEvAttrValues() const
    {
        return this->evAttrTblMap;
    }

    /*!
     * \brief setDefaultAttributeIntValue 设置默认属性整形值
     * \param attrName 属性名称
     * \param value 属性值
     */
    void setDefaultAttributeIntValue(const QString &attrName, qint32 value);

    /*!
     * \brief setDefaultAttributeHexValue 设置默认属性HEX值
     * \param attrName 属性名称
     * \param value 属性值
     */
    void setDefaultAttributeHexValue(const QString &attrName, qint32 value);

    /*!
     * \brief setDefaultAttributeFloatValue 设置默认属性浮点值
     * \param attrName 属性名称
     * \param value 属性值
     */
    void setDefaultAttributeFloatValue(const QString &attrName, double value);

    /*!
     * \brief setDefaultAttributeStringValue 设置默认属性字符串值
     * \param attrName 属性名称
     * \param value 属性值
     */
    void setDefaultAttributeStringValue(const QString &attrName, const QString &value);

    /*!
    * \brief setDefaultAttributeStringValue 设置默认属性枚举值
    * \param attrName 属性名称
    * \param value 属性值
    */
   void setDefaultAttributeEnumValue(const QString &attrName, const QString &value);

   template <typename T>
   void setNodeAttributeValue(const QString &nodeName, const QString &attrName, const T &value);

   template <typename T>
   void setMsgAttributeValue(qint32 msgId, const QString &attrName, const T &value);

   template <typename T>
   void setSigAttributeValue(qint32 msgId, const QString &sigName, const QString &attrName, const T &value);

   template <typename T>
   void setEvAttributeValue(const QString &varName, const QString &attrName, const T &value);

   template <typename T>
   void setGblAttributeValue(const QString &attrName, const T &value);

   /*!
    * \brief getMsgCount 获取所有报文数据
    * \retval  所有报文数据
    */
   qint32 getMsgCount() const
   {
       return msgList.size();
   }

   /*!
    * \brief GetMsgList 获取所有报文
    * \retval 所有报文
    */
   const QList<Message*> &GetMsgList() const
   {
       return msgList;
   }

   /*!
    * \brief onParseDone 解析器解析完成的时候，需要调用一次
    */
   void onParseDone();

private:
    Version                         version;                    //!< 版本号
    NS                              ns;                         //!< 所有NS(New Symbol)
    BS                              bs;                         //!< 位定时参数
    NodeMap                         nodeMap;                    //!< 所有节点
    ValueTableMap                   valueTableMap;              //!< 值表
    EVMap                           evMap;                      //!< 基本类型环境变量
    EVDataMap                       evDataMap;                  //!< 二进制值类型环境变量

    DefaultAttrTblMap               defaultAttrTblMap;          //!< 默认值表(也起类型值表的功能）

    NodeAttrTblMap                  nodeAttrTblMap;             //!< 所有网络节点的属性表
    EVAttrTblMap                    evAttrTblMap;               //!< 所有环境变量的属性表
    MsgAttrTblMap                   msgAttrTblMap;              //!< 所有报文的属性表
    SigAttrTablMap                  sigAttrTblMap;              //!< 所有信号的属性表

    QList<Message*>                 msgList;                    //!< 保存所有报文
};

template <typename T>
void Document::setNodeAttributeValue(const QString &nodeName, const QString &attrName, const T &value)
{
    auto tblIter = nodeAttrTblMap.find(nodeName);
    if (tblIter != nodeAttrTblMap.end()) {
        auto &tbl = tblIter.value();
        auto attrVal = tbl.getAdditionalAttribute(attrName);
        if (attrVal != NULL) {
            attrVal->setValue(value);
        } else {
            auto iter = defaultAttrTblMap.find(OBJECT_TYPE_BU);
            if (iter != defaultAttrTblMap.end()) {
                AdditionalAttribute *defaultAttr = iter.value().getAdditionalAttribute(attrName);
                if (defaultAttr) {
                    AdditionalAttribute attr = *defaultAttr;
                    attr.setValue(value);
                    tbl.addAdditionalAttribute(attrName, attr);
                }
            }
        }
    } else {
        auto iter = defaultAttrTblMap.find(OBJECT_TYPE_BU);
        if (iter != defaultAttrTblMap.end()) {
            AdditionalAttribute *defaultAttr = iter.value().getAdditionalAttribute(attrName);
            if (defaultAttr) {
                AdditionalAttributeTable tbl;
                AdditionalAttribute attr = *defaultAttr;
                attr.setValue(value);
                tbl.addAdditionalAttribute(attrName, attr);
                nodeAttrTblMap.insert(nodeName, tbl);
            }
        }
    }
}

template<typename T>
void Document::setMsgAttributeValue(qint32 msgId, const QString &attrName, const T &value)
{
    auto tblIter = msgAttrTblMap.find(msgId);
    if (tblIter != msgAttrTblMap.end()) {
        auto &tbl = tblIter.value();
        auto attrVal = tbl.getAdditionalAttribute(attrName);
        if (attrVal != NULL) {
            attrVal->setValue(value);
        } else {
            auto iter = defaultAttrTblMap.find(OBJECT_TYPE_BO);
            if (iter != defaultAttrTblMap.end()) {
                AdditionalAttribute *defaultAttr = iter.value().getAdditionalAttribute(attrName);
                if (defaultAttr) {
                    AdditionalAttribute attr = *defaultAttr;
                    attr.setValue(value);
                    tbl.addAdditionalAttribute(attrName, attr);
                }
            }
        }
    } else {
        auto iter = defaultAttrTblMap.find(OBJECT_TYPE_BO);
        if (iter != defaultAttrTblMap.end()) {
            AdditionalAttribute *defaultAttr = iter.value().getAdditionalAttribute(attrName);
            if (defaultAttr) {
                AdditionalAttributeTable tbl;
                AdditionalAttribute attr = *defaultAttr;
                attr.setValue(value);
                tbl.addAdditionalAttribute(attrName, attr);
                msgAttrTblMap.insert(msgId, tbl);
            }
        }
    }
}

template<typename T>
void Document::setSigAttributeValue(qint32 msgId, const QString &sigName,
                                    const QString &attrName, const T &value)
{
    Signal *signal = getSignal(msgId, sigName);
    if (NULL == signal) return;
    auto tblIter = sigAttrTblMap.find(signal);
    if (tblIter != sigAttrTblMap.end()) {
        auto &tbl = tblIter.value();
        auto attrVal = tbl.getAdditionalAttribute(attrName);
        if (attrVal != NULL) {
            attrVal->setValue(value);
        } else {
            auto iter = defaultAttrTblMap.find(OBJECT_TYPE_SG);
            if (iter != defaultAttrTblMap.end()) {
                AdditionalAttribute *defaultAttr = iter.value().getAdditionalAttribute(attrName);
                if (defaultAttr) {
                    AdditionalAttribute attr = *defaultAttr;
                    attr.setValue(value);
                    tbl.addAdditionalAttribute(attrName, attr);
                }
            }
        }
    } else {
        auto iter = defaultAttrTblMap.find(OBJECT_TYPE_SG);
        if (iter != defaultAttrTblMap.end()) {
            AdditionalAttribute *defaultAttr = iter.value().getAdditionalAttribute(attrName);
            if (defaultAttr) {
                AdditionalAttributeTable tbl;
                AdditionalAttribute attr = *defaultAttr;
                attr.setValue(value);
                tbl.addAdditionalAttribute(attrName, attr);
                sigAttrTblMap.insert(signal, tbl);
            }
        }
    }
}

template<typename T>
void Document::setEvAttributeValue(const QString &varName, const QString &attrName,
                                   const T &value)
{
    auto tblIter = evAttrTblMap.find(varName);
    if (tblIter != evAttrTblMap.end()) {
        auto &tbl = tblIter.value();
        auto attrVal = tbl.getAdditionalAttribute(attrName);
        if (attrVal != NULL) {
            attrVal->setValue(value);
        } else {
            auto iter = defaultAttrTblMap.find(OBJECT_TYPE_EV);
            if (iter != defaultAttrTblMap.end()) {
                AdditionalAttribute *defaultAttr = iter.value().getAdditionalAttribute(attrName);
                if (defaultAttr) {
                    AdditionalAttribute attr = *defaultAttr;
                    attr.setValue(value);
                    tbl.addAdditionalAttribute(attrName, attr);
                }
            }
        }
    } else {
        auto iter = defaultAttrTblMap.find(OBJECT_TYPE_EV);
        if (iter != defaultAttrTblMap.end()) {
            AdditionalAttribute *defaultAttr = iter.value().getAdditionalAttribute(attrName);
            if (defaultAttr) {
                AdditionalAttributeTable tbl;
                AdditionalAttribute attr = *defaultAttr;
                attr.setValue(value);
                tbl.addAdditionalAttribute(attrName, attr);
                evAttrTblMap.insert(varName, tbl);
            }
        }
    }
}

template<typename T>
void Document::setGblAttributeValue(const QString &attrName, const T &value)
{
    auto iter = defaultAttrTblMap.find(OBJECT_TYPE_GL);
    if (iter != defaultAttrTblMap.end()) {
        auto &tbl = iter.value();
        auto attrVal = tbl.getAdditionalAttribute(attrName);
        if (attrVal != NULL) {
            attrVal->setValue(value);
        }
    }
}

END_DBC4CPP_NAMESPACE

#endif // ENTITIES_H
