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

#include "FolderSet.hh"
#include "CommonUri.hh"

#include "util/Destroy.hh"

#include <algorithm>

namespace gr {

using namespace details ;

FolderSet::FolderSet( ) :
	m_root( new Resource( ".", root_href ) )
{
	m_set.insert( m_root ) ;
}

FolderSet::FolderSet( const FolderSet& fs )
{
	const Set& s = fs.m_set.get<ByIdentity>() ;
	for ( Set::const_iterator i = s.begin() ; i != s.end() ; ++i )
	{
		Resource *c = new Resource( **i ) ;
		if ( c->SelfHref() == root_href )
			m_root = c ;
		
		m_set.insert( c ) ;
	}
}

FolderSet::~FolderSet( )
{
	// delete all pointers
	const Set& s = m_set.get<ByIdentity>() ;
	std::for_each( s.begin(), s.end(), Destroy() ) ;
}

Resource* FolderSet::Root()
{
	return m_root ;
}

const Resource* FolderSet::Root() const
{
	return m_root ;
}

void FolderSet::Swap( FolderSet& fs )
{
	m_set.swap( fs.m_set ) ;
}

FolderSet& FolderSet::operator=( const FolderSet& fs )
{
	FolderSet tmp( fs ) ;
	Swap( tmp ) ;
	return *this ;
}

Resource* FolderSet::FindByHref( const std::string& href )
{
	HrefMap& map = m_set.get<ByHref>() ;
	HrefMap::iterator i = map.find( href ) ;
	return i != map.end() ? *i : 0 ;
}

const Resource* FolderSet::FindByHref( const std::string& href ) const
{
	const HrefMap& map = m_set.get<ByHref>() ;
	HrefMap::const_iterator i = map.find( href ) ;
	return i != map.end() ? *i : 0 ;
}

///	Reinsert should be called when the ID/HREF were updated
bool FolderSet::ReInsert( Resource *coll )
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

void FolderSet::Insert( Resource *coll )
{
	m_set.insert( coll ) ;
}

void FolderSet::Erase( Resource *coll )
{
	Set& s = m_set.get<ByIdentity>() ;
	s.erase( s.find( coll ) ) ;
}

void FolderSet::Update( Resource *coll, const Entry& e )
{
	coll->Update( e ) ;
	ReInsert( coll ) ;
}

} // end of namespace
