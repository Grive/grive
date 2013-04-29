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

#include <cassert>

namespace gr { namespace v2 {

Drive::Drive( )
{
}

void Drive::Refresh( http::Agent *agent )
{
	// get all folders first
	Feed folders( feeds::files ) ;
	std::vector<std::pair<std::string, Resource*> > parent_child ;

	while ( folders.Next( agent ) )
	{
		std::vector<Json> items = folders.Content()["items"].AsArray() ;
		for ( std::vector<Json>::iterator i = items.begin() ; i != items.end() ; ++i )
		{
			Resource *r = NewResource( *i ) ;
			
			Json parents ;
			if ( i->Get( "parents", parents ) )
			{
				std::vector<std::string> pids ;
				parents.Select<std::string>( "id", std::back_inserter(pids) ) ;
				
				for ( std::vector<std::string>::iterator p = pids.begin() ; p != pids.end() ; ++p )
					parent_child.push_back( std::make_pair( *p, r ) ) ;
				
				// add to root node if no parent
				if ( pids.empty() )
					m_root.AddChild( r->ID() ) ;
			}
		}
	}
	
	for ( std::vector<std::pair<std::string, Resource*> >::iterator i = parent_child.begin() ;
		i != parent_child.end() ; ++i )
	{
		Resource *parent = Find( i->first ) ;
		if ( parent != 0 )
			parent->AddChild( i->second->ID() ) ;
	}
}

Resource* Drive::FindRoot( http::Agent *agent )
{
	// get all folders first
	Feed folders( feeds::files + "/root?fields=id" ) ;
	folders.Next( agent ) ;
	
	std::string id = folders.Content()["id"].Str() ;
	std::cout << "root = " << id << std::endl ;
	
	return Find( folders.Content()["id"].Str() ) ;
}

Resource* Drive::NewResource( const Json& item )
{
	Resource *r = new Resource( item["id"].Str(), item["mimeType"].Str(), item["title"].Str() ) ;
	
	// initialize parent IDs
	std::cout << r->Title() << " " << r->ID() << std::endl ;
	
	m_db.insert(r) ;
	assert( Find(r->ID()) == r ) ;
	
	return r ;
}

Resource* Drive::Find( const std::string& id )
{
	details::ID::iterator i = m_db.get<details::ByID>().find(id) ;
	return i != m_db.get<details::ByID>().end() ? *i : 0 ;
}

const Resource* Drive::Find( const std::string& id ) const
{
	details::ID::const_iterator i = m_db.get<details::ByID>().find(id) ;
	return i != m_db.get<details::ByID>().end() ? *i : 0 ;
}

Resource* Drive::Root()
{
	return &m_root ;
}

const Resource* Drive::Root() const
{
	return &m_root ;
}

const Resource* Drive::Child( const Resource *parent, std::size_t idx ) const
{
	return Find( parent->At(idx) ) ;
}

} } // end of namespace gr::v2

