#Correct it to 1 if you want git snapshot version
%global git 0

%if !%{git}
#Should be corrected to match the Version
    %global gitcommit da89bf2
%else
    %global gitcommit b6fb4a6
    %global gitfull b6fb4a604e51009ece1d8cfe7b45ed603097b47c
    %global date 20120531
%endif

Name:           grive
Version:        0.1.1
%if %{git}
Release:        1.%{date}git%{gitcommit}%{?dist}
%else
Release:        1%{?dist}
%endif

Summary:        An open source Linux client for Google Drive

License:        GPLv2
URL:            http://grive.github.com/grive/
%if %{git}
Source0:        https://github.com/grive/%{name}/tarball/%{gitfull}
%else
Source0:        https://github.com/grive/%{name}/tarball/v%{version}
%endif

BuildRequires:  cmake
BuildRequires:  libstdc++-devel
BuildRequires:  libcurl-devel
BuildRequires:  json-c-devel
BuildRequires:  expat-devel
BuildRequires:  openssl-devel
BuildRequires:  boost-devel
BuildRequires:  binutils-devel

%description
The purpose of this project is to provide an independent implementation
of Google Drive client. It uses the Google Document List API to talk to
the servers in Google. The code is written in standard C++.


%package        devel
Summary:        Development files for grive
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description    devel
Development files for grive

%prep
%setup -q -n Grive-%{name}-%{gitcommit}


%build
%cmake .
make %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%doc COPYING README
%{_bindir}/%{name}
%{_libdir}/libgrive.so.*

%files devel
%{_includedir}/%{name}*
%{_libdir}/libgrive.so



%changelog
* Sat Jun 09 2012 Nestal Wan <me@nestal.net> 0.1.1-1
- Updated for new repository location

* Thu Jun 07 2012 Vasiliy N. Glazov <vascom2@gmail.com> 0.1.0-1
- Jump to release versioning

* Tue May 10 2012 Vasiliy N. Glazov <vascom2@gmail.com> 0.1.0-1.20120528git07553e5.R
- Update to 0.1.0

* Tue May 10 2012 Vasiliy N. Glazov <vascom2@gmail.com> 0.0.4-20120510git0c3fdaa.1.R
- Initial release
