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

#include "Entry.hh"

#include "CommonUri.hh"

#include "http/Download.hh"
#include "http/StringResponse.hh"
#include "http/XmlResponse.hh"
#include "protocol/Json.hh"
#include "protocol/OAuth2.hh"
#include "util/OS.hh"
#include "util/Path.hh"
#include "xml/Node.hh"
#include "xml/NodeSet.hh"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>

namespace gr {

Entry::Entry( const Path& file )
{
}

Entry::Entry( const Json& entry )
{
	Update( entry ) ;
}

Entry::Entry( const xml::Node& n )
{
	Update( n ) ;
}

Entry::Entry( const std::string& title, const std::string& href ) :
	m_title( title ),
	m_self_href( href )
{
}

void Entry::Update( const Json& entry )
{
	m_title				= entry["title"]["$t"].Str() ;
	
	m_filename			= entry.Has("docs$suggestedFilename") ?
		entry["docs$suggestedFilename"]["$t"].Str() : "" ;
	
	m_content_src		= entry["content"]["src"].Str() ;
	m_self_href			= entry["link"].FindInArray( "rel", "self" )["href"].Str() ;
	m_parent_href		= Parent( entry ) ;
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

void Entry::Update( const xml::Node& n )
{
	m_title				= n["title"] ;
	m_etag				= n["@gd:etag"] ;
	m_filename			= n["docs:suggestedFilename"] ;
	m_content_src		= n["content"]["@src"] ;
	m_self_href			= n["link"].Find( "@rel", "self" )["@href"] ;
	m_server_modified	= DateTime( n["updated"] ) ;	

	m_resource_id		= n["gd:resourceId"] ;
	m_server_md5		= n["docs:md5Checksum"] ;
	m_kind				= n["category"].Find( "@scheme", "http://schemas.google.com/g/2005#kind" )["@label"] ;
	m_upload_link		= n["link"].Find( "!rel", "http://schemas.google.com/g/2005#resumable-edit-media")["href"] ;

	m_parent_hrefs.clear( ) ;
	xml::NodeSet parents = n["link"].Find( "@rel", "http://schemas.google.com/docs/2007#parent" ) ;
	for ( xml::NodeSet::iterator i = parents.begin() ; i != parents.end() ; ++i )
		m_parent_hrefs.push_back( (*i)["@href"] ) ;
}

std::string Entry::Parent( const Json& entry )
{
	Json node ;
	return entry["link"].FindInArray( "rel", "http://schemas.google.com/docs/2007#parent", node ) ?
		 node["href"].Str() : std::string() ;
}

const std::vector<std::string>& Entry::ParentHrefs() const
{
	return m_parent_hrefs ;
}

std::string Entry::Title() const
{
	return m_title ;
}

std::string Entry::Filename() const
{
	return m_filename ;
}

std::string Entry::Kind() const
{
	return m_kind ;
}

std::string Entry::ServerMD5() const
{
	return m_server_md5 ;
}

DateTime Entry::ServerModified() const
{
	return m_server_modified ;
}

std::string Entry::SelfHref() const
{
	return m_self_href ;
}

std::string Entry::ParentHref() const
{
	return m_parent_href ;
}

void Entry::Download( gr::http::Agent* http, const Path& file, const http::Headers& auth ) const
{
	gr::Download dl( file.Str(), Download::NoChecksum() ) ;
	long r = http->Get( m_content_src, &dl, auth ) ;
	if ( r <= 400 )
		os::SetFileTime( file, m_server_modified ) ;
}

bool Entry::Upload( gr::http::Agent* http, std::streambuf *file, const http::Headers& auth )
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
	http->Put( m_upload_link, meta, &str, hdr ) ;
	
	std::string uplink = http->RedirLocation() ;
	
	// parse the header and find "Location"
	http::Headers uphdr ;
	uphdr.push_back( "Expect:" ) ;
	uphdr.push_back( "Accept:" ) ;
	
	http::XmlResponse xml ;
	http->Put( uplink, data, &xml, uphdr ) ;

std::cout << xml.Response() << std::endl ;

	return true ;
}

std::string Entry::ResourceID() const
{
	return m_resource_id ;
}

std::string Entry::ETag() const
{
	return m_etag ;
}

void Entry::Delete( http::Agent *http, const http::Headers& auth )
{
	http::Headers hdr( auth ) ;
	hdr.push_back( "If-Match: " + m_etag ) ;
	
std::cout << root_url + "/" + m_resource_id + "?delete=true" << std::endl ;
	
	http::StringResponse str ;
	http->Custom( "DELETE", root_url + "/" + m_resource_id + "?delete=true", &str, hdr ) ;
}

void Entry::Swap( Entry& e )
{
	m_title.swap( e.m_title ) ;
	m_filename.swap( e.m_filename ) ;
	m_kind.swap( e.m_kind ) ;
	m_server_md5.swap( e.m_server_md5 ) ;
	m_etag.swap( e.m_etag ) ;
	m_resource_id.swap( e.m_resource_id ) ;

	m_self_href.swap( e.m_self_href ) ;
	m_parent_href.swap( e.m_parent_href ) ;
	m_content_src.swap( e.m_content_src ) ;	
	m_upload_link.swap( e.m_upload_link ) ;

	m_server_modified.Swap( e.m_server_modified ) ;
}

} // end of namespace
