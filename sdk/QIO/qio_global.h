#ifndef QIO_GLOBAL_H
#define QIO_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QIO_LIBRARY)
#  define QIOSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QIOSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // QIO_GLOBAL_H
