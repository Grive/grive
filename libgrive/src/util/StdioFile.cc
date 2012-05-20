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

namespace gr {

StdioFile::StdioFile( const std::string& filename, const char *mode ) : m_file( 0 )
{
	Open( filename, mode ) ;
}

StdioFile::StdioFile( const fs::path& path, const char *mode ) : m_file( 0 )
{
	Open( path, mode ) ;
}

StdioFile::~StdioFile( )
{
	Close() ;
}

void StdioFile::Open( const std::string& filename, const char *mode )
{
	if ( IsOpened() )
		Close() ;
	
	assert( m_file == 0 ) ;
	m_file = std::fopen( filename.c_str(), mode ) ;
	
	if ( m_file == 0 )
	{
		BOOST_THROW_EXCEPTION(
			Error()
				<< boost::errinfo_api_function("fopen")
				<< boost::errinfo_errno(errno)
				<< boost::errinfo_file_name(filename)
				<< boost::errinfo_file_open_mode(mode)
		) ;
	}
}

void StdioFile::Open( const fs::path& path, const char *mode )
{
	Open( path.string(), mode ) ;
}

void StdioFile::Close()
{
	if ( IsOpened() )
	{
		std::fclose( m_file ) ;
		m_file = 0 ;
	}
}

bool StdioFile::IsOpened() const
{
	return m_file != 0 ;
}

std::size_t StdioFile::Read( void *ptr, std::size_t size )
{
	assert( m_file != 0 ) ;
	return std::fread( ptr, 1, size, m_file ) ;
}

std::size_t StdioFile::Write( const void *ptr, std::size_t size )
{
	assert( m_file != 0 ) ;
	return std::fwrite( ptr, 1, size, m_file ) ;
}

int StdioFile::Seek( long offset, int whence )
{
	assert( m_file != 0 ) ;
	return std::fseek( m_file, offset, whence ) ;
}

long StdioFile::Tell() const
{
	assert( m_file != 0 ) ;
	return std::ftell( m_file ) ;
}

} // end of namespace
