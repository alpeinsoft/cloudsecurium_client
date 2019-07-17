#include <QtCore>
#include "encrypted_folder.h"

/*
extern "C" {
    #include "cryptfs.h"
}
*/
EncryptedFolder::EncryptedFolder(QString mount_path)
{
    LOG("start dummy: %s\n", mount_path.toAscii().data());
}

EncryptedFolder::~EncryptedFolder()
{
    LOG("kill dummy\n");
}

bool EncryptedFolder::checkKey(const QString &folder) {
    LOG("Checking for key in %s\n", folder.toAscii().data());
    return QDir(folder).exists(QString(".key"));
}

void EncryptedFolder::generateKey(const QString &folder) {
    if (!QDir(folder).exists())
        QDir(folder).mkpath(".");
    QFile file(QDir::toNativeSeparators(QDir(folder).absolutePath()) + QDir::separator() + ".key");
    file.open(QIODevice::WriteOnly);
    file.close();
    LOG("Kinda generated key in %s\n", QDir(folder).entryList(QDir::NoDotAndDotDot).join(QString(" ")).toAscii().data());
}
