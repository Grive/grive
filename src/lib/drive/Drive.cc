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

#include "Drive.hh"

#include "protocol/HTTP.hh"
#include "protocol/Json.hh"
#include "protocol/OAuth2.hh"
#include "util/Crypt.hh"
#include "util/DateTime.hh"
#include "util/OS.hh"
#include "util/Path.hh"

// standard C++ library
#include <algorithm>
#include <cassert>
#include <fstream>
#include <map>
#include <sstream>

// for debugging only
#include <iostream>

namespace gr {

const std::string root_url = "https://docs.google.com/feeds/default/private/full" ;

Drive::Drive( OAuth2& auth ) :
	m_auth( auth ),
	m_root( ".", "https://docs.google.com/feeds/default/private/full/folder%3Aroot" )
{
	m_http_hdr.push_back( "Authorization: Bearer " + m_auth.AccessToken() ) ;
	m_http_hdr.push_back( "GData-Version: 3.0" ) ;
	
	Json resp = Json::Parse( http::Get( root_url + "?alt=json&showfolders=true", m_http_hdr )) ;
	
	Json resume_link ;
	if ( resp["feed"]["link"].FindInArray( "rel", "http://schemas.google.com/g/2005#resumable-create-media", resume_link ) )
		m_resume_link = resume_link["href"].As<std::string>() ;
	
	Json::Array entries = resp["feed"]["entry"].As<Json::Array>() ;
	ConstructDirTree( entries ) ;
	
	for ( Json::Array::iterator i = entries.begin() ; i != entries.end() ; ++i )
	{
		if ( !Collection::IsCollection( *i ) )
		{
			UpdateFile( *i ) ;
		}
	}
}

Drive::~Drive( )
{
}

std::string Drive::Kind( const Json& entry )
{
	Json node ;
	return entry["category"].FindInArray( "scheme", "http://schemas.google.com/g/2005#kind", node ) ?
		node["label"].As<std::string>() : std::string() ;
}

struct SortCollectionByHref
{
	bool operator()( const Collection& c1, const Collection& c2 ) const
	{
		return c1.Href() < c2.Href() ;
	}
} ;

Drive::FolderListIterator Drive::FindFolder( const std::string& href )
{
	// try to find the parent by its href
	std::vector<Collection>::iterator it =
		std::lower_bound(
			m_coll.begin(),
			m_coll.end(),
			Collection( "", href ),
			SortCollectionByHref() ) ;
	
	 return (it != m_coll.end() && it->Href() == href) ? it : m_coll.end() ;
}

void Drive::ConstructDirTree( const std::vector<Json>& entries )
{
	assert( m_coll.empty() ) ;
	std::map<std::string, std::string> parent_href ;

	// first, get all collections from the query result
	for ( Json::Array::const_iterator i = entries.begin() ; i != entries.end() ; ++i )
	{
		if ( Collection::IsCollection( *i ) )
		{
			m_coll.push_back( Collection( *i ) ) ;
			parent_href.insert(
				std::make_pair(
					m_coll.back().Href(),
					Collection::ParentHref( *i ) ) ) ;
		}
	}
	assert( m_coll.size() == parent_href.size() ) ;
	
	// second, build up linkage between parent and child 
	std::sort( m_coll.begin(), m_coll.end(), SortCollectionByHref() ) ;
	for ( FolderListIterator i = m_coll.begin() ; i != m_coll.end() ; ++i )
	{
		assert( parent_href.find( i->Href() ) != parent_href.end() ) ;
		std::string parent = parent_href[i->Href()] ;
		
		if ( parent.empty() )
			m_root.AddChild( &*i ) ;
		else
		{
			FolderListIterator pit = FindFolder( parent ) ;
			if ( pit != m_coll.end() )
			{
				// it shouldn't happen, just in case
				if ( &*i == &*pit )
					std::cout
						<< "the parent of folder " << i->Title()
						<< " is itself. ignored" << std::endl ;
				else
					pit->AddChild( &*i ) ;
			}
		}
	}
	
	// lastly, iterating from the root, create the directories in the local file system
	assert( m_root.Parent() == 0 ) ;
	m_root.CreateSubDir( Path() ) ;
}

std::string Drive::Parent( const Json& entry )
{
	Json node ;
	return entry["link"].FindInArray( "rel", "http://schemas.google.com/docs/2007#parent", node ) ?
		 node["href"].As<std::string>() : std::string() ;
}

void Drive::UpdateFile( const Json& entry )
{
	// only handle uploaded files
	if ( entry.Has( "docs$filename" ) )
	{
		// use title as the filename
		std::string filename	= entry["docs$filename"]["$t"].Get() ;
		std::string url			= entry["content"]["src"].Get() ;
		std::string parent_href	= Parent( entry ) ;
/*
		Json kind_json ;
		if ( entry["category"].FindInArray( "scheme", "http://schemas.google.com/g/2005#kind", kind_json ) )
		{
			std::cout << filename << " kind = " << kind_json << std::endl ;
		}
*/
		bool changed = true ;
		Path path = Path() / filename ;

		// determine which folder the file belongs to
		if ( !parent_href.empty() )
		{
			FolderListIterator pit = FindFolder( parent_href ) ;
			if ( pit != m_coll.end() )
				path = pit->Dir() / filename ;
		}

		// compare checksum first if file exists
		std::ifstream ifile( path.Str().c_str(), std::ios::binary | std::ios::out ) ;
		if ( ifile && entry.Has("docs$md5Checksum") )
		{
			std::string remote_md5	= entry["docs$md5Checksum"]["$t"].As<std::string>() ;
			std::string local_md5	= crypt::MD5( ifile.rdbuf() ) ;
			
			std::transform( remote_md5.begin(), remote_md5.end(), remote_md5.begin(), tolower ) ;
			if ( local_md5 == remote_md5 )
				changed = false ;
		}
		
		// if the checksum is different, file is changed and we need to update
		if ( changed )
		{
			DateTime remote( entry["updated"]["$t"].As<std::string>() ) ;
			DateTime local = ifile ? os::FileMTime( path.Str() ) : DateTime() ;
			
			// remote file is newer, download file
			if ( !ifile || remote > local )
			{
std::cout << "downloading " << path << std::endl ;
				http::GetFile( url, path.Str(), m_http_hdr ) ;
				os::SetFileTime( path.Str(), remote ) ;
			}
			else
			{
std::cout << "local " << filename << " is newer" << std::endl ;
				// re-reading the file
				ifile.seekg(0) ;
				
				UploadFile( entry, filename, ifile.rdbuf() ) ;
			}
		}
	}
}

void Drive::UploadFile( const Json& entry, const std::string& filename, std::streambuf *file )
{
	std::string meta =
	"<?xml version='1.0' encoding='UTF-8'?>\n"
	"<entry xmlns=\"http://www.w3.org/2005/Atom\" xmlns:docs=\"http://schemas.google.com/docs/2007\">"
		"<category scheme=\"http://schemas.google.com/g/2005#kind\" "
		"term=\"http://schemas.google.com/docs/2007#file\"/>"
		"<title>" + filename + "</title>"
	"</entry>" ;

	std::string data(
		(std::istreambuf_iterator<char>(file)),
		(std::istreambuf_iterator<char>()) ) ;
	
	std::ostringstream xcontent_len ;
	xcontent_len << "X-Upload-Content-Length: " << data.size() ;
	
	http::Headers hdr( m_http_hdr ) ;
//	hdr.push_back( "Slug: Grive Document" ) ;
	hdr.push_back( "Content-Type: application/atom+xml" ) ;
	hdr.push_back( "X-Upload-Content-Type: application/octet-stream" ) ;
	hdr.push_back( xcontent_len.str() ) ;
  	hdr.push_back( "If-Match: " + entry["gd$etag"].As<std::string>() ) ;
	hdr.push_back( "Expect:" ) ;
	
/*	std::string resp = http::PostDataWithHeader(
		m_resume_link + "?convert=false",
		meta,
		hdr ) ;
*/
	Json resume_link = entry["link"].FindInArray( "rel",
  		"http://schemas.google.com/g/2005#resumable-edit-media" )["href"] ;
//	std::cout << resume_link.As<std::string>() << std::endl ;

	std::istringstream ss( http::Put( resume_link.Get(), meta, hdr ) ) ;
	
	std::string line ;
	while ( std::getline( ss, line ) )
	{
		static const std::string location = "Location: " ;
		if ( line.substr( 0, location.size() ) == location )
		{
			std::string uplink = line.substr( location.size() ) ;
			uplink = uplink.substr( 0, uplink.size() -1 ) ;
			
			http::Headers uphdr ;
// 			uphdr.push_back( "Content-Type: application/octet-stream" ) ;
// 			uphdr.push_back( "Content-Range: bytes 0-999/1000" ) ;
			uphdr.push_back( "Expect:" ) ;
			uphdr.push_back( "Accept:" ) ;
			
			http::Put( uplink, data, uphdr ) ;
		}
	}
}

} // end of namespace
