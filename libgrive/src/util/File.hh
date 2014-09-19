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

#pragma once

#include "DataStream.hh"
#include "Exception.hh"
#include "FileSystem.hh"
#include "Types.hh"

#include <string>

struct stat ;

namespace gr {

/**	\brief	A wrapper class for file read/write.

	It is a simple wrapper around the UNIX file descriptor. It will
	throw exceptions (i.e. Error) when it encounters errors.
*/
class File : public DataStream
{
public :
	/// File specific errors. It often includes
	/// boost::errinfo_api_function and boost::errinfo_errno for the
	/// detail information.
	struct Error : virtual Exception {} ;

public :
	File() ;
	File( const fs::path& path ) ;
	File( const fs::path& path, int mode ) ;
	~File( ) ;

	File( const File& rhs ) ;
	File& operator=( const File& rhs ) ;
	void Swap( File& other ) ;

	void OpenForRead( const fs::path& path ) ;
	void OpenForWrite( const fs::path& path, int mode = 0600 ) ;
	void Close() ;
	bool IsOpened() const ;

	std::size_t Read( char *ptr, std::size_t size ) ;
	std::size_t Write( const char *ptr, std::size_t size ) ;

	off_t Seek( off_t offset, int whence ) ;
	off_t Tell() const ;
	u64_t Size() const ;

	void Chmod( int mode ) ;

	void* Map( off_t offset, std::size_t length ) ;
	static void UnMap( void *addr, std::size_t length ) ;

	struct stat Stat() const ;

private :
	void Open( const fs::path& path, int flags, int mode ) ;

private :
	int	m_fd ;
} ;

} // end of namespace
