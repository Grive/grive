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

#include "CommonUri.hh"
#include "Entry.hh"

#include "http/Agent.hh"
#include "http/ResponseLog.hh"
#include "http/XmlResponse.hh"
#include "protocol/OAuth2.hh"
#include "util/Crypt.hh"
#include "util/DateTime.hh"
#include "util/Log.hh"
#include "util/OS.hh"
#include "util/Path.hh"
#include "xml/Node.hh"
#include "xml/NodeSet.hh"

// standard C++ library
#include <algorithm>
#include <cassert>
#include <fstream>
#include <map>
#include <sstream>

// for debugging only
#include <iostream>

namespace gr {

Drive::Drive( OAuth2& auth ) :
	m_auth( auth )
{
	m_http_hdr.push_back( "Authorization: Bearer " + m_auth.AccessToken() ) ;
	m_http_hdr.push_back( "GData-Version: 3.0" ) ;
	
	http::Agent http ;
	ConstructDirTree( &http ) ;
	
	http::XmlResponse xrsp ;
	http::ResponseLog log( "first-", ".xml", &xrsp ) ;
	
	http.Get( feed_base + "?showfolders=true&showroot=true", &log, m_http_hdr ) ;
	xml::Node resp = xrsp.Response() ;

	m_resume_link = resp["link"].
		Find( "@rel", "http://schemas.google.com/g/2005#resumable-create-media" )["@href"] ;
		
	bool has_next = false ;
	do
	{
		xml::NodeSet entries = resp["entry"] ;
		for ( xml::NodeSet::iterator i = entries.begin() ; i != entries.end() ; ++i )
		{
			Entry file( *i ) ;
			if ( file.Kind() != "folder" )
			{
				FolderListIterator pit = FindFolder( file.ParentHref() ) ;
				if ( pit != m_coll.end() && pit->IsInRootTree() )
					UpdateFile( file, *pit, &http ) ;
				else
					Log( "file \"%1%\" parent doesn't exist, ignored", file.Title() ) ;
			}
		}
		
		xml::NodeSet nss = resp["link"].Find( "@rel", "next" ) ;
		has_next = !nss.empty() ;

		if ( has_next )
		{
			http::ResponseLog log2( "second-", ".xml", &xrsp ) ;
			http.Get( nss["@href"], &log2, m_http_hdr ) ;
			resp = xrsp.Response() ;
		}
	} while ( has_next ) ;
}

Drive::~Drive( )
{
}

struct SortCollectionByHref
{
	bool operator()( const Collection& c1, const Collection& c2 ) const
	{
		return c1.SelfHref() < c2.SelfHref() ;
	}
} ;

Drive::FolderListIterator Drive::FindFolder( const std::string& href )
{
	// try to find the parent by its href
	std::pair<FolderListIterator,FolderListIterator> its =
		std::equal_range(
			m_coll.begin(),
			m_coll.end(),
			Collection( "", href ),
			SortCollectionByHref() ) ;
	
	 return (its.first != its.second) ? its.first : m_coll.end() ;
}

Drive::FolderListIterator Drive::Root( )
{
	FolderListIterator root = FindFolder( root_href ) ;
	assert( root != m_coll.end() ) ;
	return root ;
}

void Drive::ConstructDirTree( http::Agent *http )
{
	http::XmlResponse xml ;
	http::ResponseLog log( "dir-", ".xml", &xml ) ;
	
	http->Get( feed_base + "/-/folder?max-results=10&showroot=true", &log, m_http_hdr ) ;

	xml::Node resp = xml.Response() ;

	assert( m_coll.empty() ) ;
	m_coll.push_back( Collection( ".", root_href ) ) ;
	
	while ( true )
	{
		xml::NodeSet entries = resp["entry"] ;

		// first, get all collections from the query result
		for ( xml::NodeSet::iterator i = entries.begin() ; i != entries.end() ; ++i )
		{
			Entry e( *i ) ;
			if ( e.Kind() == "folder" )
			{
				if ( e.ParentHrefs().size() == 1 )
					m_coll.push_back( Collection( e ) ) ;
				else
					Log( "folder \"%1%\" has multiple parents, ignored", e.Title(), log::warning ) ;
			}
		}
		
		xml::NodeSet next = resp["link"].Find( "@rel", "next" ) ;
		if ( next.empty() )
			break ;

		http->Get( next["@href"], &xml, m_http_hdr ) ;
		resp = xml.Response() ;
	}

	// second, build up linkage between parent and child 
	std::sort( m_coll.begin(), m_coll.end(), SortCollectionByHref() ) ;
	for ( FolderListIterator i = m_coll.begin() ; i != m_coll.end() ; ++i )
	{
		FolderListIterator pit = FindFolder( i->ParentHref() ) ;
		if ( pit != m_coll.end() )
		{
			// it shouldn't happen, just in case
			if ( &*i == &*pit )
				Log( "the parent of folder %1% is itself, ignored.", i->Title(), log::warning ) ;
			else
				pit->AddChild( &*i ) ;
		}
		else
			Log( "can't find folder \"%1%\" (\"%2%\")", i->Title(), i->ParentHref(), log::warning ) ;
	}

	// lastly, iterating from the root, create the directories in the local file system
	assert( Root()->Parent() == 0 ) ;
	Root()->CreateSubDir( Path() ) ;
}

void Drive::UpdateFile( Entry& file, const Collection& parent, http::Agent *http )
{
	// only handle uploaded files
	if ( !file.Filename().empty() )
	{
		bool changed = true ;
		Path path = parent.Dir() / file.Filename() ;
		
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
				file.Download( http, path, m_http_hdr ) ;
			
			else
			{
				// re-reading the file
				ifile.seekg(0) ;
				file.Upload( http, ifile.rdbuf(), m_http_hdr ) ;
			}
		}
	}
	else
	{
		Log( "file \"%1%\" is a google document, ignored", file.Title() ) ;
	}
}

} // end of namespace
