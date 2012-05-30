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

#pragma once

#include "Resource.hh"

#include "util/FileSystem.hh"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/mem_fun.hpp>

namespace gr {

class Json ;

namespace details
{
	using namespace boost::multi_index ;
	struct ByID {} ;
	struct ByHref {} ;
	struct ByIdentity {} ;

	typedef multi_index_container<
		Resource*,
		indexed_by<
			hashed_non_unique<tag<ByHref>,	const_mem_fun<Resource, std::string,	&Resource::SelfHref> >,
			hashed_non_unique<tag<ByID>,	const_mem_fun<Resource, std::string,	&Resource::ResourceID> >,
			hashed_unique<tag<ByIdentity>,	identity<Resource*> >
		>
	> Folders ;
	
	typedef Folders::index<ByID>::type			IDMap ;
	typedef Folders::index<ByHref>::type		HrefMap ;
	typedef Folders::index<ByIdentity>::type	Set ;
}

/*!	\brief	A simple container for storing folders

	This class stores a set of folders and provide fast search access from ID, HREF etc.
	It is a wrapper around multi_index_container from Boost library.
*/
class ResourceTree
{
public :
	typedef details::Set::iterator iterator ;

public :
	ResourceTree( ) ;
	ResourceTree( const ResourceTree& fs ) ;
	~ResourceTree( ) ;
	
	void Swap( ResourceTree& fs ) ;
	ResourceTree& operator=( const ResourceTree& fs ) ;
	
	Resource* FindByHref( const std::string& href ) ;
	const Resource* FindByHref( const std::string& href ) const ;

	Resource* FindByPath( const fs::path& path ) ;
	Resource* FindByID( const std::string& id ) ;
	
	bool ReInsert( Resource *coll ) ;
	
	void Insert( Resource *coll ) ;
	void Erase( Resource *coll ) ;
	void Update( Resource *coll, const Entry& e, const DateTime& last_sync ) ;
	
	Resource* Root() ;
	const Resource* Root() const ;
	
	iterator begin() ;
	iterator end() ;

private :
	void Clear() ;

private :
	details::Folders	m_set ;
	Resource*			m_root ;
} ;

} // end of namespace
