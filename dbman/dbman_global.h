#ifndef DBMAN_GLOBAL_H
#define DBMAN_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(DBMAN_LIBRARY)
#define DBMAN_EXPORT Q_DECL_EXPORT
#else
#define DBMAN_EXPORT Q_DECL_IMPORT
#endif

#endif // DBMAN_GLOBAL_H
