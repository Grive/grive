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
#include "Feed.hh"

#include "http/CurlAgent.hh"
#include "http/ResponseLog.hh"
#include "http/XmlResponse.hh"
#include "protocol/Json.hh"
#include "protocol/OAuth2.hh"
#include "util/Destroy.hh"
#include "util/log/Log.hh"
#include "xml/Node.hh"
#include "xml/NodeSet.hh"

#include <boost/bind.hpp>

// standard C++ library
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <map>
#include <sstream>

// for debugging only
#include <iostream>

namespace gr {

namespace
{
	const std::string state_file = ".grive_state" ;
}

Drive::Drive( OAuth2& auth, const Json& options ) :
	m_auth( auth ),
	m_state( state_file, options )
{
	m_http_hdr.Add( "Authorization: Bearer " + m_auth.AccessToken() ) ;
	m_http_hdr.Add( "GData-Version: 3.0" ) ;
}

void Drive::FromRemote( const Entry& entry )
{
	// entries from change feed does not have the parent HREF,
	// so these checkings are done in normal entries only
	Resource *parent = m_state.FindByHref( entry.ParentHref() ) ;
	
	if ( parent != 0 && !parent->IsFolder() )
		Log( "warning: entry %1% has parent %2% which is not a folder, ignored",
			entry.Title(), parent->Name(), log::verbose ) ;
	
	else if ( parent == 0 || !parent->IsInRootTree() )
		Log( "file \"%1%\" parent doesn't exist, ignored", entry.Title(), log::verbose ) ;
		
	else
		m_state.FromRemote( entry ) ;
}

void Drive::FromChange( const Entry& entry )
{
	if ( entry.IsRemoved() )
		Log( "file \"%1%\" represents a deletion, ignored", entry.Title(), log::verbose ) ;
	
	// folders go directly
	else
		m_state.FromRemote( entry ) ;
}

void Drive::SaveState()
{
	m_state.Write( state_file ) ;
}

void Drive::SyncFolders( http::Agent *http )
{
	assert( http != 0 ) ;

	Log( "Synchronizing folders", log::info ) ;

	http::XmlResponse xml ;
	http->Get( feed_base + "/-/folder?max-results=50&showroot=true", &xml, m_http_hdr ) ;
	
	Feed feed( xml.Response() ) ;
	do
	{
		// first, get all collections from the query result
		for ( Feed::iterator i = feed.begin() ; i != feed.end() ; ++i )
		{
			Entry e( *i ) ;
			if ( e.Kind() == "folder" )
			{
				if ( e.ParentHrefs().size() != 1 )
					Log( "folder \"%1%\" has multiple parents, ignored", e.Title(), log::verbose ) ;
				
				else if ( e.Title().find('/') != std::string::npos )
					Log( "folder \"%1%\" contains a slash in its name, ignored", e.Title(), log::verbose ) ;
				
				else
					m_state.FromRemote( e ) ;
			}
		}
	} while ( feed.GetNext( http, m_http_hdr ) ) ;

	m_state.ResolveEntry() ;
}

void Drive::DetectChanges()
{
	Log( "Reading local directories", log::info ) ;
	m_state.FromLocal( "." ) ;
	
	http::CurlAgent http ;

	long prev_stamp = m_state.ChangeStamp() ;
	Trace( "previous change stamp is %1%", prev_stamp ) ;
	
	SyncFolders( &http ) ;

	Log( "Reading remote server file list", log::info ) ;
	http::XmlResponse xrsp ;
	http.Get( feed_base + "?showfolders=true&showroot=true", &xrsp, m_http_hdr ) ;
	xml::Node resp = xrsp.Response() ;

	m_resume_link = resp["link"].
		Find( "@rel", "http://schemas.google.com/g/2005#resumable-create-media" )["@href"] ;

	Feed feed( resp ) ;
	do
	{
		std::for_each( feed.begin(), feed.end(), boost::bind( &Drive::FromRemote, this, _1 ) ) ;
	} while ( feed.GetNext( &http, m_http_hdr ) ) ;
	
	// pull the changes feed
	if ( prev_stamp != -1 )
	{
		http::ResponseLog log( "/tmp/changes-", ".xml", &xrsp ) ;
		Log( "Detecting changes from last sync", log::info ) ;
		http.Get( ChangesFeed(prev_stamp+1), &log, m_http_hdr ) ;
		
		Feed changes( xrsp.Response() ) ;
		std::for_each( changes.begin(), changes.end(), boost::bind( &Drive::FromChange, this, _1 ) ) ;
	}
}

void Drive::Update()
{
	Log( "Synchronizing files", log::info ) ;
	http::CurlAgent http ;
	m_state.Sync( &http, m_http_hdr ) ;
	
	UpdateChangeStamp( &http ) ;
}

void Drive::DryRun()
{
	Log( "Synchronizing files (dry-run)", log::info ) ;
	m_state.Sync( 0, m_http_hdr ) ;
}

void Drive::UpdateChangeStamp( http::Agent *http )
{
	assert( http != 0 ) ;

	// get changed feed
	http::XmlResponse xrsp ;
	http->Get( ChangesFeed(m_state.ChangeStamp()+1), &xrsp, m_http_hdr ) ;
	
	// we should go through the changes to see if it was really Grive to made that change
	// maybe by recording the updated timestamp and compare it?
	m_state.ChangeStamp( 
		std::atoi(xrsp.Response()["docs:largestChangestamp"]["@value"].front().Value().c_str()) ) ;
}

} // end of namespace
