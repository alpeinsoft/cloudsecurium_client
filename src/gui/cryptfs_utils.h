#ifndef CRYPTFS_UTILS_H
#define CRYPTFS_UTILS_H

#define CRYPTFS_UTILS_DEBUG

#ifdef CRYPTFS_UTILS_DEBUG
    #define LOG(format, ...) fprintf(stderr, format, ##__VA_ARGS__)
#else 
    #define LOG(format, ...)
#endif


#include <QtCore>


bool is_encrypted(const QString &folder);
bool can_be_encrypted(const QString &folder);

#endif