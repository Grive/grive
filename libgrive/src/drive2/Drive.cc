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
#include "util/Exception.hh"

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
	NewResource( agent, folders )  ;
	
	// get all files
	Feed files( feeds::files ) ;
	NewResource( agent, files ) ;

	// build parent-child linkage between folders
	for ( details::DB::iterator i = m_db.begin() ; i != m_db.end() ; ++i )
	{
		Resource *parent = Find( (*i)->Parent() ), *child = *i ;
		assert( child != 0 ) ;
		
		if ( parent != 0 )
		{
			// initialize parent IDs
			parent->AddChild( child->ID() ) ;
			child->SetParent( parent->ID() ) ;
		}
	}
}

void Drive::NewResource( http::Agent *agent, Feed& items )
{
	assert( agent != 0 ) ;
	
	while ( items.Next( agent ) )
	{
		std::vector<Json> item_json = items.Content()["items"].AsArray() ;
		for ( std::vector<Json>::iterator i = item_json.begin() ; i != item_json.end() ; ++i )
			NewResource( *i ) ;
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
	// assume resource is directly under root
	std::string parent_id = m_root != 0 ? m_root->ID() : "" ;
	
	Json parents ;
	if ( item.Get( "parents", parents ) )
	{
		std::vector<std::string> pids ;
		parents.Select<std::string>( "id", std::back_inserter(pids) ) ;

		// only the first parent counts
		if ( !pids.empty() )
			parent_id = pids.front() ;
	}

	Resource *r = new Resource(
		item["id"].Str(),
		item["mimeType"].Str(),
		item["title"].Str(),
		parent_id ) ;
	
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
	if ( idx >= parent->ChildCount() )
		BOOST_THROW_EXCEPTION(
			Exception()
		) ;
	
	return Find( parent->At(idx) ) ;
}

const Resource* Drive::Parent( const Resource *child ) const
{
	return Find( child->Parent() ) ;
}

} } // end of namespace gr::v2

