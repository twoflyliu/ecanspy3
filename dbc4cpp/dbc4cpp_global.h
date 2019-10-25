#ifndef DBC4CPP_GLOBAL_H
#define DBC4CPP_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(DBC4CPP_LIBRARY)
#  define DBC4CPPSHARED_EXPORT Q_DECL_EXPORT
#else
#  define DBC4CPPSHARED_EXPORT Q_DECL_IMPORT
#endif

#include "dbc4cpp_config.h"

#endif // DBC4CPP_GLOBAL_H
