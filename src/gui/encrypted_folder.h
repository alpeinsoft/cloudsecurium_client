#ifndef ENCRYPTED_FOLDER_H
#define ENCRYPTED_FOLDER_H

#include <QtCore>/*
extern "C" {
    #include "cryptfs.h"
}
*/

#define CRYPTFS_UTILS_DEBUG

#ifdef CRYPTFS_UTILS_DEBUG
    #define LOG(format, ...) fprintf(stderr, format, ##__VA_ARGS__)
#else
    #define LOG(format, ...)
#endif

class EncryptedFolder
{
public:
    EncryptedFolder(QString mount_path);
    ~EncryptedFolder();

    static bool checkKey(const QString &folder);
    static void generateKey(const QString &folder);
};

#endif
