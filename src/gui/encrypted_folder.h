#ifndef ENCRYPTED_FOLDER_H
#define ENCRYPTED_FOLDER_H

#include <QThread>
#include <QtCore>
#include "common/utility.h"
extern "C" {
    #include "cryptfs.h"
}


class EncryptedFolder
{
    QString mount_path, encryption_path, key_path;
    struct cryptfs* cfs = nullptr;
    int mount_rc = -1;
    class CryptfsLoop: public QThread {
    public:
        cryptfs *cfs = nullptr;

        CryptfsLoop(cryptfs* cfs)
        {
            this->cfs = cfs;
        }

        void run()
        {
            setTerminationEnabled(true);
            LOG("settermination true\n");
            cryptfs_loop(cfs);
            LOG("cryptfs_loop finished\n");
#ifdef __APPLE__
            cryptfs_ummount(cfs);
            LOG("ummount finished\n");
#endif
        }
    } *loop = nullptr;
public:
    EncryptedFolder(QString mount_path, const char *password);
    ~EncryptedFolder();
    bool isRunning();

    static bool checkKey(const QString &folder);
    static void generateKey(const QString &folder, const char *password);
    static void mkEncryptionDir(const QString path);
    static QString uncryptPathByEncryptPath(const QString path);
    QString mountPath();
};

void nullifyString(QString password);
void nullifyString(QByteArray *password);

#endif
