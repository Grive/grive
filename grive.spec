%define rev %(git ls-remote https://github.com/linwiz/grive.git refs/heads/master|awk '{print $1}')

Name:           grive
Version:        0.3.0pre_git%{rev}
Release:        1%{?dist}

Summary:        An open source Linux client for Google Drive

License:        GPLv2
URL:            http://grive.github.com/grive/

BuildRequires: git
BuildRequires: gawk
BuildRequires: cmake
BuildRequires: libstdc++-devel
BuildRequires: libcurl-devel
BuildRequires: json-c-devel
BuildRequires: expat-devel
BuildRequires: openssl-devel
BuildRequires: boost-devel
BuildRequires: binutils-devel
BuildRequires: yajl-devel
BuildRequires: libgcrypt-devel

%description
The purpose of this project is to provide an independent implementation
of Google Drive client. It uses the Google Document List API to talk to
the servers in Google. The code is written in standard C++.

%prep
rm -rf grive-%{version}
git clone https://github.com/linwiz/grive.git -b master grive-%{version}
cd grive-%{version}

%build
cd grive-%{version}
%cmake .
make %{?_smp_mflags}

%install
cd grive-%{version}
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT
rm -rf $RPM_BUILD_ROOT/%{_bindir}/bgrive

%files
%{_docdir}/%{name}/COPYING
%{_docdir}/%{name}/README
%{_bindir}/%{name}
%{_mandir}/man1/%{name}.1.gz
