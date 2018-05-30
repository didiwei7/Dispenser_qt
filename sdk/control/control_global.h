#ifndef CONTROL_GLOBAL_H
#define CONTROL_GLOBAL_H

#include <QtCore/qglobal.h>
#include <Windows.h>

#if defined(CONTROL_LIBRARY)
#  define CONTROLSHARED_EXPORT Q_DECL_EXPORT
#else
#  define CONTROLSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // CONTROL_GLOBAL_H
