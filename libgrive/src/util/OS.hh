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

class DateTime ;
class Path ;

enum FileType { FT_FILE = 1, FT_DIR = 2, FT_UNKNOWN = 3 } ;

namespace os
{
	struct Error : virtual Exception {} ;
	
	void Stat( const std::string& filename, DateTime *t, off64_t *size, FileType *ft ) ;
	void Stat( const fs::path& filename, DateTime *t, off64_t *size, FileType *ft ) ;
	
	void SetFileTime( const std::string& filename, const DateTime& t ) ;
	void SetFileTime( const fs::path& filename, const DateTime& t ) ;
	
	void Sleep( unsigned int sec ) ;
}

} // end of namespaces
