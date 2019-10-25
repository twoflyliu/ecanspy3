#ifndef PARSER_H
#define PARSER_H

#include "dbc4cpp_global.h"
#include "builder.h"

#include <QStringList>
#include <QTextStream>

BEGIN_DBC4CPP_NAMESPACE

/*!
 * \brief Parser定义了解析接口
 */
struct Parser
{
    virtual ~Parser() {}

    /*!
     * \brief parse     解析指定文件
     * \param file      要解析的文件名称
     * \param builder   用于构建最终的结果
     * \retval 返回成功与否信息
     */
    virtual bool parse(const QString &file, Builder &builder) = 0;

    /*!
     * \brief getErrMsg 获取错误消息
     * \retval 返回错误消息
     */
    virtual const QString &getErrMsg() const = 0;
};

class ParserBase : public Parser
{
public:
    ParserBase();

    const QString &getErrMsg() const;
    bool parse(const QString &file, Builder &builder);

protected:
    virtual bool onParse(Builder &Builder) = 0;
    virtual QString getCodec() const = 0;

    QChar getChar();
    void putChar();
    bool isEnd();

protected:
    Context context;

    QString content;
    qint32 cursor;
};

class DBCParser : public ParserBase
{
public:
    typedef bool (DBCParser::*ParseMethod)(Builder &Builder);
    typedef QMap<QString, ParseMethod> ParseMethodMap;

    bool onParse(Builder &builder);
    DBCParser();
protected:

    QString getCodec() const
    {
        return "gbk";
    }

    /*!
     * \brief parseVersion 解析版本号
     * \param builder 构建器
     * \retval 解析成功与否
     */
    bool parseVersion(Builder &builder);

    /*!
     * \brief parseNewSymbol 解析新标签
     * \param builder 构建器
     * \retval 解析成功与否
     */
    bool parseNewSymbol(Builder &builder);

    /*!
     * \brief parseBitSetting 解析位设置
     * \param builder 构建器
     * \retval 解析成功与否
     */
    bool parseBitSetting(Builder &builder);

    /*!
     * \brief parseNode 解析节点
     * \param builder 构建器
     * \retval 解析成功与否
     */
    bool parseNode(Builder &builder);

    /*!
     * \brief parseMessage 解析报文
     * \param builder 构建器
     * \retval 解析成功与否
     */
    bool parseMessage(Builder &builder);

    /*!
     * \brief parseSignal 解析信号
     * \param builder 构建器
     * \retval 解析成功与否
     */
    bool parseSignal(Builder &builder);

    /*!
     * \brief parseValueTable 解析值表
     * \param builder 构建器
     * \retval 解析成功与否
     */
    bool parseValueTable(Builder &builder);

    /*!
     * \brief parseAttrDefine 解析属性定义
     * \param builder 构建器
     * \retval 解析成功与否
     */
    bool parseAttrDefine(Builder &builder);

    /*!
     * \brief parseAttrDefaultDefine 解析属性默认值定义
     * \param builder 构建器
     * \retval 解析成功与否
     */
    bool parseAttrDefaultDefine(Builder &builder);

    /*!
     * \brief parseAttrValue 解析属性值
     * \param builder 构建器
     * \retval 解析成功与否
     */
    bool parseAttrValue(Builder &builder);

    /*!
     * \brief parseValue 解析值描述
     * \param builder 构建器
     * \retval 解析成功与否
     */
    bool parseValue(Builder &builder);

    /*!
     * \brief parseBoTxBu 解析单报文多发送节点
     * \param builder 构建器
     * \return 解析成功与否
     */
    bool parseBoTxBu(Builder &builder);

    /*!
     * \brief parseComment 解析注释
     * \param builder 构建器
     * \return 构建成功与否
     */
    bool parseComment(Builder &builder);

    /*!
     * \brief doParseValueDesc 进行解析值描述
     * \param [out] valueDesc 接收解析后的结果
     * \retval 解析成功与否
     */
    bool doParseValueDesc(QList<Builder::ValueTableEntry> &valueDesc);

private:

    /*!
     * \brief getNextIdentifier 获取下一个标识符
     * \param [out] nextIdentifier  接收下一个标识符
     * \param [in] eol true - 检测到行尾则终止， false - 只有检测到eof才终止
     * \retval 是否接收到
     */
    bool getNextIdentifier(QString &nextIdentifier, bool eol=true);

    /*!
     * \brief getNextString 获取下一个字符串
     * \param [out] nextString 接收下一个字符串
     * \retval 是否接收到
     */
    bool getNextString(QString &nextString);

    /*!
     * \brief getNextSemicolon 获取一个封号';'
     * \retval 是否接收到
     */
    bool getNextSemicolon();

    /*!
     * \brief getNextColon 获取下一个冒号':'
     * \retval 是否接收到
     */
    bool getNextColon();

    /*!
     * \brief getNextComma 获取下一个逗号','
     * \retval 是否接收到
     */
    bool getNextComma();

    /*!
     * \brief getNextInt 获取下一个整数
     * \param [out] intVal 接收下一个整数
     * \param [out] eol true - 检测到行尾自动终止，false - 扫描到行尾没有扫描到，则换行扫描
     * \retval  是否接收到
     */
    bool getNextInt(qint32 &intVal, bool eol=true);

    /*!
     * \brief getNextUnsignedInt 获取下一个无符号整数
     * \param [out] uintVal 接收下一个无符号整数
     * \param [out] eol true - 检测到行尾自动终止，false - 扫描到行尾没有扫描到，则换行扫描
     * \retval 是否接收到
     */
    bool getNextUnsignedInt(quint32 &uintVal, bool eol=true);

    /*!
     * \brief getNextDouble 获取下一个小数值
     * \param [out] doubleVal 接收下一个小数值
     * \param [out] eol true - 检测到行尾自动终止，false - 扫描到行尾没有扫描到，则换行扫描
     * \return 是否接收到
     */
    bool getNextDouble(double &doubleVal, bool eol=true);

    /*!
     * \brief getNextNewSymbol 获取下一个New Symbol
     * \param [out] newSymbol 接收下一个New Symbol
     * \retval 是否接收到
     */
    bool getNextNewSymbol(QString &newSymbol);

    /*!
     * \brief getNextChar 获取下一个字符
     * \param ch   要获取的字符
     * \param desc 描述信息
     * \param eol  以换行符作为终止字符
     * \retval 是否接收到
     */
    bool getNextChar(QChar ch, QString desc, bool eol=true);

    /*!
     * \brief getNextChar 获取下一个在集合中中的字符
     * \param set   集合
     * \param desc  描述
     * \param [out] out 接收收到的字符
     * \param eol   以换行符作为终止字符
     * \retval 是否接收到
     */
    bool getNextChar(const QString set, const QString &desc, QChar &out, bool eol=true);

    /*!
     * \brief getNextValue  获取下一个值
     * \param [out] iret    true - 则说明返回值是ival
     * \param [out] ival    接收读到的整数值
     * \param [out] dret    true - 则说明返回值是dval
     * \param [out] dval    接收读到的小数值
     * \param [out] sret    true - 则说明返回值是sval
     * \param [out] sval    接收读到的字符串值
     * \return 是否接收到
     */
    bool getNextValue(bool &iret, qint32 &ival, bool &dret, double &dval,
                      bool &sret, QString &sval);

private:
    ParseMethodMap parseMethodMap;
};

END_DBC4CPP_NAMESPACE

#endif // PARSER_H
