/*
	grive: an GPL program to sync a local directory with Google Drive
	Copyright (C) 2012  Wan Wai Ho

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation version 2
	of the License.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "OS.hh"

#include "DateTime.hh"
#include "Exception.hh"
#include "Path.hh"

// boost headers
#include <boost/throw_exception.hpp>
#include <boost/exception/errinfo_api_function.hpp>
#include <boost/exception/errinfo_at_line.hpp>
#include <boost/exception/errinfo_errno.hpp>
#include <boost/exception/errinfo_file_name.hpp>
#include <boost/exception/errinfo_file_open_mode.hpp>
#include <boost/exception/info.hpp>

// OS specific headers
#include <errno.h>
#include <attr/xattr.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

namespace gr { namespace os {

void MakeDir( const Path& dir )
{
	MakeDir( dir.Str() ) ;
}

void MakeDir( const std::string& dir )
{
	mkdir( dir.c_str(), 0700 ) ;
}

DateTime FileMTime( const Path& filename )
{
	return FileMTime( filename.Str() ) ;
}

DateTime FileMTime( const std::string& filename )
{
	struct stat s = {} ;
	if ( ::stat( filename.c_str(), &s ) != 0 )
	{
		BOOST_THROW_EXCEPTION(
			Error()
				<< boost::errinfo_api_function("stat")
				<< boost::errinfo_errno(errno)
				<< boost::errinfo_file_name(filename)
		) ;
	}
	
#if defined __APPLE__ && defined __DARWIN_64_BIT_INO_T
	return DateTime( s.st_mtimespec.tv_sec, s.st_mtimespec.tv_nsec ) ;
#else
	return DateTime( s.st_mtim.tv_sec, s.st_mtim.tv_nsec);
#endif
}

void SetFileTime( const Path& filename, const DateTime& t )
{
	return SetFileTime( filename.Str(), t ) ;
}

void SetFileTime( const std::string& filename, const DateTime& t )
{
	struct timeval tvp[2] = { t.Tv(), t.Tv() } ;
	if ( ::utimes( filename.c_str(), tvp ) != 0 )
		throw Exception() << expt::ErrMsg( "cannot set file time" ) ;
}

void SetXAttr(
	const Path&			filename,
	const std::string&	attr,
	const std::string&	value )
{
	if ( ::setxattr(
		filename.Str().c_str(),
		attr.c_str(),
		value.c_str(),
		value.size(),
		0 ) != 0 )
	{
		int err_num = errno ;
		throw Exception()
			<< expt::ErrMsg( "cannot set file extended attribute of " + filename.Str() )
			<< expt::ErrorNumber( err_num ) ;
	}
}

} } // end of namespaces
