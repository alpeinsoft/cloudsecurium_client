#include "fusedialog.h"
#include <QUrl>
#include <QTimer>
#include <QDesktopServices>
#include <QMessageBox>
#include <common/utility.h>
#include <QApplication>
#include <QProcess>
#include <QDir>

Q_LOGGING_CATEGORY(lcFuseDialog, "cloudsecurium.gui.localFolderEncryption", QtInfoMsg)
#define CURRENT_LC lcFuseDialog

static const QString osxfuseUrl = QString("https://updates.securium.ch/csdc/osxfuse.pkg");
static const QString osxDownloadPath = QString("/tmp/osxfuse.pkg");

#ifdef _WIN32
#include <QSettings>
static QSettings m("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\"
        "Windows\\CurrentVersion\\App Paths\\cloudsecurium.exe",
        QSettings::NativeFormat);
static const QString winDownloadPath =
        m.value("Default").toString().replace("cloudsecurium", "dokan");
#endif

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
#ifndef _WIN32
    QString cmd = QString("curl -o %1 %2").arg(osxDownloadPath, osxfuseUrl);
    return system(cmd.toUtf8().data());
#else
    LOG("Got: %s\n", winDownloadPath.toUtf8().data());
    return !QFile::exists(winDownloadPath);
#endif
}

install_status fuseInstall()
{
    if (fuseDownload()) {
        ERROR("downloadFuse returned nonzero");
        return download_error;
    }
#ifndef _WIN32
    int rc = QProcess::startDetached(
                QString("open -a Installer ") + osxDownloadPath);
#else
    int rc = QDesktopServices::openUrl(QUrl::fromLocalFile(winDownloadPath));
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
