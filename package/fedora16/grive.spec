Name: grive
Version: 0.1.0
Release:	1%{?dist}
Summary: An Open Source Linux Client for Google Drive
Group: System Environment/Base
License:  GPLv2+
URL:http://match065.github.com/grive/
Source0:grive-0.1.0.tar.gz
BuildRoot:%(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXX)

BuildRequires:cppunit
BuildRequires:libstdc++
BuildRequires:libstdc++-devel
BuildRequires:json-c
BuildRequires:json-c-devel
BuildRequires:openssl
BuildRequires:openssl-devel
BuildRequires:expat
BuildRequires:expat-devel
BuildRequires:cppunit
BuildRequires:libcurl
BuildRequires:libcurl-devel
Requires:cppunit
Requires:libstdc++
Requires:json-c
Requires:openssl
Requires:expat
Requires:libcurl

%description
Grive 0.1.0

Grive is still considered experimental. It just downloads all the files in your google drive
into the current directory. After you make some changes to the local files, run grive and
it will upload your changes back to your google drive.

There are a few things that grive does not do at the moment:
- wait for changes in file system to occur and upload. Grive only sync when you run it.
- create new files in google drive. Grive only uploads changed files that already
  exist in server.
  
Of course these will be done in future, possibly the next release.

This is compiled in Fedora 16 x64. You need the following libraries to run:
json-c, libcurl, libstdc++ and openssl. CppUnit is also optional and required
only if you want to build the test cases.

When grive is ran for the first time, you should use the "-a" argument to grant
permission to grive to access to your Google Drive. An URL should be printed.
Go to the link. You will need to login to your google account if you haven't
done so. After granting the permission to grive, the browser will show you
an authenication code. Copy-and-paste that to the standard input of grive.

If everything works fine, grive will create a .grive file in your home directory.
It will also start downloading files from your Google Drive to your current directory.

Enjoy!

%prep
%setup -q %{name}


%build
cmake CMakeLists.txt  
make  DESTDIR=%{buildroot}

%install
rm -rf $RPM_BUILD_ROOT
#make install DESTDIR=%{buildroot}
#%makeinstall  DESTDIR=%{buildroot} 

make DESTDIR=$RPM_BUILD_ROOT install 
mkdir -p  %{buildroot}/usr/lib
mkdir -p  %{buildroot}/usr/local/grive/
mkdir -p  %{buildroot}/usr/share/man/man1/

ln -s /usr/local/lib/libgrive.so %{buildroot}/usr/lib/libgrive.so
ln -s /usr/local/lib/libgrive.so.0 %{buildroot}/usr/lib/libgrive.so.0
ln -s /usr/local/lib/libgrive.so.0.1.0  %{buildroot}/usr/lib/libgrive.so.0.1.0

cp  ~/rpmbuild/BUILD/grive-0.1.0/README %{buildroot}/usr/local/grive/
mv  ~/rpmbuild/BUILD/grive-0.1.0/README %{buildroot}/usr/share/man/man1/grive.1
chmod -x $RPM_BUILD_ROOT%{_mandir}/man1/grive.1


%Clean

rm -rf $RPM_BUILD_ROOT


%files
%defattr(-,root,root,-)
   /usr/local/bin/grive
   /usr/local/include/grive/drive/CommonUri.hh
   /usr/local/include/grive/drive/Drive.hh
   /usr/local/include/grive/drive/Entry.hh
   /usr/local/include/grive/drive/Resource.hh
   /usr/local/include/grive/drive/ResourceTree.hh
   /usr/local/include/grive/drive/State.hh
   /usr/local/include/grive/protocol/Json.hh
   /usr/local/include/grive/protocol/JsonResponse.hh
   /usr/local/include/grive/protocol/OAuth2.hh
   /usr/local/include/grive/util/CArray.hh
   /usr/local/include/grive/util/Crypt.hh
   /usr/local/include/grive/util/DateTime.hh
   /usr/local/include/grive/util/DefaultLog.hh
   /usr/local/include/grive/util/Destroy.hh
   /usr/local/include/grive/util/Exception.hh
   /usr/local/include/grive/util/FileSystem.hh
   /usr/local/include/grive/util/Function.hh
   /usr/local/include/grive/util/Log.hh
   /usr/local/include/grive/util/OS.hh
   /usr/local/include/grive/util/SignalHandler.hh
   /usr/local/include/grive/util/StdioFile.hh
   /usr/local/include/grive/xml/Error.hh
   /usr/local/include/grive/xml/Node.hh
   /usr/local/include/grive/xml/NodeSet.hh
   /usr/local/include/grive/xml/TreeBuilder.hh
   /usr/local/lib/libgrive.so
   /usr/local/lib/libgrive.so.0
   /usr/local/lib/libgrive.so.0.1.0
   /usr/lib/libgrive.so
   /usr/lib/libgrive.so.0
   /usr/lib/libgrive.so.0.1.0
   /usr/local/grive/README
   /usr/share/man/man1/grive.1.gz

%post
/sbin/ldconfig || exit 1
exit 0

%postun
/sbin/ldconfig
[ "$1" -eq 0 ] 
exit 0


%doc README

%changelog
* Sat Jun 02 2012 Nestal Wan me@nestal.net
- Removed gdbm as dependency.
- Updated descriptions.

* Sat May 26 2012 Jesus Bustos jesus.bustos@adminilinux.org
- Changed the name from grive to match-grive.
- Corrected previos changelog.

* Tue May 25 2012 Jesus Bustos jesus.bustos@adminlinux.org 
- Added man page.

