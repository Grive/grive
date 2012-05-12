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

#include "http/Agent.hh"
#include "util/DateTime.hh"

#include <iosfwd>

namespace gr {

namespace xml
{
	class Node ;
}

class Json ;
class OAuth2 ;
class Path ;

/*!	\brief	corresponds to an "entry" in the resource feed

	This class is decodes an entry in the resource feed. It will stored the properties like
	title, filename and ETag etc in member variables.
*/
class Entry
{
public :
	explicit Entry( const Path& file ) ;
	explicit Entry( const xml::Node& n ) ;
	Entry( const std::string& title, const std::string& href ) ;

	std::string Title() const ;
	std::string Filename() const ;
	std::string Kind() const ;
	std::string ServerMD5() const ;
	DateTime ServerModified() const ;
	
	std::string ResourceID() const ;
	std::string ETag() const ;
	
	std::string SelfHref() const ;
	std::string ParentHref() const ;
	
	const std::vector<std::string>& ParentHrefs() const ;
	
	void Download( gr::http::Agent* http, const Path& file, const http::Headers& auth ) const ;
	bool Upload( gr::http::Agent* http, std::streambuf *file, const http::Headers& auth ) ;
	void Delete( gr::http::Agent* http, const gr::http::Headers& auth ) ;
	
	void Swap( Entry& e ) ;
	
private :
	void Update( const xml::Node& entry ) ;

private :
	std::string		m_title ;
	std::string		m_filename ;
	std::string		m_kind ;
	std::string		m_server_md5 ;
	std::string		m_etag ;
	std::string		m_resource_id ;

	std::vector<std::string>	m_parent_hrefs ;
	
	std::string		m_self_href ;
	std::string		m_content_src ;
	std::string		m_parent_href ;
	std::string		m_upload_link ;

	DateTime		m_server_modified ;
} ;

} // end of namespace
