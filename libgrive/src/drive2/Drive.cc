/*
	grive: an GPL program to sync a local directory with Google Drive
	Copyright (C) 2013 Wan Wai Ho

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
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
	MA  02110-1301, USA.
*/

#include "Drive.hh"

#include "CommonUri.hh"
#include "Feed.hh"
#include "protocol/Json.hh"

#include <iostream>
#include <iterator>

namespace gr { namespace v2 {

Drive::Drive( )
{
}

void Drive::Refresh( http::Agent *agent )
{
	// get all folders first
//	Feed folders( 
//		"https://www.googleapis.com/drive/v2/files?q=mimeType+%3d+%27application/vnd.google-apps.folder%27" ) ;
	Feed folders( feeds::files ) ;
//	folders.Query( "mimeType", "application/vnd.google-apps.folder" ) ;

	while ( folders.Next( agent ) )
	{
		std::vector<Json> items = folders.Content()["items"].AsArray() ;
		for ( std::vector<Json>::iterator i = items.begin() ; i != items.end() ; ++i )
		{
			const Resource *r = Add( *i ) ;
		}
	}
}

const Resource* Drive::Add( const Json& item )
{
	Resource *r = new Resource( item["id"].Str(), item["mimeType"].Str(), item["title"].Str() ) ;
	
	// initialize parent IDs
	Json parents ;
	if ( item.Get( "parents", parents ) )
	{
		std::vector<std::string> parent_ids ;
		parents.Select<std::string>( "id", std::back_inserter(parent_ids) ) ;
		
		r->SetParent( parent_ids.begin(), parent_ids.end() ) ;
		std::cout << r->Title() << " " << r->ID() << " " << parent_ids.size() << std::endl ;
	}
	
	return *m_db.insert(r).first ;
}

Resource* Drive::Find( const std::string& id )
{
	details::ID::iterator i = m_db.get<details::ByID>().find(id) ;
	return i != m_db.get<details::ByID>().end() ? *i : 0 ;
}

} } // end of namespace gr::v2

