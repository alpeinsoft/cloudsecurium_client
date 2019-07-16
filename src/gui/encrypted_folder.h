#ifndef ENCRYPTED_FOLDER_H
#define ENCRYPTED_FOLDER_H

#include <QtCore>/*
extern "C" {
    #include "cryptfs.h"
}
*/
class EncryptedFolder
{
public:
    EncryptedFolder(QString crypted_path, QString mount_path, QString password);
    ~EncryptedFolder();
};

#endif
