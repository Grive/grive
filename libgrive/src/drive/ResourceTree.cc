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

#include "ResourceTree.hh"
#include "CommonUri.hh"

#include "util/Destroy.hh"
#include "util/log/Log.hh"

#include <algorithm>
#include <cassert>

namespace gr { namespace v1 {

using namespace details ;

ResourceTree::ResourceTree( const fs::path& rootFolder ) :
    m_root(new Resource(rootFolder))
{
	m_set.insert( m_root ) ;
}

ResourceTree::ResourceTree( const ResourceTree& fs ) :
	m_root( 0 )
{
	const Set& s = fs.m_set.get<ByIdentity>() ;
	for ( Set::const_iterator i = s.begin() ; i != s.end() ; ++i )
	{
		Resource *c = new Resource( **i ) ;
		if ( c->SelfHref() == root_href )
			m_root = c ;
		
		m_set.insert( c ) ;
	}
	
	assert( m_root != 0 ) ;
}

ResourceTree::~ResourceTree( )
{
	Clear() ;
}

void ResourceTree::Clear()
{
	// delete all pointers
	const Set& s = m_set.get<ByIdentity>() ;
	std::for_each( s.begin(), s.end(), Destroy() ) ;
	
	m_set.clear() ;
	m_root = 0 ;
}

Resource* ResourceTree::Root()
{
	assert( m_root != 0 ) ;
	return m_root ;
}

const Resource* ResourceTree::Root() const
{
	assert( m_root != 0 ) ;
	return m_root ;
}

Resource* ResourceTree::FindByHref( const std::string& href )
{
	// for the resource that not yet have href (e.g. not yet fetched from server)
	// their href will be empty.
	if ( href.empty() )
		return 0 ;

	HrefMap& map = m_set.get<ByHref>() ;
	HrefMap::iterator i = map.find( href ) ;
	return i != map.end() ? *i : 0 ;
}

const Resource* ResourceTree::FindByHref( const std::string& href ) const
{
	const HrefMap& map = m_set.get<ByHref>() ;
	HrefMap::const_iterator i = map.find( href ) ;
	return i != map.end() ? *i : 0 ;
}

///	Reinsert should be called when the ID/HREF were updated
bool ResourceTree::ReInsert( Resource *coll )
{
	Set& s = m_set.get<ByIdentity>() ;
	Set::iterator i = s.find( coll ) ;
	if ( i != s.end() )
	{
		s.erase( i ) ;
		m_set.insert( coll ) ;
		return true ;
	}
	else
		return false ;
}

void ResourceTree::Insert( Resource *coll )
{
	m_set.insert( coll ) ;
}

void ResourceTree::Erase( Resource *coll )
{
	Set& s = m_set.get<ByIdentity>() ;
	s.erase( s.find( coll ) ) ;
}

void ResourceTree::Update( Resource *coll, const Entry& e, const DateTime& last_sync )
{
	assert( coll != 0 ) ;

	coll->FromRemote( e, last_sync ) ;
	ReInsert( coll ) ;
}

ResourceTree::iterator ResourceTree::begin()
{
	return m_set.get<ByIdentity>().begin() ;
}

ResourceTree::iterator ResourceTree::end()
{
	return m_set.get<ByIdentity>().end() ;
}

} } // end of namespace gr::v1
