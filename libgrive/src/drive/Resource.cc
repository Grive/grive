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

#include "Resource.hh"
#include "CommonUri.hh"

#include "util/OS.hh"
#include "xml/Node.hh"
#include "xml/NodeSet.hh"

#include <cassert>

// for debugging
#include <iostream>

namespace gr {

Resource::Resource( const xml::Node& entry ) :
	m_entry		( entry ),
	m_parent	( 0 )
{
}

Resource::Resource( const Entry& entry ) :
	m_entry		( entry ),
	m_parent	( 0 )
{
}

Resource::Resource(
	const std::string& title,
	const std::string& href ) :
	m_entry	( title, href ),
	m_parent( 0 )
{
}

void Resource::Update( const Entry& e )
{
	m_entry = e ;
}

std::string Resource::SelfHref() const
{
	return m_entry.SelfHref() ;
}

std::string Resource::Title() const
{
	return m_entry.Title() ;
}

std::string Resource::ResourceID() const
{
	return m_entry.ResourceID() ;
}

const Resource* Resource::Parent() const
{
	return m_parent ;
}

Resource* Resource::Parent()
{
	return m_parent ;
}

std::string Resource::ParentHref() const
{
	return m_entry.ParentHref() ;
}

void Resource::AddChild( Resource *child )
{
	assert( child != 0 ) ;
	assert( child->m_parent == 0 ) ;
	assert( child != this ) ;

	child->m_parent = this ;
	m_child.push_back( child ) ;
}

void Resource::AddLeaf( File *file )
{
	m_leaf.push_back( file ) ;
}

void Resource::Swap( Resource& coll )
{
	m_entry.Swap( coll.m_entry ) ;
	std::swap( m_parent, coll.m_parent ) ;
	m_child.swap( coll.m_child ) ;
	m_leaf.swap( coll.m_leaf ) ;
}

void Resource::CreateSubDir( const fs::path& prefix )
{
	fs::path dir = prefix / m_entry.Title() ;
	fs::create_directories( dir ) ;
	
	for ( std::vector<Resource*>::iterator i = m_child.begin() ; i != m_child.end() ; ++i )
	{
		assert( (*i)->m_parent == this ) ;
		(*i)->CreateSubDir( dir ) ;
	}
}

fs::path Resource::Dir() const
{
	assert( m_parent != this ) ;
	return m_parent != 0 ? (m_parent->Dir() / m_entry.Title()) : "." ;
}

bool Resource::IsInRootTree() const
{
	return m_parent == 0 ? (SelfHref() == root_href) : m_parent->IsInRootTree() ;
}

Resource* Resource::FindChild( const std::string& title )
{
	for ( std::vector<Resource*>::iterator i = m_child.begin() ; i != m_child.end() ; ++i )
	{
		assert( (*i)->m_parent == this ) ;
		if ( (*i)->Title() == title )
			return *i ;
	}
	return 0 ;
}

} // end of namespace

namespace std
{
	void swap( gr::Resource& c1, gr::Resource& c2 )
	{
		c1.Swap( c2 ) ;
	}
}
