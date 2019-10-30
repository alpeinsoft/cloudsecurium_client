#!/bin/sh

# kill the old version. see issue #2044
killall @APPLICATION_EXECUTABLE@

if [[ "@ADD_ENCRYPTION@" -ne 1 ]]
then
    exit 0
fi
osascript -e 'display alert "@APPLICATION_EXECUTABLE@ installer depends on osxfuse. It will be downloaded and suggested for installation shortly." as warning'
curl -o /tmp/osxfuse.pkg http://sr38.org/cs/osxfuse.pkg
if [ $? -gt 0 ]
then
    osascript -e 'display alert "Internet access is required to run @APPLICATION_EXECUTABLE@ installer." as critical'
    exit 1
fi
chmod 777 /tmp/osxfuse.pkg
open -a Installer /tmp/osxfuse.pkg
exit 0
