#include <QtCore>
#include <QInputDialog>
#include "encrypted_folder.h"

extern "C" {
    #include "cryptfs.h"
}

EncryptedFolder::EncryptedFolder(QString local_path, char *password)
{
    LOG("start dummy: with mnt_path %s\n", local_path.toUtf8().data());
    if (!checkKey(local_path))
        return;
    this->encryption_path = QString(local_path);
    LOG("Cryptfs: encryption_path = %s\n", this->encryption_path.toUtf8().data());

    this->key_path = QString(this->encryption_path + QString(".key"));
    LOG("Cryptfs: key_path = %s\n", this->key_path.toUtf8().data());

    this->mount_path = EncryptedFolder::generateMountPath(local_path);
    LOG("Cryptfs: mount_path = %s\n", this->mount_path.toUtf8().data());

    this->cfs = cryptfs_create(
                this->encryption_path.toUtf8().data(),
                this->key_path.toUtf8().data()
                );
    if (this->cfs == nullptr)
        return;
    LOG("Cryptfs: created structure successfully at addr %lu\n", this->cfs);

    mount_rc = cryptfs_mount(
                cfs,
                mount_path.toUtf8().data(),
                password
                );
    if (mount_rc) {
        perror("cryptfs_mount");
        return;
    }
    LOG("got: %d\n", mount_rc);

    loop = new CryptfsLoop(this->cfs);
    loop->start();
    loop->wait(100);
    if (!isRunning())
        LOG("Cryptfs: failed to run loop\n");
    else
        LOG("Cryptfs: loop runs successfully!\n");
}

EncryptedFolder::~EncryptedFolder()
{
    if (!isRunning())
        return;
    LOG("killing dummy...\n");
    LOG("after unmount %d\n", cryptfs_ummount(this->cfs));
    loop->wait(100);
    if (loop->isRunning())
        loop->terminate();
    delete loop;
    cryptfs_free(this->cfs);
    LOG("kill dummy\n");
}

bool EncryptedFolder::isRunning()
{
    if (loop != nullptr)
        return loop->isRunning();
    return false;
}

bool EncryptedFolder::checkKey(const QString &folder) {
    LOG("Checking for key in %s\n", folder.toUtf8().data());
    return QDir(folder).exists(QString(".key"));
}

void EncryptedFolder::generateKey(const QString &folder_path, char* passwd) {
    QDir folder = QDir(folder_path);
    if (!folder.exists())
        folder.mkpath(".");
    if (folder.exists(QString(".key")))
        return;
    char* path = QString(
            QDir::toNativeSeparators(folder.absolutePath())
            + QDir::separator()
            + QString(".key")
            ).toUtf8().data();
    cryptfs_generate_key_file(passwd, path);
    LOG(
            "Kinda generated key in %s with passwd %s\n",
            folder.entryList(QDir::NoDotAndDotDot).join(QString(" ")).toUtf8().data(),
            passwd
            );
}

QString EncryptedFolder::generateMountPath(const QString &folder)
{
    QString tmp = QString(folder);
    tmp.chop(1);
    tmp.append(QString("_UNCRYPT")+QDir::separator());
    LOG("Created %s\n", tmp.toUtf8().data());
    return tmp;
}
