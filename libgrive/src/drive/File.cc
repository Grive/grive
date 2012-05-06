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

#include "File.hh"

#include "CommonUri.hh"

#include "http/Download.hh"
#include "http/StringResponse.hh"
#include "http/XmlResponse.hh"
#include "protocol/Json.hh"
#include "protocol/OAuth2.hh"
#include "util/OS.hh"
#include "util/Path.hh"
#include "xml/Node.hh"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>

namespace gr {

File::File( const Path& file )
{
}

File::File( const Json& entry )
{
	Update( entry ) ;
}

void File::Update( const Json& entry )
{
	m_title				= entry["title"]["$t"].Str() ;
	
	m_filename			= entry.Has("docs$suggestedFilename") ?
		entry["docs$suggestedFilename"]["$t"].Str() : "" ;
	
	m_href				= entry["content"]["src"].Str() ;
	m_parent			= Parent( entry ) ;
	m_server_modified	= DateTime( entry["updated"]["$t"].Str() ) ;
	m_etag				= entry["gd$etag"].Str() ;
	
	m_resource_id		= entry["gd$resourceId"]["$t"].Str() ;
	
	m_server_md5		= entry.Has("docs$md5Checksum") ?
		entry["docs$md5Checksum"]["$t"].Str() : "" ;

	Json node ;
	m_kind				= entry["category"].
		FindInArray( "scheme", "http://schemas.google.com/g/2005#kind", node )
		? node["label"].Str() : std::string() ;
	
	m_upload_link		= entry["link"].
		FindInArray( "rel", "http://schemas.google.com/g/2005#resumable-edit-media", node )
		? node["href"].Str() : std::string() ;

  	// convert to lower case for easy comparison
	std::transform(
		m_server_md5.begin(),
		m_server_md5.end(),
		m_server_md5.begin(),
		tolower ) ;
}

std::string File::Parent( const Json& entry )
{
	Json node ;
	return entry["link"].FindInArray( "rel", "http://schemas.google.com/docs/2007#parent", node ) ?
		 node["href"].Str() : std::string() ;
}

std::string File::Title() const
{
	return m_title ;
}

std::string File::Filename() const
{
	return m_filename ;
}

std::string File::Kind() const
{
	return m_kind ;
}

std::string File::ServerMD5() const
{
	return m_server_md5 ;
}

DateTime File::ServerModified() const
{
	return m_server_modified ;
}

std::string File::Href() const
{
	return m_href ;
}

std::string File::Parent() const
{
	return m_parent ;
}

void File::Download( const Path& file, const http::Headers& auth ) const
{
	gr::Download dl( file.Str(), Download::NoChecksum() ) ;
	http::Agent http ;
	long r = http.Get( m_href, &dl, auth ) ;
	if ( r <= 400 )
		os::SetFileTime( file, m_server_modified ) ;
}

bool File::Upload( std::streambuf *file, const http::Headers& auth )
{
	// upload link missing means that file is read only
	if ( m_upload_link.empty() )
		return false ;

	std::string meta =
	"<?xml version='1.0' encoding='UTF-8'?>\n"
	"<entry xmlns=\"http://www.w3.org/2005/Atom\" xmlns:docs=\"http://schemas.google.com/docs/2007\">"
		"<category scheme=\"http://schemas.google.com/g/2005#kind\" "
		"term=\"http://schemas.google.com/docs/2007#file\"/>"
		"<title>" + m_filename + "</title>"
	"</entry>" ;

	std::string data(
		(std::istreambuf_iterator<char>(file)),
		(std::istreambuf_iterator<char>()) ) ;
	
	std::ostringstream xcontent_len ;
	xcontent_len << "X-Upload-Content-Length: " << data.size() ;
	
	http::Headers hdr( auth ) ;
	hdr.push_back( "Content-Type: application/atom+xml" ) ;
	hdr.push_back( "X-Upload-Content-Type: application/octet-stream" ) ;
	hdr.push_back( xcontent_len.str() ) ;
  	hdr.push_back( "If-Match: " + m_etag ) ;
	hdr.push_back( "Expect:" ) ;
	
	http::StringResponse str ;
	http::Agent http ;
	http.Put( m_upload_link, meta, &str, hdr ) ;
	
	std::string uplink = http.RedirLocation() ;
	
	// parse the header and find "Location"
	http::Headers uphdr ;
	uphdr.push_back( "Expect:" ) ;
	uphdr.push_back( "Accept:" ) ;
	
	http::XmlResponse xml ;
	http.Put( uplink, data, &xml, uphdr ) ;

std::cout << xml.Response() << std::endl ;

	return true ;
}

std::string File::ResourceID() const
{
	return m_resource_id ;
}

std::string File::ETag() const
{
	return m_etag ;
}

void File::Delete( http::Agent *http, const http::Headers& auth )
{
	http::Headers hdr( auth ) ;
	hdr.push_back( "If-Match: " + m_etag ) ;
	
std::cout << root_url + "/" + m_resource_id + "?delete=true" << std::endl ;
	
	http::StringResponse str ;
	http->Custom( "DELETE", root_url + "/" + m_resource_id + "?delete=true", &str, hdr ) ;
}

} // end of namespace
