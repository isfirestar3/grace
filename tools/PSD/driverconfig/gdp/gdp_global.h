#ifndef GDP_GLOBAL_H
#define GDP_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef GDP_LIB
# define GDP_EXPORT Q_DECL_EXPORT
#else
# define GDP_EXPORT Q_DECL_IMPORT
#endif

#endif // GDP_GLOBAL_H
