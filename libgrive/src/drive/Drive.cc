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
// #include "http/ResponseLog.hh"
#include "http/XmlResponse.hh"
#include "protocol/Json.hh"
#include "protocol/OAuth2.hh"
#include "util/NotificationHandler.hh"
#include "util/Destroy.hh"
#include "util/Log.hh"
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

Drive::Drive( OAuth2& auth ) :
	m_auth( auth ),
	m_state( state_file )
{
	m_http_hdr.push_back( "Authorization: Bearer " + m_auth.AccessToken() ) ;
	m_http_hdr.push_back( "GData-Version: 3.0" ) ;

	http::Agent http ;
	http::XmlResponse xrsp ;
	http.Get( feed_metadata, &xrsp, m_http_hdr ) ;
	
	std::string change_stamp = xrsp.Response()["docs:largestChangestamp"]["@value"] ;
	Trace( "change stamp is %1%", change_stamp ) ;

	m_state.ChangeStamp( change_stamp ) ;
	m_state.FromLocal( "." ) ;
	
	ConstructDirTree( &http ) ;
	
	std::string uri = feed_base + "?showfolders=true&showroot=true" ;
/*	if ( !change_stamp.empty() )
	{
		int ichangestamp = std::atoi( change_stamp.c_str() ) + 1 ;
		uri = (boost::format( "%1%&start-index=%2%" ) % uri % ichangestamp ).str() ;
	}
*/
	http.Get( uri, &xrsp, m_http_hdr ) ;
	xml::Node resp = xrsp.Response() ;

	m_resume_link = resp["link"].
		Find( "@rel", "http://schemas.google.com/g/2005#resumable-create-media" )["@href"] ;

	bool has_next = false ;
	do
	{
		xml::NodeSet entries = resp["entry"] ;
		for ( xml::NodeSet::iterator i = entries.begin() ; i != entries.end() ; ++i )
		{
			if ( (*i)["content"] == "" )
				continue ;

			Entry file( *i ) ;
			if ( file.Kind() != "folder" )
			{
				Resource *p = m_state.FindFolderByHref( file.ParentHref() ) ;
				if ( file.Filename().empty() )
					Log( "file \"%1%\" is a google document, ignored", file.Title(), log::info ) ;
				
				else if ( file.ParentHref().empty() || p == 0 || !p->IsInRootTree() )
					Log( "file \"%1%\" parent doesn't exist, ignored", file.Title(), log::info ) ;
				
				else if ( p != 0 && !p->IsFolder() )
					Log( "entry %1% has parent %2% which is not a folder, ignored",
						file.Title(), p->Name(), log::info ) ;
				
				else
					m_state.FromRemote( file ) ;
			}
		}
		
		xml::NodeSet nss = resp["link"].Find( "@rel", "next" ) ;
		has_next = !nss.empty() ;

		if ( has_next )
		{
// 			http::ResponseLog log2( "second-", ".xml", &xrsp ) ;
			http.Get( nss["@href"], &xrsp, m_http_hdr ) ;
			resp = xrsp.Response() ;
		}
	} while ( has_next ) ;
}

void Drive::SaveState()
{
	m_state.Write( state_file ) ;
}

void Drive::ConstructDirTree( http::Agent *http )
{
	http::XmlResponse xml ;
// 	http::ResponseLog log( "dir-", ".xml", &xml ) ;
	
	http->Get( feed_base + "/-/folder?max-results=10&showroot=true", &xml, m_http_hdr ) ;

	xml::Node resp = xml.Response() ;

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
					m_state.FromRemote( e ) ;

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

	m_state.ResolveEntry() ;
}

void Drive::Update()
{
	http::Agent http ;
	std::for_each( m_state.begin(), m_state.end(),
		boost::bind( &Resource::Sync, _1, &http, m_http_hdr ) ) ;
    Notify::Commit();
}

} // end of namespace
