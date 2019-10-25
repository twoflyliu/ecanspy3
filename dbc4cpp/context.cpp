#include "context.h"

BEGIN_DBC4CPP_NAMESPACE

Context::Context() : state(STATE_OK)
{

}

const QString &Context::getDBCFileName()
{
    this->state = STATE_OK;
    return this->dbcFileName;
}

void Context::setDBCFileName(const QString &dbcFileName)
{
    this->dbcFileName = dbcFileName;
}

int Context::getCurLineNo() const
{
    return this->curLineNo;
}

void Context::setCurLineNo(int curLineNo)
{
    this->curLineNo = curLineNo+1;
}

int Context::getCurColNo() const
{
    return this->curColNo;
}

void Context::setCurColNo(int curColNo)
{
    this->curColNo = curColNo;
    this->preColNo = curColNo;
}

const QString &Context::getErrMsg() const
{
    return this->errMsg;
}

void Context::setErrMsg(const QString &errMsg)
{
    this->state = STATE_ERR;
    this->errMsg = errMsg;
}

void Context::clrErrMsg()
{
    this->state = STATE_OK;
    this->errMsg.clear();
}

const QString &Context::getCurLine() const
{
    return this->curLine;
}

void Context::setCurLine(const QString newCurLine)
{
    this->curLine = newCurLine;
}

void Context::moveNextLine()
{
    this->curLineNo++;
    this->curColNo = 0;
}

void Context::movePrevLineLastPos()
{
    this->curLineNo--;
    this->curColNo = this->preColNo;
}

void Context::movePrevCol()
{
    this->curColNo--;
}

void Context::moveNextCol()
{
    this->preColNo = this->curColNo;
    this->curColNo++;
}

Context::operator bool() const
{
    return STATE_OK == this->state;
}

END_DBC4CPP_NAMESPACE
