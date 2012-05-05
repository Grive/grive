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

#include "File.hh"

#include "protocol/HTTP.hh"
#include "protocol/Json.hh"
#include "protocol/JsonResponse.hh"
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
	
	http::JsonResponse str ;
	http::Agent agent ;
	agent.Get( root_url + "?alt=json&showfolders=true", &str, m_http_hdr ) ;
	Json resp = str.Response() ;
	
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

void Drive::UpdateFile( const Json& entry )
{
	// only handle uploaded files
	if ( entry.Has( "docs$suggestedFilename" ) )
	{
		File file( entry ) ;
	
		bool changed = true ;
		Path path = Path() / file.Filename() ;

		// determine which folder the file belongs to
		if ( !file.Parent().empty() )
		{
			FolderListIterator pit = FindFolder( file.Parent() ) ;
			if ( pit != m_coll.end() )
				path = pit->Dir() / file.Filename() ;
		}

		// compare checksum first if file exists
		std::ifstream ifile( path.Str().c_str(), std::ios::binary | std::ios::in ) ;
		if ( ifile && file.ServerMD5() == crypt::MD5(ifile.rdbuf()) )
			changed = false ;
		
		// if the checksum is different, file is changed and we need to update
		if ( changed )
		{
			DateTime remote	= file.ServerModified() ;
			DateTime local	= ifile ? os::FileMTime( path ) : DateTime() ;
			
			// remote file is newer, download file
			if ( !ifile || remote > local )
			{
std::cout << "downloading " << path << std::endl ;
				file.Download( path, m_http_hdr ) ;
			}
			else
			{
std::cout << "local " << path << " is newer" << std::endl ;
				// re-reading the file
				ifile.seekg(0) ;
				
				if ( !file.Upload( ifile.rdbuf(), m_http_hdr ) )
std::cout << path << " is read only" << std::endl ;
			}
		}
	}
}

} // end of namespace
