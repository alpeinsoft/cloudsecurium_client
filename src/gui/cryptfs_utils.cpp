#include "cryptfs_utils.h"
#include <QtCore>


bool is_encrypted(const QString &folder)
{
    #if 0//CRYPTFS_UTILS_DEBUG
        LOG("~/storage: folder %d, has key %d", QDir(QString("/home/lyavon/storage")).exists(), QDir("/home/lyavon/storage").exists(QString(".key")));
        LOG("~/storage/git: folder %d, has key %d", QDir(QString("/home/lyavon/storage/git")).exists(), QDir("/home/lyavon/storage/git").exists(QString(".key")));
    #endif

    return QDir(folder).exists() && QDir(folder).exists(QString(".key"));
}


bool can_be_encrypted(const QString &folder)
{
    return
        !QDir(folder).exists() ||
        QDir(folder).entryInfoList(
            QDir::NoDotAndDotDot
            | QDir::AllEntries
        ).count() == 0;
}