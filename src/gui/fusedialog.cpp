#include "fusedialog.h"
#include <QUrl>
#include <QTimer>
#include <QDesktopServices>
#include <QMessageBox>
#include <common/utility.h>
#include <QApplication>
#include <QProcess>

static const QString osxfuse_url = QString("https://updates.securium.ch/csdc/osxfuse.pkg");
static const QString dokan_url = QString("");
static const QString osx_download_path = QString("/tmp/osxfuse.pkg");
static const QString win_download_path = QString("");

bool fuseInstallDialog(bool proposeOnly)
{
    QString title = QString("Folder encryption");
    QString text = QString(
            "An %1 package is required to support local folder encryption. "
            "Would you like to download and install %1 now? In case of decline, "
            "local folder encryption will not work.");
#if defined(Q_OS_MAC)
    text = text.arg("OSXFUSE");
#else
    text = text.arg("DOKAN");
#endif
    auto ans = QMessageBox::question(nullptr, title, text);
    if (proposeOnly)
        return  (ans == QMessageBox::Yes) ? true : false;
    if (ans == QMessageBox::Yes)
       fuseInstallResultDialog(fuseInstall());
}

bool fuseDownload()
{
#if defined(Q_OS_MAC)
    QString cmd = QString("curl -o %1 %2").arg(osx_download_path, osxfuse_url);
#else
    // fix it for windows
    QString cmd = QString("curl -o %1 %2").arg(win_download_path, dokan_url);
#endif
    return system(cmd.toUtf8().data());
}

install_status fuseInstall()
{
    if (fuseDownload()) {
        LOG("downloadFuse returned nonzero");
        return download_error;
    }
#if defined(Q_OS_MAC)
    int rc = QProcess::startDetached(
                QString("open -a Installer ") + osx_download_path);
#else
    int rc = QDesktopServices::openUrl(QUrl::fromLocalFile(win_download_path));
#endif
    return rc ? ok : install_error;
}

void fuseInstallResultDialog(install_status status)
{
    QString text;
    switch(status) {
    case ok:
        QApplication::quit();
        LOG("after QAppliacation::quit()\n");
    case download_error:
        text = QString("Download error.");
        break;
    case install_error:
        text = QString("Can't run installer.");
    }
    QMessageBox::critical(nullptr, "installer", text);
}
