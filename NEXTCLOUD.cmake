set( APPLICATION_NAME       "CloudSecurium" )
set( APPLICATION_SHORTNAME  "CloudSecurium" )
set( APPLICATION_EXECUTABLE "cloudsecurium" )
set( APPLICATION_DOMAIN     "cloudsecurium.com" )
set( APPLICATION_VENDOR     "Alpein Software SWISS AG" )
set( APPLICATION_UPDATE_URL "https://updates.securium.ch/csdc/updates/" CACHE string "URL for updater" )
set( APPLICATION_HELP_URL   "" CACHE STRING "URL for the help menu" )
set( APPLICATION_ICON_NAME  "CloudSecurium" )
set( APPLICATION_SERVER_URL "" CACHE STRING "URL for the server to use. If entered the server can only connect to this instance" )

set( LINUX_PACKAGE_SHORTNAME "cloudsecurium" )

set( THEME_CLASS            "NextcloudTheme" )
set( APPLICATION_REV_DOMAIN "com.nextcloud.desktopclient" )
set( WIN_SETUP_BITMAP_PATH  "${CMAKE_SOURCE_DIR}/admin/win/nsi" )

set( MAC_INSTALLER_BACKGROUND_FILE "${CMAKE_SOURCE_DIR}/admin/osx/installer-background.png" CACHE STRING "The MacOSX installer background image")

# set( THEME_INCLUDE          "${OEM_THEME_DIR}/mytheme.h" )
# set( APPLICATION_LICENSE    "${OEM_THEME_DIR}/license.txt )

option( WITH_CRASHREPORTER "Build crashreporter" OFF )
#set( CRASHREPORTER_SUBMIT_URL "https://crash-reports.owncloud.com/submit" CACHE STRING "URL for crash reporter" )
#set( CRASHREPORTER_ICON ":/owncloud-icon.png" )

## Updater options
option( BUILD_UPDATER "Build updater" OFF )

option( WITH_PROVIDERS "Build with providers list" ON )


## Theming options
set( APPLICATION_WIZARD_HEADER_BACKGROUND_COLOR "#0082c9" CACHE STRING "Hex color of the wizard header background")
set( APPLICATION_WIZARD_HEADER_TITLE_COLOR "#ffffff" CACHE STRING "Hex color of the text in the wizard header")
option( APPLICATION_WIZARD_USE_CUSTOM_LOGO "Use the logo from ':/client/theme/colored/wizard_logo.png' else the default application icon is used" ON )

