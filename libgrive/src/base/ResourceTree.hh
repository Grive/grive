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

namespace details
{
	using namespace boost::multi_index ;
	struct ByMD5 {} ;
	struct ByHref {} ;
	struct ByIdentity {} ;
	struct BySize {} ;

	typedef multi_index_container<
		Resource*,
		indexed_by<
			hashed_non_unique<tag<ByHref>,	const_mem_fun<Resource, std::string,	&Resource::SelfHref> >,
			hashed_non_unique<tag<ByMD5>,	const_mem_fun<Resource, std::string,	&Resource::MD5> >,
			hashed_non_unique<tag<BySize>,	const_mem_fun<Resource, u64_t,			&Resource::Size> >,
			hashed_unique<tag<ByIdentity>,	identity<Resource*> >
		>
	> Folders ;
	
	typedef Folders::index<ByMD5>::type			MD5Map ;
	typedef Folders::index<ByHref>::type		HrefMap ;
	typedef Folders::index<BySize>::type		SizeMap ;
	typedef Folders::index<ByIdentity>::type	Set ;
	typedef std::pair<SizeMap::iterator, SizeMap::iterator> SizeRange ;
	typedef std::pair<MD5Map::iterator, MD5Map::iterator> MD5Range ;
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
	ResourceTree( const fs::path& rootFolder ) ;
	ResourceTree( const ResourceTree& fs ) ;
	~ResourceTree( ) ;
	
	Resource* FindByHref( const std::string& href ) ;
	const Resource* FindByHref( const std::string& href ) const ;
	details::MD5Range FindByMD5( const std::string& md5 ) ;
	details::SizeRange FindBySize( u64_t size ) ;

	bool ReInsert( Resource *coll ) ;
	
	void Insert( Resource *coll ) ;
	void Erase( Resource *coll ) ;
	void Update( Resource *coll, const Entry& e ) ;
	
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

} // end of namespace gr
