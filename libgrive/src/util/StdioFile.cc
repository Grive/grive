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

#include "StdioFile.hh"

#include <cassert>

// boost headers
#include <boost/throw_exception.hpp>
#include <boost/exception/errinfo_api_function.hpp>
#include <boost/exception/errinfo_at_line.hpp>
#include <boost/exception/errinfo_errno.hpp>
#include <boost/exception/errinfo_file_name.hpp>
#include <boost/exception/errinfo_file_open_mode.hpp>
#include <boost/exception/info.hpp>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

// local functions
namespace {

off_t LSeek( int fd, off_t offset, int whence )
{
	assert( fd >= 0 ) ;
	
	off_t r = ::lseek( fd, offset, whence ) ;
	if ( r == static_cast<off_t>(-1) )
	{
		BOOST_THROW_EXCEPTION(
			gr::StdioFile::Error()
				<< boost::errinfo_api_function("lseek")
				<< boost::errinfo_errno(errno)
		) ;
	}
	
	return r ;
}

struct stat FStat( int fd )
{
	struct stat s = {} ;
	if ( ::fstat( fd, &s ) != 0 )
	{
		BOOST_THROW_EXCEPTION(
			gr::StdioFile::Error()
				<< boost::errinfo_api_function("fstat")
				<< boost::errinfo_errno(errno)
		) ;
	}
	return s ;
}

} // end of local functions

namespace gr {

StdioFile::StdioFile( ) : m_fd( -1 )
{
}

StdioFile::StdioFile( const fs::path& path ) : m_fd( -1 )
{
	OpenForRead( path ) ;
}

StdioFile::StdioFile( const fs::path& path, int mode ) : m_fd( -1 )
{
	OpenForWrite( path, mode ) ;
}

StdioFile::~StdioFile( )
{
	Close() ;
}

void StdioFile::Open( const fs::path& path, int flags, int mode )
{
	if ( IsOpened() )
		Close() ;
	
	assert( m_fd == -1 ) ;
	m_fd = ::open( path.string().c_str(), flags, mode ) ;
	if ( m_fd == -1 )
	{
		BOOST_THROW_EXCEPTION(
			Error()
				<< boost::errinfo_api_function("open")
				<< boost::errinfo_errno(errno)
				<< boost::errinfo_file_name(path.string())
		) ;
	}
}

void StdioFile::OpenForRead( const fs::path& path )
{
	Open( path, O_RDONLY, 0 ) ;
}

void StdioFile::OpenForWrite( const fs::path& path, int mode )
{
	Open( path, O_CREAT|O_RDWR|O_TRUNC, mode ) ;
}

void StdioFile::Close()
{
	if ( IsOpened() )
	{
		close( m_fd ) ;
		m_fd = -1 ;
	}
}

bool StdioFile::IsOpened() const
{
	return m_fd != -1 ;
}

std::size_t StdioFile::Read( void *ptr, std::size_t size )
{
	assert( IsOpened() ) ;
	ssize_t count = ::read( m_fd, ptr, size ) ;
	if ( count == -1 )
	{
		BOOST_THROW_EXCEPTION(
			Error()
				<< boost::errinfo_api_function("read")
				<< boost::errinfo_errno(errno)
		) ;
	}
	return count ;
}

std::size_t StdioFile::Write( const void *ptr, std::size_t size )
{
	assert( IsOpened() ) ;
	ssize_t count = ::write( m_fd, ptr, size ) ;
	if ( count == -1 )
	{
		BOOST_THROW_EXCEPTION(
			Error()
				<< boost::errinfo_api_function("read")
				<< boost::errinfo_errno(errno)
		) ;
	}
	return count ;
}

off_t StdioFile::Seek( off_t offset, int whence )
{
	assert( IsOpened() ) ;
	return LSeek( m_fd, offset, whence ) ;
}

off_t StdioFile::Tell() const
{
	assert( IsOpened() ) ;
	return LSeek( m_fd, 0, SEEK_CUR ) ;
}

u64_t StdioFile::Size() const
{
	assert( IsOpened() ) ;
	
	struct stat s = FStat(m_fd) ;
	
	assert( s.st_size >= 0 ) ;
	return static_cast<uint64_t>( s.st_size ) ;
}

void StdioFile::Chmod( int mode )
{
	assert( IsOpened() ) ;
	
	if ( ::fchmod( m_fd, mode ) != 0 )
	{
		BOOST_THROW_EXCEPTION(
			Error()
				<< boost::errinfo_api_function("fchmod")
				<< boost::errinfo_errno(errno)
		) ;
	}
}

void* StdioFile::Map( off_t offset, std::size_t length )
{
	assert( IsOpened() ) ;
	
	void *addr = ::mmap( 0, length, PROT_READ, MAP_PRIVATE, m_fd, offset ) ;
	if ( addr == reinterpret_cast<void*>( -1 ) )
	{
		BOOST_THROW_EXCEPTION(
			Error()
				<< boost::errinfo_api_function("mmap")
				<< boost::errinfo_errno(errno)
		) ;
	}
	return addr ;
}

void StdioFile::UnMap( void *addr, std::size_t length )
{
	if ( ::munmap( addr, length ) != 0 )
	{
		BOOST_THROW_EXCEPTION(
			Error()
				<< boost::errinfo_api_function("munmap")
				<< boost::errinfo_errno(errno)
		) ;
	}
}

} // end of namespace
