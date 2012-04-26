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

#include "Collection.hh"

#include "protocol/Json.hh"

#include <cassert>

// OS specific library
#include <sys/stat.h>
#include <sys/types.h>

// for debugging
#include <iostream>

namespace gr {

Collection::Collection( const Json& entry ) :
	m_title		( entry["title"]["$t"].As<std::string>() ),
	m_href		( entry["link"].FindInArray( "rel", "self" )["href"].As<std::string>() ),
	m_parent	( 0 )
{
}

Collection::Collection(
	const std::string& title,
	const std::string& href ) :
	m_title	( title ),
	m_href	( href ),
	m_parent( 0 )
{
}

std::string Collection::ParentHref( const Json& entry )
{
	Json node ;
	return entry["link"].FindInArray( "rel", "http://schemas.google.com/docs/2007#parent", node ) ?
		 node["href"].As<std::string>() : std::string() ;
}

const std::string& Collection::Href() const
{
	return m_href ;
}

const std::string& Collection::Title() const
{
	return m_title ;
}

const Collection* Collection::Parent() const
{
	return m_parent ;
}

Collection* Collection::Parent()
{
	return m_parent ;
}

void Collection::AddChild( Collection *child )
{
	assert( child != 0 ) ;
	assert( child->m_parent == 0 ) ;
	assert( child != this ) ;

	child->m_parent = this ;
	m_child.push_back( child ) ;
}

bool Collection::IsCollection( const Json& entry )
{
	Json node ;
	return
		entry["category"].FindInArray( "scheme", "http://schemas.google.com/g/2005#kind", node ) &&
		node["label"].As<std::string>() == "folder" ;
}

void Collection::Swap( Collection& coll )
{
	m_title.swap( coll.m_title ) ;
	m_href.swap( coll.m_href ) ;
	std::swap( m_parent, coll.m_parent ) ;
	m_child.swap( coll.m_child ) ;
}

void Collection::CreateSubDir( const std::string& prefix )
{
	std::string dir = prefix + m_title ;
	mkdir( dir.c_str(), 0700 ) ;
	
	for ( std::vector<Collection*>::iterator i = m_child.begin() ; i != m_child.end() ; ++i )
	{
		assert( (*i)->m_parent == this ) ;
		(*i)->CreateSubDir( prefix + m_title + "/" ) ;
	}
}

std::string Collection::Path() const
{
	assert( m_parent != this ) ;
	return m_parent != 0 ? (m_parent->Path() + "/" + m_title) : m_title ;
}

} // end of namespace

namespace std
{
	void swap( gr::Collection& c1, gr::Collection& c2 )
	{
		c1.Swap( c2 ) ;
	}
}
