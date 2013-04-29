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

Drive::Drive( ) :
	m_root( 0 )
{

}

void Drive::Refresh( http::Agent *agent )
{
	// find root node ID
	assert( m_root == 0 ) ;
	m_root = NewResource( agent, "root" ) ;
	
	// get all folders first
	Feed folders( feeds::files ) ;
	folders.Query( "mimeType", mime_types::folder ) ;
	std::vector<std::pair<std::string, Resource*> > parent_child ;
	while ( folders.Next( agent ) )
	{
		std::vector<Json> items = folders.Content()["items"].AsArray() ;
		for ( std::vector<Json>::iterator i = items.begin() ; i != items.end() ; ++i )
		{
			Resource *r = NewResource( *i ) ;
//std::cout << r->Title() << " " << r->ID() << std::endl ;

			Json parents ;
			if ( i->Get( "parents", parents ) )
			{
				std::vector<std::string> pids ;
				parents.Select<std::string>( "id", std::back_inserter(pids) ) ;

				// onlly the first parent counts
				if ( !pids.empty() )
					parent_child.push_back( std::make_pair( pids.front(), r ) ) ;
					
//				for ( std::vector<std::string>::iterator p = pids.begin() ; p != pids.end() ; ++p )
				{
//					std::cout << "parent = " << *p << std::endl ;
//					parent_child.push_back( std::make_pair( *p, r ) ) ;
				}
			}
		}
	}
	
	for ( std::vector<std::pair<std::string, Resource*> >::iterator i = parent_child.begin() ;
		i != parent_child.end() ; ++i )
	{
		Resource *parent = Find( i->first ), *child = i->second ;
		assert( child != 0 ) ;
		
		if ( parent != 0 )
		{
			std::cout << "parent of " << child->Title() << " is " << parent->Title() << std::endl ;
			
			// initialize parent IDs
			parent->AddChild( child->ID() ) ;
			child->SetParent( parent->ID() ) ;
		}
	}
}

/// Create resource base on ID
Resource* Drive::NewResource( http::Agent *agent, const std::string& id )
{
	Feed feed( feeds::files + "/" + id ) ;
	feed.Next( agent ) ;
	
	return NewResource( feed.Content() ) ;
}

Resource* Drive::NewResource( const Json& item )
{
	Resource *r = new Resource( item["id"].Str(), item["mimeType"].Str(), item["title"].Str() ) ;
	
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
	return m_root ;
}

const Resource* Drive::Root() const
{
	return m_root ;
}

const Resource* Drive::Child( const Resource *parent, std::size_t idx ) const
{
	return Find( parent->At(idx) ) ;
}

const Resource* Drive::Parent( const Resource *child ) const
{
	return Find( child->Parent() ) ;
}

} } // end of namespace gr::v2

