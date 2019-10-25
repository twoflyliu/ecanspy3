#ifndef SIGNALACCESSOR_H
#define SIGNALACCESSOR_H

#include "dbc4cpp/parser.h"
#include "UsbCanUtil.h"
#include <stdint.h>

namespace SignalAccessor
{
    quint64 getSignalValue(const CAN_OBJ *obj, const Signal *signal);
    void setSignalValue(CAN_OBJ *obj, Signal *signal, quint64 value);

    QString getSignalPhysicalValue(const CAN_OBJ *obj, Signal *signal);
}

#endif // SIGNALACCESSOR_H
