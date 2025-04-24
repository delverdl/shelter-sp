#ifndef UISHELTER_GLOBAL_H
#define UISHELTER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(UISHELTER_LIBRARY)
#define UISHELTER_EXPORT Q_DECL_EXPORT
#else
#define UISHELTER_EXPORT Q_DECL_IMPORT
#endif

#endif // UISHELTER_GLOBAL_H
