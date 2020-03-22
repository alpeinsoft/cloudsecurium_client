#ifndef ENCRYPTED_FOLDER_H
#define ENCRYPTED_FOLDER_H

#include <QThread>
#include <QtCore>
#include "common/utility.h"
extern "C" {
    #include "cryptfs.h"
}

#define CRYPTFS_UTILS_DEBUG

#ifdef CRYPTFS_UTILS_DEBUG
    #define LOG(format, ...) fprintf(stderr, format, ##__VA_ARGS__)
#else
    #define LOG(format, ...)
#endif

class EncryptedFolder
{
#ifdef __apple__
    static int mount_number;
#endif
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
        }
    } *loop = nullptr;
public:
    EncryptedFolder(QString mount_path, char *password);
    ~EncryptedFolder();
    bool isRunning();

    static bool checkKey(const QString &folder);
    static void generateKey(const QString &folder, char *password);
    QString generateMountPath(const QString &folder);
    QString mountPath();
};

#endif
