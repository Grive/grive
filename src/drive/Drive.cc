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

// standard C++ library
#include <algorithm>
#include <cassert>
#include <fstream>
#include <map>

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
	m_root.CreateSubDir( "" ) ;
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

		bool changed = true ;
		std::string path = "./" + filename ;

		// determine which folder the file belongs to
		if ( !parent_href.empty() )
		{
			FolderListIterator pit = FindFolder( parent_href ) ;
			if ( pit != m_coll.end() )
				path = pit->Path() + "/" + filename ;
		}
		
//		std::cout << "file time: " << entry["updated"]["$t"].As<std::string>() << " " << remote << " " << local << std::endl ;
		
		// compare checksum first if file exists
		std::ifstream ifile( path.c_str(), std::ios::binary | std::ios::out ) ;
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
			DateTime local = ifile ? os::FileMTime( path ) : DateTime() ;
			
			// remote file is newer, download file
			if ( remote > local )
			{
std::cout << "downloading " << path << std::endl ;
				http::GetFile( url, path, m_http_hdr ) ;
				os::SetFileTime( path, remote ) ;
			}
			else
			{
std::cout << "local " << filename << " is newer" << std::endl ;
				UploadFile( entry ) ;
			}
		}
	}
}

void Drive::UploadFile( const Json& entry )
{
// 	std::cout << "entry:\n" << entry << std::endl ;
	
	Json resume_link = entry["link"].FindInArray( "rel",
		"http://schemas.google.com/g/2005#resumable-edit-media" )["href"] ;
	std::cout << resume_link.As<std::string>() << std::endl ;

	std::string resp = http::Put( resume_link.Get(), "", m_http_hdr ) ;
	
	std::cout << "resp " << resp ;
}

} // end of namespace
