# Grive2 0.5.2-dev

13 Nov 2019, Vitaliy Filippov

http://yourcmc.ru/wiki/Grive2

This is the fork of original "Grive" (https://github.com/Grive/grive) Google Drive client
with the support for the new Drive REST API and partial sync.

Grive simply downloads all the files in your Google Drive into the current directory.
After you make some changes to the local files, run
grive again and it will upload your changes back to your Google Drive. New files created locally
or in Google Drive will be uploaded or downloaded respectively. Deleted files will also be "removed".
Currently Grive will NOT destroy any of your files: it will only move the files to a
directory named .trash or put them in the Google Drive trash. You can always recover them.

There are a few things that Grive does not do at the moment:
- continously wait for changes in file system or in Google Drive to occur and upload.
  A sync is only performed when you run Grive (there are workarounds for almost
  continuous sync. See below).
- symbolic links support.
- support for Google documents.

These may be added in the future.

Enjoy!

## Usage

When Grive is run for the first time, you should use the "-a" argument to grant
permission to Grive to access to your Google Drive:

```bash
cd $HOME
mkdir google-drive
cd google-drive
grive -a
```

A URL should be printed. Go to the link. You will need to login to your Google
account if you haven't done so. After granting the permission to Grive, the
browser will show you an authenication code. Copy-and-paste that to the
standard input of Grive.

If everything works fine, Grive will create .grive and .grive_state files in your
current directory. It will also start downloading files from your Google Drive to
your current directory.

To resync the direcory, run `grive` in the folder.

```bash
cd $HOME/google-drive
grive
```

### Exclude specific files and folders from sync: .griveignore

Rules are similar to Git's .gitignore, but may differ slightly due to the different
implementation.

- lines that start with # are comments
- leading and trailing spaces ignored unless escaped with \
- non-empty lines without ! in front are treated as "exclude" patterns
- non-empty lines with ! in front are treated as "include" patterns
  and have a priority over all "exclude" ones
- patterns are matched against the filenames relative to the grive root
- a/**/b matches any number of subpaths between a and b, including 0
- **/a matches `a` inside any directory
- b/** matches everything inside `b`, but not b itself
- \* matches any number of any characters except /
- ? matches any character except /
- .griveignore itself isn't ignored by default, but you can include it in itself to ignore


### Scheduled syncs and syncs on file change events

There are tools which you can use to enable both scheduled syncs and syncs
when a file changes. Together these gives you an experience almost like the
Google Drive clients on other platforms (it misses the almost instantious
download of changed files in the google drive).

Grive installs such a basic solution which uses inotify-tools together with
systemd timer and services. You can enable it for a folder in your `$HOME`
directory (in this case the `$HOME/google-drive`):

First install the `inotify-tools` (seems to be named like that in all major distros): 
test that it works by calling `inotifywait -h`.

Prepare a Google Drive folder in your $HOME directory with `grive -a`.

```bash
# 'google-drive' is the name of your Google Drive folder in your $HOME directory
systemctl --user enable grive-timer@$(systemd-escape google-drive).timer
systemctl --user start grive-timer@$(systemd-escape google-drive).timer
systemctl --user enable grive-changes@$(systemd-escape google-drive).service
systemctl --user start grive-changes@$(systemd-escape google-drive).service
```

You can enable and start these two units for multiple folders in your `$HOME`
directory if you need to sync with multiple google accounts.

### Shared files

Files and folders which are shared with you don't automatically show up in
your folder. They need to be added explicitly to your Google Drive: go to the
Google Drive website, right click on the file or folder and chose 'Add to My
Drive'.

### Different OAuth2 client to workaround over quota and google approval issues

Google recently started to restrict access for unapproved applications:
https://developers.google.com/drive/api/v3/about-auth?hl=ru

Grive2 is currently awaiting approval but it seems it will take forever.
Also even if they approve it the default Client ID supplied with grive may
exceed quota and grive will then fail to sync.

You can supply your own OAuth2 client credentials to work around these problems
by following these steps:

1. Go to https://console.developers.google.com/apis/api/drive.googleapis.com
2. Choose a project (you might need to create one first)
3. Go to https://console.developers.google.com/apis/library/drive.googleapis.com and
   "Enable" the Google Drive APIs
4. Go to https://console.cloud.google.com/apis/credentials and click "Create credentials > Help me choose"
5. In the "Find out what credentials you need" dialog, choose:
   - Which API are you using: "Google Drive API"
   - Where will you be calling the API from: "Other UI (...CLI...)"
   - What data will you be accessing: "User Data"
6. In the next steps create a client id (name doesn't matter) and
   setup the consent screen (defaults are ok, no need for any URLs)
7. The needed "Client ID" and "Client Secret" are either in the shown download
   or can later found by clicking on the created credential on
   https://console.developers.google.com/apis/credentials/
8. When you change client ID/secret in an existing Grive folder you must first delete
   the old `.grive` configuration file.
9. Call `grive -a --id <client_id> --secret <client_secret>` and follow the steps
   to authenticate the OAuth2 client to allow it to access your drive folder.

## Installation

For the detailed instructions, see http://yourcmc.ru/wiki/Grive2#Installation

### Install dependencies

You need the following libraries:

- yajl 2.x
- libcurl
- libstdc++
- libgcrypt
- Boost (Boost filesystem, program_options, regex, unit_test_framework and system are required)
- expat

There are also some optional dependencies:
- CppUnit (for unit tests)
- libbfd (for backtrace)
- binutils (for libiberty, required for compilation in OpenSUSE, Ubuntu, Arch and etc)

On a Debian/Ubuntu/Linux Mint machine just run the following command to install all
these packages:

    sudo apt-get install git cmake build-essential libgcrypt11-dev libyajl-dev \
        libboost-all-dev libcurl4-openssl-dev libexpat1-dev libcppunit-dev binutils-dev \
        debhelper zlib1g-dev dpkg-dev pkg-config

Fedora:

    sudo dnf install git cmake libgcrypt-devel gcc-c++ libstdc++ yajl-devel boost libcurl-devel expat-devel binutils zlib


FreeBSD:

    pkg install git cmake boost-libs yajl libgcrypt pkgconf cppunit libbfd

### Build Debian packages

On a Debian/Ubuntu/Linux Mint you can use `dpkg-buildpackage` utility from `dpkg-dev` package
to build grive. Just clone the repository, `cd` into it and run

    dpkg-buildpackage -j4

### Manual build

Grive uses cmake to build. Basic install sequence is

    mkdir build
    cd build
    cmake ..
    make -j4
    sudo make install

Alternativly you can define your own client_id and client_secret during build

    mkdir build
    cd build
    cmake .. "-DAPP_ID:STRING=<client_id>" "-DAPP_SECRET:STRING=<client_secret>"
    make -j4
    sudo make install

## Version History

### Grive2 v0.5.2-dev

### Grive2 v0.5.1

- Support for .griveignore
- Automatic sync solution based on inotify-tools and systemd
- no-remote-new and upload-only modes
- Ignore regexp does not persist anymore (note that Grive will still track it to not
  accidentally delete remote files when changing ignore regexp)
- Added options to limit upload and download speed
- Faster upload of new and changed files. Now Grive uploads files without first calculating
  md5 checksum when file is created locally or when its size changes.
- Added -P/--progress-bar option to print ASCII progress bar for each processed file (pull request by @svartkanin)
- Added command-line options to specify your own client_id and client_secret
- Now grive2 skips links, sockets, fifos and other unusual files
- Various small build fixes

### Grive2 v0.5

- Much faster and more correct synchronisation using local modification time and checksum cache (similar to git index)
- Automatic move/rename detection, -m option removed
- force option works again
- Instead of crashing on sync exceptions Grive will give a warning and attempt to sync failed files again during the next run.
- Revision support works again. Grive 0.4.x always created new revisions for all files during sync, regardless of the absence of the --new-rev option.
- Shared files now sync correctly

### Grive2 v0.4.2

- Option to exclude files by perl regexp
- Reimplemented HTTP response logging for debug purposes
- Use multipart uploads (update metadata and contents at the same time) for improved perfomance & stability
- Bug fixes
- Simple option to move/rename files and directories, via `grive -m oldpath newpath` (by Dylan Wulf, wulfd1@tcnj.edu)

Known issues:
- force option does not work as documented #51

### Grive2 v0.4.1

- Bug fixes

### Grive2 v0.4.0

First fork release, by Vitaliy Filippov / vitalif at mail*ru
- Support for the new Google Drive REST API (old "Document List" API is shut down by Google 20 April 2015)
- REAL support for partial sync: syncs only one subdirectory with `grive -s subdir`
- Major refactoring - a lot of dead code removed, JSON-C is not used anymore, API-specific code is split from non-API-specific
- Some stability fixes from Visa Putkinen https://github.com/visap/grive/commits/visa
- Slightly reduce number of syscalls when reading local files.

### Grive v0.3

Bug fix & minor feature release. Fixed bugs:
- #93: missing reference count increment in one of the Json constructors
- #82: retry for HTTP error 500 & 503
- #77: Fixed a bug where grive crashed on the first run.

New features:
- #87: support for revisions
- #86: ~~partial sync (contributed by justin at tierramedia.com)~~ that's not partial sync,
  that's only support for specifying local path on command line

