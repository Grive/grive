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

#include "Entry.hh"
#include "http/Agent.hh"
#include "util/FileSystem.hh"

#include <iosfwd>

namespace gr {

namespace http
{
	class Agent ;
}

class Collection ;

class File
{
public :
	explicit File( const Entry& e, const Collection *parent ) ;

	void Update( http::Agent *http, const http::Headers& auth ) ;
	void Delete( http::Agent* http, const http::Headers& auth ) ;
	
	fs::path Path() const ;
	std::string ResourceID() const ;

private :
	void Download( http::Agent* http, const fs::path& file, const http::Headers& auth ) const ;
	bool Upload( http::Agent* http, std::streambuf *file, const http::Headers& auth ) ;

private :
	Entry				m_entry ;
	const Collection	*m_parent ;
} ;

} // end of namespace
