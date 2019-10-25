#ifndef CONTEXT_H
#define CONTEXT_H

#include "dbc4cpp_global.h"

#include <QString>

BEGIN_DBC4CPP_NAMESPACE

/*!
 * \brief Context类保存解析过程中的上下文
 */
class Context
{
public:
    enum State{
        STATE_OK = 0,           //!< 当内部解析都正确的时候，为此状态
        STATE_ERR              //!< 当内部解析出错的时候，为此状态
    };

    /*!
     * \brief Context 默认构造器
     */
    Context();

    /*!
     * \brief getDBCFileName 获取当前解析的DBC文件名称
     *
     * 当解析出错的时候，可以调用此方法来获取温习出错的DBC文件名称
     *
     * \retval 当前解析的DBC文件名称
     */
    const QString& getDBCFileName();

    /*!
     * \brief setDBCFileName 设置当前解析的DBC文件名称
     * \param dbcFileName    要设置的DBC文件名称
     */
    void setDBCFileName(const QString &dbcFileName);

    /*!
     * \brief getCurLineNo   获取当前正在解析的行号
     *
     * 当解析出错的时候，可以调用此方法来获取解析出错的行号
     *
     * \retval 当前正在解析的行号
     */
    int getCurLineNo() const;

    /*!
     * \brief setCurLineNo   设置当前正在解析的行号
     * \param curLineNo      要设置的当前正在解析的行号
     */
    void setCurLineNo(int curLineNo);

    /*!
     * \brief getCurColNo 获取当前列号
     * \retval 当前列号
     */
    int getCurColNo() const;

    /*!
     * \brief setCurColNo 设置当前列号
     * \param curColNo 当前列号
     */
    void setCurColNo(int curColNo);

    /*!
     * \brief operator bool  检测当前是否解析出错
     * \retval 如果没有出错，则返回true, 否则返回false
     */
    operator bool() const;

    /*!
     * \brief getErrMsg 获取解析出错的消息
     * \retval 解析出错的消息
     */
    const QString &getErrMsg() const;

    /*!
     * \brief setErrMsg 设置错误消息
     * \param errMsg    要设置的错误消息
     */
    void setErrMsg(const QString &errMsg);

    /*!
     * \brief clrErrMsg 清除消息
     */
    void clrErrMsg();

    /*!
     * \brief getCurLine 获取当前行
     * \retval 当前行
     */
    const QString &getCurLine() const;

    /*!
     * \brief setCurLine 设置当前行
     * \param newCurLine 新的当前行
     */
    void setCurLine(const QString newCurLine);

    /*!
     * \brief moveNextLine 移到下一行
     */
    void moveNextLine();

    /*!
     * \brief movePrevLineLastPos 移动前一行的最后一个位置
     */
    void movePrevLineLastPos();

    /*!
     * \brief movePrevCol 移动到前一列
     */
    void movePrevCol();

    /*!
     * \brief moveNextCol 移动到下一列
     */
    void moveNextCol();

private:
    QString dbcFileName;        //!< 解析DBC文件名称

    int     curLineNo;          //!< 当前报文行号

    int     curColNo;           //!< 当前列号
    int     preColNo;           //!< 之前的列好

    QString curLine;            //!< 当前行内容

    State   state;              //!< 当前状态
    QString errMsg;             //!< 当包状态为出错的时候，保存出错消息
};

END_DBC4CPP_NAMESPACE

#endif // CONTEXT_H
