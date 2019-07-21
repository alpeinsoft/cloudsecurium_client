#include <QtCore>
#include <QInputDialog>
#include "encrypted_folder.h"

extern "C" {
    #include "cryptfs.h"
}

EncryptedFolder::EncryptedFolder(QString local_path)
{
    LOG("start dummy: with mnt_path %s\n", local_path.toAscii().data());
    if (!checkKey(local_path))
        return;
    this->encryption_path = QString(local_path);
    LOG("Cryptfs: encryption_path = %s\n", this->encryption_path.toAscii().data());

    this->key_path = QString(this->encryption_path + QString(".key"));
    LOG("Cryptfs: key_path = %s\n", this->key_path.toAscii().data());

    this->mount_path = EncryptedFolder::generateMountPath(local_path);
    LOG("Cryptfs: mount_path = %s\n", this->mount_path.toAscii().data());

    this->cfs = cryptfs_create(
                this->encryption_path.toAscii().data(),
                this->key_path.toAscii().data()
                );
    if (this->cfs == nullptr)
        return;
    LOG("Cryptfs: created structure successfully at addr %lu\n", this->cfs);

    do {
        QString password = QInputDialog::getText(
                    nullptr,
                    QString("Password"),
                    QString("Password for "+local_path),
                    QLineEdit::Password
                    );
        LOG("got: %s\n", password.toAscii().data());

        mount_rc = cryptfs_mount(
                    cfs,
                    mount_path.toAscii().data(),
                    password.toAscii().data()
                    );
        if (mount_rc)
            perror("cryptfs_mount");
        LOG("got: %d\n", mount_rc);
    } while (mount_rc);
    LOG("Cryptfs: mount successful at %s\n", mount_path.toAscii().data());

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
    LOG("Checking for key in %s\n", folder.toAscii().data());
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
            ).toAscii().data();
    cryptfs_generate_key_file(passwd, path);
    LOG(
            "Kinda generated key in %s with passwd %s\n",
            folder.entryList(QDir::NoDotAndDotDot).join(QString(" ")).toAscii().data(),
            passwd
            );
}

QString EncryptedFolder::generateMountPath(const QString &folder)
{
    QString tmp = QString(folder);
    tmp.chop(1);
    tmp.append(QString("_UNCRYPT")+QDir::separator());
    LOG("Created %s\n", tmp.toAscii().data());
    return tmp;
}
