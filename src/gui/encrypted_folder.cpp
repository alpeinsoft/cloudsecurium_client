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
                this->mount_path.toUtf8().data(),
                password
                );
    if (mount_rc) {
        perror("cryptfs_mount");
        return;
    }
    LOG("mount_rc: %d\n", mount_rc);
    this->mount_path += QString("/");
    this->loop = new CryptfsLoop(this->cfs);
    LOG("Created cryptfs loop at %lu\n", this->loop);
    this->loop->start();
    LOG("loop started\n");
    this->loop->wait(100);
    LOG("loop waited\n");
    if (!isRunning())
        LOG("Cryptfs: failed to run loop\n");
    else {
        LOG("Cryptfs: loop runs successfully!\n");
#ifdef __apple__
        this->mount_number++;
#endif
    }
}

EncryptedFolder::~EncryptedFolder()
{
    if (!isRunning())
        return;
    LOG("killing dummy...\n");
    int rc = cryptfs_ummount(this->cfs);
    QThread::sleep(3);
    LOG("after unmount %d\n", rc);
    loop->wait(100);
    LOG("Waited for loop to stop\n");
    if (loop->isRunning()) {
        LOG("terminating loop\n");
        loop->terminate();
    }
    LOG("loop terminated\n");
    delete this->loop;
    this->loop = nullptr;
    LOG("loop deleted\n");
    QString path = QString(this->mountPath());
    LOG("var path\n");
    path.chop(1);
    LOG("path chop\n");
    QDir d(path);
    LOG("create qdir\n");
    rc = d.removeRecursively();
    LOG("removed the directory %d\n", rc);
    cryptfs_free(this->cfs);
    this->cfs = nullptr;
    LOG("cryptfs_free\n");
}

bool EncryptedFolder::isRunning()
{
    if (this->loop != nullptr)
        return this->loop->isRunning();
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
    LOG("folder now exists but .key doesn't\n");
    QString path = QString(
            QDir::toNativeSeparators(folder.absolutePath())
            + QDir::separator()
            + QString(".key")
            );
    LOG("got path  %s\n", path.toUtf8().data());
    LOG("send path %s to cryptfs_generate_key_file\n", path.toUtf8().data());
    cryptfs_generate_key_file(passwd, path.toUtf8().data());
    LOG(
            "Kinda generated key in %s with passwd %s\n",
            folder.entryList(QDir::NoDotAndDotDot).join(QString(" ")).toUtf8().data(),
            passwd
            );
}

QString EncryptedFolder::generateMountPath(const QString &folder)
{
    QString path = QString(folder);
    if (path.endsWith("/"))
        path.chop(1);
    path += QString("_UNCRYPT");
#ifdef __apple__
    path += QString::number(this->mount_number);
#endif
    QDir uncr(QDir::toNativeSeparators(path));
    if (uncr.exists())
        uncr.removeRecursively();
    uncr.mkpath(".");
    //path += QString(QDir::separator());
    LOG("Created %s\n", path.toUtf8().data());
    LOG("folder: %s\n", folder.toUtf8().data());
    return path;
}

QString EncryptedFolder::mountPath() {
    return mount_path;
}
