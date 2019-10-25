#include "signalaccessor.h"

namespace {

//Intel Signal内的起始位是最低位
//Intel Signal内的起始位是最高位

quint64 getSignalValueIntel(const CAN_OBJ *obj, const Signal *signal)
{
    quint64 value = 0;

    // 起始长度(MSB行）
    int startRowIndex = signal->getStartBit() / 8;
    int startBegColIndex = signal->getStartBit() % 8;
    int startRowBitLen = (8 - startBegColIndex);
    int startRowOffsetBit = startBegColIndex;

    if (startRowBitLen > signal->getBitLen()) {
        startRowBitLen = signal->getBitLen();
    }

    //最低行(LSB)
    value = (obj->Data[startRowIndex]>>startRowOffsetBit) & ((1<<startRowBitLen)-1);

    // 中间行
    int curRowIndex = startRowIndex + 1;
    int offsetBit = startRowBitLen;
    int midRowCnt = (signal->getBitLen() - startRowBitLen) / 8;
    for (int i = 0; i < midRowCnt; i++) {
        value |= (obj->Data[curRowIndex++] << offsetBit);
        offsetBit += 8;
    }

    // 最后一行
    int lastRowBitLen = signal->getBitLen() - (8*midRowCnt) -startRowBitLen;
    if (lastRowBitLen > 0) {
        value |= (obj->Data[curRowIndex] & ((1 << lastRowBitLen)-1)) << offsetBit;
    }

    return value;
}

quint64 getSignalValueMotorola(const CAN_OBJ *obj, const Signal *signal)
{
    quint64 value = 0;

    // 起始长度(MSB行）
    int startRowIndex = signal->getStartBit() / 8;
    int startEndColIndex = signal->getStartBit() % 8;
    int startRowBitLen = startEndColIndex + 1;
    int startRowMaxBitLen = startRowBitLen;
    int startRowOffsetBit = 0;

    if (signal->getBitLen() < startRowBitLen) {
        startRowOffsetBit = startRowBitLen - signal->getBitLen();
        startRowBitLen = signal->getBitLen();
    }

    //最低行(MSB)
    value = (obj->Data[startRowIndex] & ((1<<startRowMaxBitLen)-1)) >> startRowOffsetBit;

    // 中间行
    int curRowIndex = startRowIndex + 1;
    int midRowCnt = (signal->getBitLen() - startRowBitLen) / 8;
    int offsetBit = startRowBitLen;
    for (int i = 0; i < midRowCnt; i++) {
        value = (value << offsetBit) | obj->Data[curRowIndex++];
        offsetBit = 8;
    }

    // 最后一行
    int lastRowBitLen = signal->getBitLen() - (8*midRowCnt) - startRowBitLen;
    if (lastRowBitLen > 0) {
        value = (value << lastRowBitLen) | (obj->Data[curRowIndex]>>(8-lastRowBitLen));
    }

    return value;
}

void setSignalValueIntel(CAN_OBJ *obj, Signal *signal, quint64 value)
{
    // 起始长度(MSB行）信息
    int startRowIndex = signal->getStartBit() / 8;
    int startBegColIndex = signal->getStartBit() % 8;
    int startRowBitLen = (8 - startBegColIndex);
    int startRowOffsetBit = startBegColIndex;

    if (startRowBitLen > signal->getBitLen()) {
        startRowBitLen = signal->getBitLen();
    }

    // 中间行信息
    int midRowCnt = (signal->getBitLen() - startRowBitLen) / 8;

    // 最后行信息
    int lastRowBitLen = signal->getBitLen() - (8*midRowCnt) - startRowBitLen;
    int curRowIndex = startRowIndex;

    // 设置最低行(LSB)
    int mask = ((1<<startRowBitLen)-1) << startRowOffsetBit;
    obj->Data[curRowIndex] &= ~mask;
    obj->Data[curRowIndex++] |= (value & ((1<<startRowBitLen)-1)) << startRowOffsetBit;
    value >>= startRowBitLen;

    // 设置中间行
    for (int i = 0; i < midRowCnt; i++) {
        obj->Data[curRowIndex++] = value & 0xff;
        value >>= 8;
    }

    // 设置最后行
    if (lastRowBitLen > 0) {
        mask = ((1<<lastRowBitLen)-1);
        obj->Data[curRowIndex] &= ~mask;
        obj->Data[curRowIndex] |= value & ((1<<lastRowBitLen)-1);
    }
}

void setSignalValueMotorola(CAN_OBJ *obj, Signal *signal, quint64 value)
{
    // 起始长度(MSB行）信息
    int startRowIndex = signal->getStartBit() / 8;
    int startEndColIndex = signal->getStartBit() % 8;
    int startRowBitLen = startEndColIndex + 1;
    int startRowMaxBitLen = startRowBitLen;
    int startRowOffsetBit = 0;
    int mask = 0;

    if (startRowBitLen > signal->getBitLen()) {
        startRowOffsetBit = startRowBitLen - signal->getBitLen();
        startRowBitLen = signal->getBitLen();
    }

    // 中间行信息
    int midRowCnt = (signal->getBitLen() - startRowBitLen) / 8;

    // 最后行信息
    int lastRowBitLen = signal->getBitLen() - (8*midRowCnt) - startRowBitLen;

    int curRowIndex = startRowIndex;

    // 设置起始行（MSB）
    mask = (((1<<startRowMaxBitLen)-1) >> startRowOffsetBit) << startRowOffsetBit;
    obj->Data[curRowIndex] &= ~mask;
    obj->Data[curRowIndex++] |=  ((value >> (signal->getBitLen()-startRowBitLen))&((1<<startRowBitLen)-1))
            << startRowOffsetBit;

    // 设置中间行
    for (int i = 0; i < midRowCnt; ++i) {
        obj->Data[curRowIndex++] = (value >> (lastRowBitLen+(midRowCnt-1-i)*8)) & 0xff;
    }

    // 设置最后行
    if (lastRowBitLen > 0) {
        mask = ((1<<lastRowBitLen)-1) << (8-lastRowBitLen);
        obj->Data[curRowIndex] &= ~mask;
        obj->Data[curRowIndex] |= (value & ((1<<lastRowBitLen)-1)) << (8-lastRowBitLen);
    }
}

}



quint64 SignalAccessor::getSignalValue(const CAN_OBJ *obj, const Signal *signal)
{
    if (NULL == obj || NULL == signal) return 0;
    switch (signal->getByteOrder()) {
    case Signal::BYTE_ORDER_TYPE_MOTOROLA:
        return getSignalValueMotorola(obj, signal);
    case Signal::BYTE_ORDER_TYPE_INTEL:
        return getSignalValueIntel(obj, signal);
    }
    return 0;
}

void SignalAccessor::setSignalValue(CAN_OBJ *obj, Signal *signal, quint64 value)
{
    if (NULL == obj || NULL == signal) return;
    switch (signal->getByteOrder()) {
    case Signal::BYTE_ORDER_TYPE_MOTOROLA:
        return setSignalValueMotorola(obj, signal, value);
    case Signal::BYTE_ORDER_TYPE_INTEL:
        return setSignalValueIntel(obj, signal, value);
    }
}

QString SignalAccessor::getSignalPhysicalValue(const CAN_OBJ *obj, Signal *signal)
{
    if (NULL == obj || NULL == signal) return QString("");
    quint64 rawValue = getSignalValue(obj, signal);

    auto &tbl = signal->getValueDescTable();
    auto iter = tbl.find(rawValue);
    if (iter != tbl.end()) {
        return iter.value().trimmed();
    }
    return QString("%1 %2").arg(signal->getFactor()*rawValue + signal->getOffset()).arg(signal->getUnit());
}
