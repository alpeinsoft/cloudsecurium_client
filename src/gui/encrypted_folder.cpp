#include <QtCore>
#include "encrypted_folder.h"
#include "cryptfs_utils.h"

/*
extern "C" {
    #include "cryptfs.h"
}
*/
EncryptedFolder::EncryptedFolder(QString crypted_path, QString mount_path, QString password)
{
    LOG("start dummy: %s -> %s\n : %s", crypted_path.toAscii().data(), mount_path.toAscii().data(), password.toAscii().data());
}

EncryptedFolder::~EncryptedFolder()
{
    LOG("kill dummy\n");
}
