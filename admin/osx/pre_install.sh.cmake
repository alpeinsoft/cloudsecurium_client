#!/bin/bash

# kill the old version. see issue #2044
killall @APPLICATION_EXECUTABLE@

if [[ "@ADD_ENCRYPTION@" -ne 1 ]]
then
    exit 0
fi

if [[ -n $(pkgutil --pkgs | grep -i osxfuse.pkg.Core) ]]
then
    exit 0
fi

ru_fuse_dialog="\
osascript -e \
'display alert \"Требуется установить пакет OSXFUSE для поддержки шифрования локальной папки. \
Желаете ли вы сейчас установить OSXFUSE? В случае отказа локальное шифрование папки работать не будет.\" \
buttons {\"Нет\", \"Да\"} as warning' \
-e 'if button returned of result = \"Нет\" then' \
-e '    error 1' \
-e 'end if' 2>/dev/null \
"
ru_error_msg="\
В процессе загрузки пакета OSXFUSE возникла ошибка соединения. \
Повторите установку OSXFUSE позже после того как устраните проблему с доступом в интернет. \
Локальное шифрование не будет доступно если пакет OSXFUSE не будет установлен."

de_fuse_dialog="\
osascript -e \
'display alert \"Das OSXFUSE-Paket ist erforderlich, um die lokale Ordnerverschlüsselung zu unterstützen. \
Möchten Sie OSXFUSE jetzt installieren? Im Falle eines Fehlers funktioniert die lokale Ordnerverschlüsselung nicht.\" \
buttons {\"Nein\", \"Ja\"} as warning' \
-e 'if button returned of result = \"Nein\" then' \
-e '    error 1' \
-e 'end if' 2>/dev/null \
"
de_error_msg="\
Beim Laden des OSXFUSE-Pakets ist ein Verbindungsfehler aufgetreten. \
Versuchen Sie später erneut, OSXFUSE zu installieren, nachdem Sie das Problem mit dem Internetzugang behoben haben. \
Die lokale Verschlüsselung ist nicht verfügbar, wenn das OSXFUSE-Paket nicht installiert ist."

en_fuse_dialog="\
osascript -e \
'display alert \"An OSXFUSE package is required to support local folder encryption. \
Would you like to install OSXFUSE now? In case of decline, local folder encryption will not work.\" \
buttons {\"No\", \"Yes\"} as warning' \
-e 'if button returned of result = \"No\" then' \
-e '    error 1' \
-e 'end if' 2>/dev/null \
"
en_error_msg="\
A connection error occurred while loading the OSXFUSE package. \
Retry installing OSXFUSE later after fixing the problem with internet connection. \
Local encryption will not be available if OSXFUSE is not installed."

loc=$(osascript -e 'user locale of (get system info)' | cut -d_ -f1)

error_msg="$loc""_error_msg"
eval "error_msg=\$$error_msg"
if [[ -z $error_msg ]]
then
    error_msg=$en_error_msg
fi

fuse_dialog="$loc""_fuse_dialog"
eval "fuse_dialog=\$$fuse_dialog"
if [[ -z $fuse_dialog ]]
then
    fuse_dialog=$en_fuse_dialog
fi

eval $fuse_dialog
if [[ $? -eq 1 ]]
then
    exit 0
fi

curl -o /tmp/osxfuse.pkg https://updates.securium.ch/csdc/osxfuse.pkg
if [ $? -gt 0 ]
then
    osascript -e "display alert \"$error_msg\" as critical"
    exit 0
fi

chmod 777 /tmp/osxfuse.pkg
open -a Installer /tmp/osxfuse.pkg
exit 0
