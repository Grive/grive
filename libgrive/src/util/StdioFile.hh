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

#include "Exception.hh"
#include "FileSystem.hh"

#include <string>

namespace gr {

class StdioFile
{
public :
	struct Error : virtual Exception {} ;

public :
	StdioFile() ;
	StdioFile( const fs::path& path ) ;
	StdioFile( const fs::path& path, int mode ) ;
	~StdioFile( ) ;

	void OpenForRead( const fs::path& path ) ;
	void OpenForWrite( const fs::path& path, int mode = 0600 ) ;
	void Close() ;
	bool IsOpened() const ;
	
	std::size_t Read( void *ptr, std::size_t size ) ;
	std::size_t Write( const void *ptr, std::size_t size ) ;

	long Seek( long offset, int whence ) ;
	long Tell() const ;
	
	void Chmod( int mode ) ;

private :
	void Open( const fs::path& path, int flags, int mode ) ;
	
private :
	int	m_fd ;
} ;
	
} // end of namespace
