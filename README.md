# Nextcloud Desktop Client

The :computer: Nextcloud Desktop Client is a tool to synchronize files from Nextcloud Server
with your computer.

<p align="center">
    <img src="https://nextcloud.com/wp-content/themes/next/assets/img/clients/desktop/macsettings.png?x16328" alt="Desktop Client on Mac OS]">
</p>

## :blue_heart: :tada: Contributing

### :hammer_and_wrench: How to compile the desktop client

:building_construction: [System requirements](https://github.com/nextcloud/desktop/wiki/System-requirements-for-compiling-the-desktop-client) includes OpenSSL 1.1.x, QtKeychain, Qt 5.x.x and zlib.

#### :memo: Step by step instructions

##### Clone the repo and create build directory
```
$ git clone https://github.com/nextcloud/desktop.git
$ cd desktop
$ mkdir build
$ cd build
```
##### Compile and install

:warning: For development reasons it is better to **install the client on user space** instead on the global system. Mixing up libs/dll's of different version can lead to undefined behavior and crashes:

* You could use the **cmake flag** ```CMAKE_INSTALL_PREFIX``` as ```~/.local/``` in a **Linux** system. If you want to install system wide you could use ```/usr/local``` or ```/opt/nextcloud/```.

* On **Windows 10** [```$USERPROFILE```](https://docs.microsoft.com/en-us/windows/deployment/usmt/usmt-recognized-environment-variables#a-href-idbkmk-2avariables-that-are-recognized-only-in-the-user-context) refers to ```C:\Users\<USERNAME>```.

##### Linux & Mac OS

```
$ cmake .. -DCMAKE_INSTALL_PREFIX=~/nextcloud-desktop-client -DCMAKE_BUILD_TYPE=Debug -DNO_SHIBBOLETH=1
$ make install
```

##### Windows

```
$ cmake -G "Visual Studio 15 2017 Win64" .. -DCMAKE_INSTALL_PREFIX=$USERPROFILE\nextcloud-desktop-client -DCMAKE_BUILD_TYPE=Debug -DNO_SHIBBOLETH=1
$ cmake --build . --config Debug --target install
```

:information_source: More detailed instructions can be found at the [Desktop Client Wiki](https://github.com/nextcloud/desktop/wiki).

### :inbox_tray: Where to find binaries to download

#### :high_brightness: Daily builds

- Daily builds based on the latest master are available for Linux :penguin:, Mac, and Windows
[in the desktop/daily folder of our download server](https://download.nextcloud.com/desktop/daily).
For more info: [Wiki/Daily Builds](https://github.com/nextcloud/desktop/wiki/Daily-Builds).

#### :rocket: Releases

- Refer to the [download page https://nextcloud.com/install/#install-clients](https://nextcloud.com/install/#install-clients)

### :bomb: Reporting issues

- If you find any bugs or have any suggestion for improvement, please
file an issue at https://github.com/nextcloud/client/issues. Do not
contact the authors directly by mail, as this increases the chance
of your report being lost. :boom:

### :smiley: :trophy: Pull requests

- If you created a patch :heart_eyes:, please submit a [Pull
Request](https://github.com/nextcloud/client/pulls).
- How to create a pull request? This guide will help you get started: [Opening a pull request](https://opensource.guide/how-to-contribute/#opening-a-pull-request) :heart:


## :satellite: Contact us

If you want to contact us, e.g. before starting a more complex feature, for questions :question:
you can join us at
[#nextcloud-client](https://webchat.freenode.net/?channels=nextcloud-client).

## :v: Code of conduct

The Nextcloud community has core values that are shared between all members during conferences, hackweeks and on all interactions in online platforms including [Github](https://github.com/nextcloud) and [Forums](https://help.nextcloud.com). If you contribute, participate or interact with this community, please respect [our shared values](https://nextcloud.com/code-of-conduct/). :relieved:

## :memo: Source code

The Nextcloud Desktop Client is developed in Git. Since Git makes it easy to
fork and improve the source code and to adapt it to your need, many copies
can be found on the Internet, in particular on GitHub. However, the
authoritative repository maintained by the developers is located at
https://github.com/nextcloud/desktop.

## :scroll: License

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
    for more details.

# Debugging
Cloudserurium provides additional debugging options that are highly
customizable and concise to use.

## ERROR function
ERROR function helps to report occured errors. It's syntax is similar to
printf function, but it also provides information about file, line and
function where the ERROR is called.

```C++
// example.cpp
#include "common/utility.h"

void example()
{
    ERROR("example #%d\n", 1);
}
// output would be:
// example.cpp +6, example() Error: example #1
```

By default, ERROR's output is directed to the **stderr**, but you may redirect
it to **file** or to application's **log browser**.

In order to redirect it to file (currently it is **/tmp/cs_log_error**), set
cmake option **-D CS_REDIRECT_ERRORS_TO_FILE=ON**.

In order to redirect ERROR's output to the **log browser**, set cmake option
**-D CS_REDIRECT_ERRORS_TO_LOGGER=ON**. It also requires extra code inside
modules where you wish to use ERROR: you have to define **CURRENT_LC** for
ERROR to use.

```C++
// example.cpp, CS_REDIRECT_ERRORS_TO_LOGGER is set
#include "common/utility.h"

Q_LOGGING_CATEGORY(lcTest, "test.test.test", QtInfoMsg)
#define CURRENT_LC lcTest

void example()
{
    ERROR("example #%d\n", 2);
}
// output inside log browser will be:
// example.cpp +9, example() Error: example #2
// plus additional info that is provided by logger handler
```

## LOG function
LOG function is designed to help with debugging. It's syntax is similar to
printf fuction, but it also provides information about file, line and
function where the LOG is called.

It has the following features:
1. Enable debug output only for the modules you're interested in.
2. Enable debug output from all the modules.
3. Suppress all the debug output.
4. Output redirection to stdout, log browser and file.

By default, output of the LOG function is supressed, output is directed to the
stdout and debug output from all the modules is disabled.

In order to turn LOG's output on, cmake build option **-D CS_ENABLE_DEBUG=ON**
has to be set. You also have to define **ENABLE_MODULE_DEBUG** inside the file
you wish to use LOG function.

```C++
// example.cpp, CS_ENABLE_DEBUG is set
#include "common/utility.h"
#define ENABLE_MODULE_DEBUG true

void example()
{
    LOG("example #%d\n", 3);
}
// output would be:
// example.cpp +7, example(): example #3
```

If you set cmake build option **-D CS_ALL_MODULES_DEBUG=ON**, debug output
from all the modules will be enabled reguardless of **ENABLE_MODULE_DEBUG**
definitions.

If you would like to redirect LOG's output to **file** (currently it is
**/tmp/cs_log_debug**), you have to set **-D CS_REDIRECT_DEBUG_TO_FILE=ON**.

If you would like to redirect LOG's output to the **log browser**, you have
to set **-D CS_REDIRECT_DEBUG_TO_LOGGER=ON**. It also requires extra code
inside modules where you wish to use LOG: you have to define **CURRENT_LC**
for LOG to use. (Don't forget to define **ENABLE_MODULE_DEBUG** unless you
use **CS_REDIRECT_DEBUG_TO_LOGGER** option).

```C++
// example.cpp, CS_ENABLE_DEBUG, CS_REDIRECT_DEBUG_TO_LOGGER are set
#include "common/utility.h"

Q_LOGGING_CATEGORY(lcTest, "test.test.test", QtInfoMsg)
#define CURRENT_LC lcTest
#define ENABLE_MODULE_DEBUG true

void example()
{
    LOG("example #%d\n", 4);
}
// output inside log browser will be:
// example.cpp +10, example(): example #4
// plus additional info that is provided by logger handler
```