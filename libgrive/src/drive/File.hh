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

#include <string>

#include "protocol/HTTP.hh"
#include "util/DateTime.hh"

#include <iosfwd>

namespace gr {

class Json ;
class OAuth2 ;
class Path ;

class File
{
public :
	explicit File( const Path& file ) ;
	explicit File( const Json& entry ) ;

	std::string Title() const ;
	std::string Filename() const ;
	std::string Kind() const ;
	std::string ServerMD5() const ;
	DateTime ServerModified() const ;
	
	std::string Href() const ;
	std::string Parent() const ;
	
	void Download( const Path& file, const http::Headers& auth ) const ;
	void Upload( std::streambuf *file, const http::Headers& auth ) ;

private :
	void Update( const Json& entry ) ;
	static std::string Parent( const Json& entry ) ;

private :
	std::string		m_title ;
	std::string		m_filename ;
	std::string		m_kind ;
	std::string		m_server_md5 ;
	std::string		m_etag ;

	std::string		m_href ;
	std::string		m_parent ;
	std::string		m_upload_link ;

	DateTime		m_server_modified ;
} ;

} // end of namespace
