# Grive2 0.5.1-dev

28 Sep 2016, Vitaliy Filippov

http://yourcmc.ru/wiki/Grive2

This is the fork of original "Grive" (https://github.com/Grive/grive) Google Drive client
with the support for the new Drive REST API and partial sync.

Grive can be considered still beta or pre-beta quality. It simply downloads all the files in your
Google Drive into the current directory. After you make some changes to the local files, run
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

These may be added in the future, possibly the next release.

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
systemctl --user enable grive-timer@google-drive.timer
systemctl --user start grive-timer@google-drive.timer
systemctl --user enable grive-changes@google-drive.service
systemctl --user start grive-changes@google-drive.service
```

You can enable and start these two units for multiple folders in your `$HOME`
directory if you need to sync with multiple google accounts.

### Shared files

Files and folders which are shared with you don't automatically show up in
your folder. They need to be added explicitly to your Google Drive: go to the
Google Drive website, right click on the file or folder and chose 'Add to My
Drive'.

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

## Version History

### Grive2 v0.5.1-dev

- support for .griveignore
- automatic sync solution based on inotify-tools and systemd
- no-remote-new and upload-only modes
- ignore regexp does not persist anymore (note that Grive will still track it to not
  accidentally delete remote files when changing ignore regexp)
- added options to limit upload and download speed
- faster upload of new and changed files. now Grive uploads files without first calculating
  md5 checksum when file is created locally or when its size changes.
- added -P/--progress-bar option to print ASCII progress bar for each processed file (pull request by @svartkanin)

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

