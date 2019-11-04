#ifndef FUSEDIALOG_H
#define FUSEDIALOG_H

#include <QString>

enum install_status {ok, download_error, install_error};

bool fuseInstallDialog(bool proposeOnly=false);
void fuseInstallResultDialog(install_status status);
install_status fuseInstall();

#endif // FUSEDIALOG_H
